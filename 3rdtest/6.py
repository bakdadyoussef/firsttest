
from pynput import keyboard
import time

controller = keyboard.Controller()

active = False

def on_press(key):
    global active

    if key == keyboard.Key.f8:
        if not active:
            controller.press('w')
            controller.press(keyboard.Key.shift_l)
            active = True
            print("W + LEFT SHIFT: PRESSED")
        else:
            controller.release('w')
            controller.release(keyboard.Key.shift_l)
            active = False
            print("W + LEFT SHIFT: RELEASED")

    elif key == keyboard.Key.esc:
        if active:
            controller.release('w')
            controller.release(keyboard.Key.shift_l)
        print("Exiting...")
        return False


print("Press F8 to toggle W + Left Shift.")
print("Press ESC to exit.")

with keyboard.Listener(on_press=on_press) as listener:
    listener.join()
