"""NumPad Mouse Control application entry point."""
from __future__ import annotations
import sys
from PySide6.QtWidgets import QApplication, QMessageBox
from app.application import NumPadMouseApplication


def main() -> int:
    app = QApplication(sys.argv)
    app.setApplicationName("NumPad Mouse Control")
    app.setOrganizationName("NumPadMouse")
    app.setQuitOnLastWindowClosed(False)
    try:
        controller = NumPadMouseApplication(app)
        controller.window.show()
        return app.exec()
    except Exception as exc:
        QMessageBox.critical(None, "NumPad Mouse Control - Startup Error", str(exc))
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
