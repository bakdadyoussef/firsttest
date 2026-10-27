Yes. You can build a Python program that turns your numeric keypad into a fully configurable mouse controller, and it can be much more flexible than Windows' built-in Mouse Keys (`Alt + Shift + Num Lock`).

Since you're currently using Windows and don't have a physical mouse, I'd approach this as a small accessibility tool rather than just a script that moves the cursor.

The important part is that we can control not only how fast the cursor moves, but also how quickly it accelerates, how scrolling works, which keys do what, and whether the keypad still produces numbers.

## 1. What I recommend building

Precise mouse movement

Configure slow, precise movement for small targets, faster movement for crossing the screen, and adjustable acceleration.

Complete mouse buttons

Left-click, right-click, hold-and-drag, double-click, and optionally middle-click, all from the numeric keypad.

Better scrolling

Scroll vertically and horizontally, adjust scroll increments, and configure how fast repeated scrolling occurs.

A real settings interface

Sliders and presets for movement speed, acceleration, repeat delay, repeat interval, scrolling, and key assignments.

Exclusive keypad control

While mouse mode is active, keypad presses control the pointer rather than typing digits. A toggle lets you return to normal numeric input.

## 2. The keypad layout I would use

Here is a practical default layout. It uses the keys you already have, so you won't need another input device.

NUM LOCK ON · MOUSE MODE ACTIVE

## 7

### ↖

Up-left

## 8

### ↑

Up

## 9

### ↗

Up-right

## 4

### ←

Left

## 5

### ●

Stop / click

## 6

### →

Right

## 1

### ↙

Down-left

## 2

### ↓

Down

## 3

### ↘

Down-right

### 0

Hold / drag

### .

Release / cancel drag

/ or a configurable key

Toggle left/right click mode

\+ / −

Scroll up / down

This is only the default. The final program should let you reassign keys, and it should account for different keypad layouts. For example, `5` could be a click key, or it could be reserved for stopping movement.

## 3. The settings that would actually solve your problems

The biggest improvement is to avoid treating cursor movement as a fixed-speed operation.

## Example control panel

Interactive mock settings — these sliders demonstrate the proposed controls; they do not move your actual cursor.

Base movement speed

### 8 px/tick

Precise

Medium

Fast

Acceleration multiplier

### 2.0×

None

Strong

Very strong

Movement update interval

### 12 ms

Fast

Balanced

Slow

Lower intervals can feel more responsive, but actual timing depends on Windows and the implementation.

Scroll amount per action

### 3 notches

Movement profile

Precision — small movements for selecting text and small buttons

Balanced — moderate movement with gradual acceleration

Fast — cross the screen quickly

Illustrative movement calculation

# 16 px/tick

Profile-adjusted movement before any additional acceleration from holding a key.

### What these settings mean in practice

- Base speed: How far the cursor moves for each update.
- Acceleration: Holding a direction gradually increases speed, so a quick tap remains precise but a long press travels farther.
- Update interval: How frequently the program processes held keys and moves the cursor.
- Repeat delay: How long you must hold a key before continuous movement begins.
- Scroll amount: How much the page moves per scroll action.
- Precision mode: A temporary slower mode for accurately selecting small targets.

One important distinction: Windows Mouse Keys uses its own acceleration and repeat behavior. A custom controller should implement its own movement loop rather than merely changing the built-in settings.

## 4. How to implement it properly in Python

I would build it for Windows using the following components:

| Component                 | Recommended approach                                          |
| ------------------------- | ------------------------------------------------------------- |
| Global keypad detection   | Windows low-level keyboard hook, implemented through `ctypes` |
| Suppress numeric input    | Consume keypad key events while mouse mode is active          |
| Cursor movement           | Windows `SendInput` API                                       |
| Mouse clicks and dragging | `SendInput` mouse button events                               |
| Scrolling                 | Mouse wheel and horizontal wheel events                       |
| Settings interface        | PySide6, with a simpler Tkinter alternative                   |
| Save preferences          | JSON configuration file                                       |
| Custom keymaps            | Editable key-to-action mapping                                |
| Emergency exit            | Dedicated hotkey that always remains available                |

This architecture avoids relying on the limited settings of Mouse Keys.

### Five technical details that matter

1\. The keypad must stop typing numbers. The keyboard hook must suppress the original keypad event before it reaches the active application. Moving the cursor alone is not enough.

2\. Num Lock needs deliberate handling. Keypad keys can produce different Windows key events depending on Num Lock. The program should support both states or provide a clearly defined mode.

3\. Movement must be event-driven. Track key-down and key-up events, and use a controlled update loop. Avoid a busy loop that consumes CPU while the cursor is moving.

4\. Clicks and dragging need reliable release behavior. If the program loses focus or exits while a mouse button is held, it should release that button to prevent a stuck click.

5\. The original Windows Mouse Keys feature should not compete with the new controller. Disable it while using the custom controller, and provide a reliable way to turn your program off.

A low-level keyboard hook can suppress keys, but Windows security boundaries still apply: an ordinary process cannot necessarily control elevated applications, and some secure screens cannot be controlled this way.

## 5. What I would build first

I'd make a complete Python application with:

- A PySide6 settings window.
- A configurable numeric-keypad layout.
- Separate speed settings for diagonal and straight movement.
- Adjustable acceleration, repeat delay and repeat interval.
- Vertical and horizontal scrolling.
- Left, right and middle clicks, double-click and drag.
- A temporary precision mode.
- A toggle between mouse mode and normal numeric input.
- A global emergency-stop shortcut.
- Persistent settings and customizable keymaps.
- A system-tray option, so the controller can run in the background.

The most important design choice is to intercept the keypad only when mouse mode is enabled. When you turn the mode off, the keypad should work normally again.

My recommendation is to use the Windows API directly for input handling and PySide6 for the interface. That gives us better control over responsiveness and key suppression than simply building a loop around `pyautogui`.

Bottom line: Yes, this is achievable in Python, and it can give you substantially more control than built-in Mouse Keys. The remaining limitation is that software cannot guarantee perfectly instantaneous input, but a carefully implemented controller should make the movement, acceleration and scrolling much more configurable.
