"""Test UI instantiation and teardown."""
import sys
import os

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from app.ui.app_window import AppWindow

print("Testing AppWindow initialization...")
app = AppWindow(project_root=".")
app.update_idletasks()
print("AppWindow successfully created and laid out!")
app.destroy()
print("AppWindow successfully destroyed!")
