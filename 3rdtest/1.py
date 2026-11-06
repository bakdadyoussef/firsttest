import tkinter as tk
from tkinter import messagebox
import pyautogui
import threading
import time

class MouseHoldApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Mouse Hold Tool")
        self.root.geometry("320x220")
        self.root.resizable(False, False)

        self.is_holding = False
        self.countdown_running = False

        # Delay input
        tk.Label(root, text="Delay before hold (seconds):", font=("Arial", 11)).pack(pady=(15, 5))
        
        self.delay_var = tk.StringVar(value="3")
        self.delay_entry = tk.Entry(root, textvariable=self.delay_var, font=("Arial", 12), width=10, justify="center")
        self.delay_entry.pack()

        # Countdown display
        self.counter_label = tk.Label(root, text="Ready", font=("Arial", 16, "bold"), fg="blue")
        self.counter_label.pack(pady=10)

        # Start button
        self.start_btn = tk.Button(root, text="Start Hold", font=("Arial", 12, "bold"),
                                   bg="#4CAF50", fg="white", width=15, command=self.start_countdown)
        self.start_btn.pack(pady=5)

        # Release button
        self.release_btn = tk.Button(root, text="Release Mouse", font=("Arial", 12, "bold"),
                                     bg="#f44336", fg="white", width=15, command=self.release_mouse)
        self.release_btn.pack(pady=5)

        # Status
        self.status_label = tk.Label(root, text="Status: Idle", font=("Arial", 10), fg="gray")
        self.status_label.pack(pady=10)

        # Safety: release mouse if window is closed
        self.root.protocol("WM_DELETE_WINDOW", self.on_close)

    def start_countdown(self):
        if self.countdown_running or self.is_holding:
            return

        try:
            delay = float(self.delay_var.get())
            if delay < 0:
                raise ValueError
        except ValueError:
            messagebox.showerror("Error", "Please enter a valid positive number")
            return

        self.countdown_running = True
        self.start_btn.config(state="disabled")
        self.status_label.config(text="Status: Counting down...", fg="orange")

        # Run countdown in a separate thread so GUI stays responsive
        threading.Thread(target=self.countdown, args=(delay,), daemon=True).start()

    def countdown(self, delay):
        remaining = delay
        while remaining > 0 and self.countdown_running:
            self.counter_label.config(text=f"{remaining:.1f}")
            time.sleep(0.1)
            remaining -= 0.1

        if self.countdown_running:
            self.counter_label.config(text="HOLDING!")
            self.hold_mouse()

        self.countdown_running = False
        self.root.after(0, lambda: self.start_btn.config(state="normal"))

    def hold_mouse(self):
        if not self.is_holding:
            pyautogui.mouseDown(button='left')
            self.is_holding = True
            self.root.after(0, lambda: self.status_label.config(text="Status: Mouse LEFT is HELD", fg="red"))

    def release_mouse(self):
        if self.is_holding:
            pyautogui.mouseUp(button='left')
            self.is_holding = False
            self.counter_label.config(text="Released")
            self.status_label.config(text="Status: Idle", fg="gray")

        # Also stop any ongoing countdown
        self.countdown_running = False

    def on_close(self):
        self.release_mouse()
        self.root.destroy()

if __name__ == "__main__":
    # Fail-safe: move mouse to corner to abort (pyautogui feature)
    pyautogui.FAILSAFE = True

    root = tk.Tk()
    app = MouseHoldApp(root)
    root.mainloop()import tkinter as tk
from tkinter import messagebox
import pyautogui
import threading
import time

class MouseHoldApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Mouse Hold Tool")
        self.root.geometry("320x220")
        self.root.resizable(False, False)

        self.is_holding = False
        self.countdown_running = False

        # Delay input
        tk.Label(root, text="Delay before hold (seconds):", font=("Arial", 11)).pack(pady=(15, 5))
        
        self.delay_var = tk.StringVar(value="3")
        self.delay_entry = tk.Entry(root, textvariable=self.delay_var, font=("Arial", 12), width=10, justify="center")
        self.delay_entry.pack()

        # Countdown display
        self.counter_label = tk.Label(root, text="Ready", font=("Arial", 16, "bold"), fg="blue")
        self.counter_label.pack(pady=10)

        # Start button
        self.start_btn = tk.Button(root, text="Start Hold", font=("Arial", 12, "bold"),
                                   bg="#4CAF50", fg="white", width=15, command=self.start_countdown)
        self.start_btn.pack(pady=5)

        # Release button
        self.release_btn = tk.Button(root, text="Release Mouse", font=("Arial", 12, "bold"),
                                     bg="#f44336", fg="white", width=15, command=self.release_mouse)
        self.release_btn.pack(pady=5)

        # Status
        self.status_label = tk.Label(root, text="Status: Idle", font=("Arial", 10), fg="gray")
        self.status_label.pack(pady=10)

        # Safety: release mouse if window is closed
        self.root.protocol("WM_DELETE_WINDOW", self.on_close)

    def start_countdown(self):
        if self.countdown_running or self.is_holding:
            return

        try:
            delay = float(self.delay_var.get())
            if delay < 0:
                raise ValueError
        except ValueError:
            messagebox.showerror("Error", "Please enter a valid positive number")
            return

        self.countdown_running = True
        self.start_btn.config(state="disabled")
        self.status_label.config(text="Status: Counting down...", fg="orange")

        # Run countdown in a separate thread so GUI stays responsive
        threading.Thread(target=self.countdown, args=(delay,), daemon=True).start()

    def countdown(self, delay):
        remaining = delay
        while remaining > 0 and self.countdown_running:
            self.counter_label.config(text=f"{remaining:.1f}")
            time.sleep(0.1)
            remaining -= 0.1

        if self.countdown_running:
            self.counter_label.config(text="HOLDING!")
            self.hold_mouse()

        self.countdown_running = False
        self.root.after(0, lambda: self.start_btn.config(state="normal"))

    def hold_mouse(self):
        if not self.is_holding:
            pyautogui.mouseDown(button='left')
            self.is_holding = True
            self.root.after(0, lambda: self.status_label.config(text="Status: Mouse LEFT is HELD", fg="red"))

    def release_mouse(self):
        if self.is_holding:
            pyautogui.mouseUp(button='left')
            self.is_holding = False
            self.counter_label.config(text="Released")
            self.status_label.config(text="Status: Idle", fg="gray")

        # Also stop any ongoing countdown
        self.countdown_running = False

    def on_close(self):
        self.release_mouse()
        self.root.destroy()

if __name__ == "__main__":
    # Fail-safe: move mouse to corner to abort (pyautogui feature)
    pyautogui.FAILSAFE = True

    root = tk.Tk()
    app = MouseHoldApp(root)
    root.mainloop()
