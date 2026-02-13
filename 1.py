"""
yt_downloader_pyside6.py
Single-file PySide6 GUI YouTube downloader using yt-dlp to extract stream URLs
and a multi-part, resumable HTTP downloader for fast segmented downloads.

Dependencies:
    pip install PySide6 yt-dlp requests

NOTE: Use only for content you have the right to download.
"""

import os
import sys
import math
import threading
import traceback
from urllib.parse import urlparse
from functools import partial

import requests
from yt_dlp import YoutubeDL

from PySide6.QtCore import Qt, Signal, QObject, QThread
from PySide6.QtWidgets import (
    QApplication, QWidget, QVBoxLayout, QHBoxLayout, QLabel, QLineEdit,
    QPushButton, QFileDialog, QSpinBox, QProgressBar, QTextEdit, QMessageBox,
    QComboBox, QCheckBox
)


# -------------------------
# Worker + download engine
# -------------------------
class WorkerSignals(QObject):
    started = Signal()
    progress = Signal(int)            # overall percent
    part_progress = Signal(int, int)  # (part_index, percent)
    log = Signal(str)
    finished = Signal(bool, str)      # success, message


class DownloadWorker(QThread):
    def __init__(self, url, dest_folder, parts=4, filename=None, keep_temp=True, prefer_audio=False):
        super().__init__()
        self.url = url
        self.dest_folder = dest_folder
        self.parts = max(1, int(parts))
        self.filename = filename
        self.keep_temp = keep_temp
        self.signals = WorkerSignals()
        self._pause_flag = threading.Event()
        self._pause_flag.set()  # when set -> not paused
        self._cancel_flag = threading.Event()
        self.prefer_audio = prefer_audio

    def pause(self):
        self._pause_flag.clear()
        self.signals.log.emit("Paused by user.")

    def resume(self):
        self._pause_flag.set()
        self.signals.log.emit("Resumed by user.")

    def cancel(self):
        self._cancel_flag.set()
        self._pause_flag.set()  # wake up if paused
        self.signals.log.emit("Cancel requested.")

    def run(self):
        try:
            self.signals.started.emit()
            self.signals.log.emit("Extracting video info with yt-dlp...")
            info = self._extract_info(self.url)
            if not info:
                self.signals.finished.emit(False, "Failed to extract video info.")
                return

            # Choose a format/url to download:
            fmt_url, total_size, ext, title = self._choose_format_and_url(info)
            if not fmt_url:
                self.signals.finished.emit(False, "No downloadable direct URL found for selected format.")
                return

            # build filename
            safe_title = self._safe_filename(self.filename or title or "video")
            out_ext = ext or "mp4"
            out_name = f"{safe_title}.{out_ext}"
            out_path = os.path.join(self.dest_folder, out_name)

            # If file exists already, inform and skip
            if os.path.exists(out_path):
                self.signals.log.emit(f"File already exists: {out_path}")
                self.signals.finished.emit(True, f"Already downloaded: {out_path}")
                return

            # Do the segmented resumable download
            self.signals.log.emit(f"Starting segmented download ({self.parts} parts) to: {out_path}")
            ok, msg = self._segmented_download(fmt_url, out_path, total_size)
            self.signals.finished.emit(ok, msg)
        except Exception as e:
            tb = traceback.format_exc()
            self.signals.log.emit(f"Error: {e}\n{tb}")
            self.signals.finished.emit(False, str(e))

    def _extract_info(self, url):
        ydl_opts = {
            'quiet': True,
            'skip_download': True,
            'nocheckcertificate': True,
            # do not let yt-dlp download; we only want metadata and direct URLs
        }
        with YoutubeDL(ydl_opts) as ydl:
            try:
                info = ydl.extract_info(url, download=False)
                return info
            except Exception as e:
                self.signals.log.emit(f"yt-dlp extract_info failed: {e}")
                return None

    def _choose_format_and_url(self, info):
        # Attempt to pick a progressive (audio+video) format first. If prefer_audio, pick best audio.
        formats = info.get('formats') or []
        title = info.get('title') or info.get('id') or "download"
        # If formats present, choose best progressive (has 'acodec' and 'vcodec' not 'none')
        progressive_candidates = []
        audio_candidates = []
        for f in formats:
            url = f.get('url')
            if not url:
                continue
            acodec = f.get('acodec', 'none')
            vcodec = f.get('vcodec', 'none')
            filesize = f.get('filesize') or f.get('filesize_approx') or 0
            ext = f.get('ext') or 'mp4'
            if acodec != 'none' and vcodec != 'none':
                progressive_candidates.append((filesize or 0, url, ext))
            elif acodec != 'none' and vcodec == 'none':
                audio_candidates.append((filesize or 0, url, ext))

        if self.prefer_audio and audio_candidates:
            # pick largest audio
            audio_candidates.sort(reverse=True)
            size, url, ext = audio_candidates[0]
            return url, size, ext, title

        if progressive_candidates:
            progressive_candidates.sort(reverse=True)
            size, url, ext = progressive_candidates[0]
            return url, size, ext, title

        # fallback: sometimes info itself has 'url' (e.g., single format)
        if info.get('url'):
            # unknown size
            return info.get('url'), info.get('filesize') or info.get('filesize_approx') or 0, info.get('ext'), title

        return None, None, None, title

    def _safe_filename(self, s):
        return "".join(c for c in s if c.isalnum() or c in " .-_").rstrip()

    def _segmented_download(self, url, out_path, total_size_hint):
        """
        Download `url` to `out_path` by splitting into `self.parts` HTTP Range parts.
        Supports resume: part files are named out_path.part0 .. .partN
        """
        session = requests.Session()
        # first do a HEAD to get content length and confirm "accept-ranges"
        try:
            head = session.head(url, allow_redirects=True, timeout=15)
        except Exception as e:
            self.signals.log.emit(f"HEAD failed: {e}. Trying GET to probe size.")
            head = session.get(url, stream=True, allow_redirects=True, timeout=15)
        if head is None:
            return False, "Failed to contact server."

        # try content-length
        content_length = None
        try:
            content_length = int(head.headers.get('content-length') or 0)
        except:
            content_length = 0

        if content_length == 0 and total_size_hint:
            content_length = total_size_hint or 0

        accept_ranges = head.headers.get('accept-ranges', '') != ''
        # If server doesn't accept ranges, fallback to single-threaded download with resume support
        if not accept_ranges or self.parts == 1:
            self.signals.log.emit("Server doesn't support ranges or single part requested; using single-stream resume.")
            return self._single_stream_download(session, url, out_path, content_length)

        # compute part ranges
        total = content_length
        if total <= 0:
            # Unknown size, fallback to single stream
            self.signals.log.emit("Unknown content length; falling back to single-stream download.")
            return self._single_stream_download(session, url, out_path, content_length)

        part_size = total // self.parts
        ranges = []
        for i in range(self.parts):
            start = i * part_size
            end = ((i + 1) * part_size - 1) if i < self.parts - 1 else (total - 1)
            ranges.append((start, end))

        # Prepare temp part files and check existing size to resume
        part_paths = [f"{out_path}.part{i}" for i in range(self.parts)]
        part_current = []
        for i, p in enumerate(part_paths):
            if os.path.exists(p):
                cur = os.path.getsize(p)
            else:
                cur = 0
            part_current.append(cur)

        overall_downloaded = sum(part_current)
        last_overall_percent = -1

        # Start threads per part
        threads = []
        thread_exceptions = []
        lock = threading.Lock()

        def download_range(i, rng):
            nonlocal overall_downloaded, last_overall_percent
            ppath = part_paths[i]
            expected_len = rng[1] - rng[0] + 1
            cur = os.path.getsize(ppath) if os.path.exists(ppath) else 0
            # compute start byte for resume
            start_byte = rng[0] + cur
            if start_byte > rng[1]:
                # this part is already complete
                self.signals.part_progress.emit(i, 100)
                return

            headers = {'Range': f'bytes={start_byte}-{rng[1]}'}
            try:
                with session.get(url, headers=headers, stream=True, timeout=30) as r:
                    r.raise_for_status()
                    mode = 'ab' if cur > 0 else 'wb'
                    with open(ppath, mode) as fh:
                        downloaded_in_part = cur
                        chunk_size = 1024 * 64
                        for chunk in r.iter_content(chunk_size=chunk_size):
                            # Pause support
                            while not self._pause_flag.is_set():
                                if self._cancel_flag.is_set():
                                    self.signals.log.emit("Cancel detected in part thread.")
                                    return
                                self.msleep(100)
                            if self._cancel_flag.is_set():
                                self.signals.log.emit("Cancel detected in part thread.")
                                return
                            if chunk:
                                fh.write(chunk)
                                downloaded_in_part += len(chunk)
                                with lock:
                                    overall_downloaded += len(chunk)
                                # per-part percent
                                percent_part = int((downloaded_in_part / expected_len) * 100)
                                self.signals.part_progress.emit(i, percent_part)
                                # overall percent
                                percent_overall = int((overall_downloaded / total) * 100)
                                if percent_overall != last_overall_percent:
                                    last_overall_percent = percent_overall
                                    self.signals.progress.emit(percent_overall)
            except Exception as e:
                thread_exceptions.append((i, str(e)))
                self.signals.log.emit(f"Exception in part {i}: {e}")

        # Launch threads
        for i, rng in enumerate(ranges):
            t = threading.Thread(target=download_range, args=(i, rng), daemon=True)
            threads.append(t)
            t.start()

        # Wait for finish or cancel
        while any(t.is_alive() for t in threads):
            if self._cancel_flag.is_set():
                self.signals.log.emit("Cancel requested; waiting threads to stop...")
                break
            self.msleep(200)

        # If cancelled, do not combine
        if self._cancel_flag.is_set():
            return False, "Canceled by user."

        if thread_exceptions:
            return False, f"Thread errors: {thread_exceptions}"

        # Combine parts
        self.signals.log.emit("Combining part files...")
        try:
            with open(out_path, 'wb') as out_f:
                for i, p in enumerate(part_paths):
                    with open(p, 'rb') as pf:
                        while True:
                            chunk = pf.read(1024 * 1024)
                            if not chunk:
                                break
                            out_f.write(chunk)
            # Optionally remove parts
            if not self.keep_temp:
                for p in part_paths:
                    try:
                        os.remove(p)
                    except:
                        pass
            self.signals.log.emit(f"Download complete: {out_path}")
            return True, out_path
        except Exception as e:
            return False, f"Failed to combine parts: {e}"

    def _single_stream_download(self, session, url, out_path, total):
        temp_path = out_path + ".part"
        current = os.path.getsize(temp_path) if os.path.exists(temp_path) else 0
        headers = {}
        if current:
            headers['Range'] = f'bytes={current}-'
        try:
            with session.get(url, headers=headers, stream=True, timeout=30) as r:
                r.raise_for_status()
                mode = 'ab' if current else 'wb'
                total_expected = total or int(r.headers.get('content-length') or 0) + current
                downloaded = current
                with open(temp_path, mode) as fh:
                    for chunk in r.iter_content(chunk_size=1024*64):
                        while not self._pause_flag.is_set():
                            if self._cancel_flag.is_set():
                                self.signals.log.emit("Canceled.")
                                return False, "Canceled."
                            self.msleep(100)
                        if self._cancel_flag.is_set():
                            return False, "Canceled."
                        if chunk:
                            fh.write(chunk)
                            downloaded += len(chunk)
                            percent = int((downloaded / total_expected) * 100) if total_expected else 0
                            self.signals.progress.emit(percent)
                # Move temp to final
                os.replace(temp_path, out_path)
                self.signals.log.emit(f"Download complete: {out_path}")
                return True, out_path
        except Exception as e:
            self.signals.log.emit(f"Single-stream download failed: {e}")
            return False, str(e)


# -------------------------
# GUI
# -------------------------
class MainWindow(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Modern YT Downloader (PySide6 + yt-dlp)")
        self.setMinimumSize(760, 420)
        self._worker = None

        main = QVBoxLayout(self)

        # URL area
        url_layout = QHBoxLayout()
        self.url_edit = QLineEdit()
        self.url_edit.setPlaceholderText("Paste YouTube / video URL here")
        url_layout.addWidget(QLabel("URL:"))
        url_layout.addWidget(self.url_edit)
        main.addLayout(url_layout)

        # Destination chooser
        dest_layout = QHBoxLayout()
        self.dest_label = QLabel("(no folder selected)")
        btn_folder = QPushButton("Choose folder")
        btn_folder.clicked.connect(self.choose_folder)
        dest_layout.addWidget(QLabel("Save to:"))
        dest_layout.addWidget(self.dest_label)
        dest_layout.addWidget(btn_folder)
        main.addLayout(dest_layout)

        # options row: parts, keep temp, prefer audio
        opt_layout = QHBoxLayout()
        opt_layout.addWidget(QLabel("Parts:"))
        self.parts_spin = QSpinBox(value=4, minimum=1, maximum=32)
        opt_layout.addWidget(self.parts_spin)
        self.keep_temp_cb = QCheckBox("Keep part files (for resume / debugging)")
        self.keep_temp_cb.setChecked(False)
        opt_layout.addWidget(self.keep_temp_cb)
        self.audio_only_cb = QCheckBox("Prefer audio-only (if available)")
        opt_layout.addWidget(self.audio_only_cb)
        opt_layout.addStretch()
        main.addLayout(opt_layout)

        # filename override
        fn_layout = QHBoxLayout()
        self.fn_edit = QLineEdit()
        self.fn_edit.setPlaceholderText("Optional filename (without extension)")
        fn_layout.addWidget(QLabel("Filename:"))
        fn_layout.addWidget(self.fn_edit)
        main.addLayout(fn_layout)

        # progress bars and per-part display
        self.overall_progress = QProgressBar(value=0)
        self.overall_progress.setFormat("Overall: %p%")
        main.addWidget(self.overall_progress)

        # Part progress area (dynamic)
        self.part_progress_layout = QVBoxLayout()
        main.addLayout(self.part_progress_layout)
        self._part_bars = []

        # Buttons + log
        btn_layout = QHBoxLayout()
        self.btn_start = QPushButton("Start")
        self.btn_start.clicked.connect(self.start_download)
        self.btn_pause = QPushButton("Pause")
        self.btn_pause.clicked.connect(self.pause_download)
        self.btn_resume = QPushButton("Resume")
        self.btn_resume.clicked.connect(self.resume_download)
        self.btn_cancel = QPushButton("Cancel")
        self.btn_cancel.clicked.connect(self.cancel_download)
        for b in (self.btn_pause, self.btn_resume, self.btn_cancel):
            b.setEnabled(False)
        btn_layout.addWidget(self.btn_start)
        btn_layout.addWidget(self.btn_pause)
        btn_layout.addWidget(self.btn_resume)
        btn_layout.addWidget(self.btn_cancel)
        btn_layout.addStretch()
        main.addLayout(btn_layout)

        # Log / status
        self.log = QTextEdit()
        self.log.setReadOnly(True)
        self.log.setFixedHeight(140)
        main.addWidget(self.log)

    def choose_folder(self):
        folder = QFileDialog.getExistingDirectory(self, "Choose download folder", os.path.expanduser("~"))
        if folder:
            self.dest_label.setText(folder)

    def start_download(self):
        url = self.url_edit.text().strip()
        dest = self.dest_label.text()
        parts = self.parts_spin.value()
        filename = self.fn_edit.text().strip() or None
        keep_temp = self.keep_temp_cb.isChecked()
        prefer_audio = self.audio_only_cb.isChecked()

        if not url:
            QMessageBox.warning(self, "Missing URL", "Please paste the video URL first.")
            return
        if dest.startswith("(") or not os.path.isdir(dest):
            QMessageBox.warning(self, "Missing folder", "Please choose a valid destination folder.")
            return

        # Prepare part bars area
        self._clear_part_bars()
        for i in range(parts):
            lbl = QLabel(f"Part {i+1}:")
            bar = QProgressBar(value=0)
            bar.setFormat(f"Part {i+1}: %p%")
            h = QHBoxLayout()
            h.addWidget(lbl)
            h.addWidget(bar)
            self.part_progress_layout.addLayout(h)
            self._part_bars.append(bar)

        # disable start, enable others
        self.btn_start.setEnabled(False)
        self.btn_pause.setEnabled(True)
        self.btn_cancel.setEnabled(True)
        self.btn_resume.setEnabled(False)

        # start worker
        self._worker = DownloadWorker(url, dest, parts=parts, filename=filename, keep_temp=keep_temp, prefer_audio=prefer_audio)
        self._worker.signals.log.connect(self._append_log)
        self._worker.signals.progress.connect(self._update_overall_progress)
        self._worker.signals.part_progress.connect(self._update_part_progress)
        self._worker.signals.finished.connect(self._on_finished)
        self._worker.signals.started.connect(lambda: self._append_log("Worker started."))
        self._worker.start()

    def pause_download(self):
        if self._worker:
            self._worker.pause()
            self.btn_pause.setEnabled(False)
            self.btn_resume.setEnabled(True)

    def resume_download(self):
        if self._worker:
            self._worker.resume()
            self.btn_resume.setEnabled(False)
            self.btn_pause.setEnabled(True)

    def cancel_download(self):
        if self._worker:
            self._worker.cancel()
            self.btn_cancel.setEnabled(False)

    def _append_log(self, text):
        self.log.append(text)

    def _update_overall_progress(self, percent):
        self.overall_progress.setValue(percent)

    def _update_part_progress(self, part_index, percent):
        if 0 <= part_index < len(self._part_bars):
            self._part_bars[part_index].setValue(percent)

    def _on_finished(self, success, message):
        if success:
            self._append_log(f"Finished: {message}")
            QMessageBox.information(self, "Finished", f"Download finished: {message}")
        else:
            self._append_log(f"Failed: {message}")
            QMessageBox.critical(self, "Failed", f"Download failed: {message}")

        # reset buttons
        self.btn_start.setEnabled(True)
        self.btn_pause.setEnabled(False)
        self.btn_resume.setEnabled(False)
        self.btn_cancel.setEnabled(False)

    def _clear_part_bars(self):
        # remove existing widgets/layout items from layout
        while self.part_progress_layout.count():
            item = self.part_progress_layout.takeAt(0)
            if item is None:
                continue
            # try to delete layout children
            w = item.widget()
            if w is not None:
                w.deleteLater()


def main():
    app = QApplication(sys.argv)
    w = MainWindow()
    w.show()
    sys.exit(app.exec())


if __name__ == "__main__":
    main()
