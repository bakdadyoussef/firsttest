# Windows Integration Testing Checklist

Run this checklist on Windows 10/11 before relying on the controller.

## Before enabling mouse mode

- [ ] Keep an alternate way to control the computer available.
- [ ] Launch the app with mouse mode disabled.
- [ ] Confirm the tray icon and settings window work.
- [ ] Confirm the hook status is reported as installed.
- [ ] Test the activation and emergency shortcuts configured in Background settings.

## Keypad and movement

- [ ] Test Num Lock on and off.
- [ ] Confirm top-row number keys still type normally.
- [ ] Confirm keypad digits are suppressed only when mouse mode is active.
- [ ] Test all eight directions.
- [ ] Test opposing direction keys and diagonal movement.
- [ ] Test KP 5 stop while a direction key is held.
- [ ] Test KP Enter precision mode.
- [ ] Adjust speed, acceleration delay, maximum speed, and update interval.

## Buttons and scrolling

- [ ] Test left, right, and middle click.
- [ ] Hold KP 0, move, then release; confirm drag ends.
- [ ] Test vertical scrolling in a browser and file manager.
- [ ] Assign horizontal scrolling and test in an application that supports it.
- [ ] Deactivate while holding a mouse button; confirm the button is released.

## Recovery and background operation

- [ ] Minimize to tray and confirm movement continues.
- [ ] Restore the GUI without restarting the controller.
- [ ] Use the emergency shortcut and confirm keypad input returns.
- [ ] Exit from the tray menu and confirm the app closes.
- [ ] Test invalid/corrupted configuration recovery using a backup copy, not the only live configuration.
- [ ] Test startup-at-login only after ordinary startup has been verified.
- [ ] Confirm ordinary use does not require administrator privileges.

## Limitations to record

- Some elevated applications and secure Windows screens may reject input from a non-elevated process.
- Keypad scan codes vary by hardware and layout.
- Windows timer scheduling is not real-time; record perceived latency on the actual PC.
