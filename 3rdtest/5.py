
import pyautogui
import time

print("Auto W started! Press CTRL+C to stop.")

try:
    while True:
        pyautogui.press('w')
        print("Pressed W")
        time.sleep(10)

except KeyboardInterrupt:
    print("\nStopped.")
