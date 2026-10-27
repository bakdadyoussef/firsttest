I've built the NumPad Mouse Control project and packaged the source code, GUI, Windows input backend, configuration system, tests, and full project plan into a downloadable archive.

## NumPad Mouse Control

Complete project source · ZIP archive · 45 KB

Archive verified

Download the complete project

The ZIP archive is compatible with Windows File Explorer and WinRAR.

## What's included

- PySide6 GUI: Dashboard, movement settings, mouse buttons, scrolling, keymap editor, profiles, background settings and diagnostics.
- Windows input backend: Low-level keyboard interception and mouse input using Windows APIs.
- Movement engine: Eight-direction movement, adjustable speed, acceleration, precision mode and diagonal normalization.
- Mouse controls: Clicking, dragging and scrolling.
- Background operation: System tray, activation shortcut and emergency stop.
- Configuration: Persistent JSON settings, profile management and validation.
- Development resources: Full project plan, README, installation scripts, Windows build script and testing checklist.

## How to run it on your Windows PC

1. Download and extract the ZIP archive.
2. Open the extracted `numpad_mouse` folder.
3. Double-click `run_windows.bat`.
4. Allow it to create a Python virtual environment and install the dependencies.
5. When the GUI opens, test the settings first, then enable mouse mode.

The default activation shortcut is F8. The emergency stop is Ctrl + Alt + Esc.

## Important verification note

I checked the Python syntax, ran the available configuration tests (3 passed), and verified the ZIP archive's integrity. However, this environment does not have PySide6 installed and cannot run the Windows-specific keyboard hook, so I have not verified the live GUI or actual mouse control on Windows. Please test the input behavior carefully before relying on it.

One format limitation: I couldn't create a genuine `.rar` archive because a RAR archiving utility isn't available here. I've provided the verified ZIP instead; if you specifically need RAR, you can open the extracted project in WinRAR and create a RAR archive from its folder.
