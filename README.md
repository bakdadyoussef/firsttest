# NumPad Mouse Control

A Windows 10/11 accessibility utility that turns the numeric keypad into a configurable mouse controller. It includes a PySide6 GUI, global keypad interception while mouse mode is enabled, pointer movement, clicking, dragging, scrolling, profiles, JSON settings, and system-tray operation.

> **Important:** This is a first runnable implementation of the project plan. The Windows low-level keyboard hook and input injection are Windows-specific and cannot be fully integration-tested from Linux. Test the app with mouse mode disabled first, then enable it while you have a working alternative way to control the PC (for example, Windows on-screen keyboard or a borrowed mouse). The app starts with interception disabled by default.

## Features

- Windows low-level keyboard hook using `ctypes` (no third-party keyboard-hook package).
- Suppresses recognized physical numeric-keypad keys only while mouse mode is active.
- Eight-direction movement, acceleration, speed limit, precision mode, and diagonal normalization.
- Left/right/middle clicks, hold-to-drag, vertical and horizontal scrolling.
- Editable key mappings and Balanced / Precision / Fast profiles.
- Persistent, validated JSON configuration in the user's application-data directory.
- System-tray operation, activation shortcut, emergency stop, diagnostics, and cleanup on shutdown.
- Starts with mouse mode disabled by default.

## Requirements

- Windows 10 or Windows 11 (64-bit recommended)
- Python 3.10+ supported by your selected PySide6 release
- PySide6

## Run from source

Open PowerShell in this folder:

```powershell
py -m venv .venv
.venv\Scripts\Activate.ps1
python -m pip install --upgrade pip
pip install -r requirements.txt
python main.py
```

If PowerShell blocks activation, run `.venv\Scripts\python.exe main.py` after installing dependencies with `.venv\Scripts\python.exe -m pip install -r requirements.txt`.

## Build a Windows executable

```powershell
.venv\Scripts\pyinstaller.exe --noconfirm --clean --windowed --name NumPadMouse main.py
```

The executable will be in `dist\NumPadMouse\` (or `dist\NumPadMouse.exe`, depending on PyInstaller mode/version). The app does not require administrator privileges for normal use. Some elevated applications and secure Windows screens may not accept injected input from a non-elevated process.

## Default keypad map

| Key | Action |
|---|---|
| KP 7 / 8 / 9 | Up-left / up / up-right |
| KP 4 / 5 / 6 | Left / stop / right |
| KP 1 / 2 / 3 | Down-left / down / down-right |
| KP 0 | Hold left mouse button (drag) |
| KP Decimal | Left click |
| KP Divide | Right click |
| KP Multiply | Middle click |
| KP Add / Subtract | Scroll up / down |

While mouse mode is active, recognized keypad keys are suppressed even if a key is mapped to `unassigned`. Disable mouse mode to return the keypad to ordinary input. The top-row number keys are not intentionally intercepted.

## Controls

- **F8**: default global toggle shortcut (configurable to F6–F11 in Background settings).
- **Ctrl+Alt+Esc**: default emergency stop (configurable to Ctrl+Alt+F10 or Ctrl+Alt+F11); stops movement, releases controller-held buttons, and disables mouse mode.
- Tray menu: enable/disable, open settings, emergency stop, exit.

The emergency shortcut is checked by the low-level hook and is not guaranteed to work on secure desktops or if Windows has stopped delivering hook events. Always test it before relying on it.

## Configuration

Configuration is stored in the current user's application-configuration directory under `config.json`. The GUI has a button to open the configuration folder. The application validates settings and falls back to a last-known-good backup or safe defaults when a file is invalid. Startup-at-login is managed per-user through the Windows Run registry key; it does not require administrator privileges.

## Notes and known limitations

- The Windows hook is implemented with a dedicated thread and Windows message loop. The callback is kept short and does not write files or update widgets directly.
- Keypad identification uses physical scan-code information and extended-key flags. Keyboard firmware and unusual layouts can vary, so verify the detected keymap on your hardware.
- The update interval is a target, not a real-time guarantee; Windows scheduling and target applications affect latency.
- Horizontal wheel behavior varies between applications.
- The program only releases mouse buttons that its own controller tracks as held.
- This project intentionally does not collect or log arbitrary keyboard text.
- No application can guarantee recovery from every process crash, operating-system failure, secure desktop, or hardware problem.

## Development checks

Run the platform-independent tests on any OS:

```powershell
python -m pip install pytest
python -m pytest
```

The low-level hook and SendInput integration must additionally be tested on Windows. On non-Windows systems, the GUI reports that mouse control is unavailable rather than attempting to install a Windows hook. The package was syntax-checked in the build environment, but the native Windows hook and the PySide6 GUI still require runtime testing on a Windows machine.
