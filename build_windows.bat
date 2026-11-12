@echo off
setlocal
cd /d "%~dp0"
if not exist ".venv\Scripts\python.exe" (
  py -m venv .venv
  if errorlevel 1 goto :error
)
".venv\Scripts\python.exe" -m pip install --upgrade pip
if errorlevel 1 goto :error
".venv\Scripts\python.exe" -m pip install -r requirements.txt pyinstaller
if errorlevel 1 goto :error
".venv\Scripts\pyinstaller.exe" --noconfirm --clean --windowed --name NumPadMouse main.py
if errorlevel 1 goto :error
echo.
echo Build finished. Check the dist folder.
pause
goto :eof
:error
echo.
echo Build failed. Check the error above.
pause
exit /b 1
