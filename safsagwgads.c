# Project plan: NumPad Mouse Control

We'll design this as a complete Windows desktop application in Python with a PySide6 GUI, built specifically for someone who needs to operate their PC without a physical mouse.

The goal is not merely to imitate Windows Mouse Keys. It's to create a configurable input-control application that intercepts the numeric keypad, converts its keys into mouse actions, provides fine control over responsiveness, and keeps running in the background when its settings window is closed.

The design should prioritize reliability, precise control, low CPU usage, and recovery if something goes wrong.

## 1. Product definition

[SOFTWARE](https://images.openai.com/static-rsc-4/b1BsWNAk7RlI0zq9I6xIY7ODe6COT86mITF5YnN8emBDDGBhIBR_7WMwnNSzSjKpbMTZrK14I3Svfi0v-GrHyODxDiWdWf_jXpExa_waKhqZ0m2iJ4JX7LZmPrFCw4oKN9djweiVu7Diej5Dm8lW5-5JEopt3UjeOkRvDt0tvxs?purpose=inline)

PySide6 desktop GUI

A polished control panel with adjustable settings, a live keypad map, movement profiles, and a clear indication of whether mouse mode is active.

[The Easiest Way to Run Llama 3 Locally | by Marko Vidrih | Medium](https://images.openai.com/static-rsc-4/lpCtpq2WmtTqfFomRCtHmQJqdcz7wdUuoBtwN51Tf4p-7KRSZe2tYfzAvNywi5KHNRWdxV5AUZBlf1ecnKfDXSOvXs_OASeoIoPhyyDmAyM9AMrxUOHeb1FRfbVgsmbnOBLL74VErLXlHB5ZRw5o3p4amb7mD58RJk3RA4qXpbA?purpose=inline)

Background operation

The application can minimize to the system tray, continue intercepting keys, and restore its window when needed.

[Closeup Keyboard 05](https://images.openai.com/static-rsc-4/3LgptBRQ3fBpYqMcgc1Q-vu7W_14FphvPP3ITeh5V7Wg7utkUQwtfup6cmZejtweLO9eYqpzl9AkzDA72WZF98SunB9pQQ9ifk0BX-ynj1dAot5aVkIRls0aHh2hXfyaGhN9wDVlWsBQqOEymsojjrLQeXQlk_D61w5h1ZE3vo3t8T5HmYpd5q-uEwOo8S1W?purpose=inline)

Exclusive numeric-keypad control

In mouse mode, the keypad controls the cursor and mouse buttons rather than entering numbers. When mouse mode is disabled, normal numeric input returns.

### Main design principles

1. Never lose control of the PC. A dependable emergency-stop shortcut and an automatic mouse-button release mechanism are essential.
2. Separate the GUI from input handling. Closing or freezing the settings window must not accidentally leave mouse buttons held or input handling in an unknown state.
3. Use precise movement logic. Implement adjustable speed, acceleration, repeat timing and scrolling instead of relying on the default Windows Mouse Keys behavior.
4. Make settings persistent. Save preferences, keymaps, profiles and startup choices in a configuration file.
5. Run efficiently. The program should idle with minimal CPU usage and process movement only when needed.
6. Support recovery. If the hook fails, the application exits, or the user disables mouse mode, release any held buttons and restore normal keypad input.

## 2. Complete feature specification

I would divide the application into eight functional modules so the project remains maintainable as we add features.

A. Mouse engine

Cursor movement, acceleration, clicking, dragging, button-state tracking and scrolling.

B. Keyboard engine

Global key detection, keypad interception, key-state tracking, keymap translation and normal-mode restoration.

C. Movement and timing controller

Movement profiles, precision mode, repeat delay, acceleration curves and diagonal normalization.

D. Settings and keymap manager

Editable assignments, profile presets, configuration validation, import and export.

E. PySide6 interface

Dashboard, settings pages, interactive keypad preview, profile editor and status indicators.

F. Background and tray service

Minimize to tray, restore window, hotkeys, startup behavior and controlled shutdown.

G. Safety and recovery

Emergency stop, watchdog checks, stuck-button prevention, hook failure handling and safe mode.

H. Configuration and diagnostics

JSON settings, structured logs, diagnostics, performance measurements and configuration migration.

### Feature priorities

| Priority  | Features                                                                                       |
| --------- | ---------------------------------------------------------------------------------------------- |
| Essential | Key interception, normal-input restoration, movement, clicking, scrolling, emergency stop      |
| High      | Acceleration, configurable timing, dragging, tray operation, saved settings                    |
| Advanced  | Custom keymaps, multiple profiles, precision mode, horizontal scrolling                        |
| Later     | Per-application profiles, startup with Windows, portable packaging, accessibility enhancements |

The application should be useful even before the advanced features are finished.

## 3. The exact keypad behavior

The keymap should be configurable, but the initial default should be easy to learn.

## Default numeric-keypad map

Proposed

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

### ■

Stop

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

## 0

Left-button hold

## .

Left click / release

## /

Right click

## \*

Middle click

## +

Scroll up

## −

Scroll down

The slash, asterisk, plus and minus keys have their own key identities. The program must distinguish them from the other keypad keys and support remapping.

I would change one detail from the initial concept: `5` should stop movement, not perform a click by default. That provides an immediate way to halt the cursor without accidentally clicking whatever is underneath it. Clicking can be assigned to `.` or another convenient key.

The final map should also support:

- Hold-to-move and release-to-stop.
- Simultaneous direction keys for diagonal movement.
- Holding the left mouse button while moving to drag.
- A configurable click-versus-hold behavior.
- Optional double-click action.
- Separate scrolling and movement modes.
- An option to swap left and right mouse buttons.
- A way to disable unused keypad actions.

### Keymap editor

The GUI should include a dedicated page where the user can select a key and assign an action from a dropdown. It should detect duplicate or conflicting assignments, allow a key to be unassigned, and provide a Restore default keymap button.

A keymap should describe actions, not contain arbitrary Python code. This makes configuration safer and easier to validate.

## 4. Movement engine: the core of the project

This is the most important technical part. A poor movement engine will still feel slow or unpredictable, regardless of how attractive the GUI looks.

### Movement model

Use a time-based controller with four stages:

1. Detect key-down and key-up events.
2. Track which directions are currently held.
3. Calculate the movement distance using elapsed time and the active speed profile.
4. Send the resulting mouse movement through the Windows input API.

The calculation should be based on elapsed time rather than assuming every timer callback arrives exactly on schedule. That helps keep movement more consistent if Windows briefly delays the program.

A useful conceptual model is:

\\[ v(t)=\min(v\_{\max},v_0+a t) \\]

where:

- \\(v_0\\) is the initial movement speed.
- \\(a\\) is the acceleration rate.
- \\(v\_{\max}\\) is the speed limit.
- \\(t\\) is the time the key has been held.

For a smoother experience, the implementation should support configurable acceleration curves, including a gradual ramp rather than an abrupt speed jump.

### Settings to expose

| Setting                    | Initial design range     |
| -------------------------- | ------------------------ |
| Base speed                 | 1–30 pixels per update   |
| Maximum speed              | 10–100 pixels per update |
| Acceleration               | 0–5×                     |
| Repeat delay               | 0–800 ms                 |
| Movement update interval   | 4–40 ms                  |
| Precision speed multiplier | 0.1–1.0×                 |
| Diagonal normalization     | On by default            |

These are proposed starting ranges, not guaranteed performance limits. We should test the actual feel on your PC and adjust them after the first implementation.

Diagonal normalization matters: moving diagonally should not be substantially faster than moving horizontally or vertically. We should normalize the combined direction vector so the cursor has a consistent overall speed.

### Presets

Precision

Slow movement, gentle acceleration and small increments. Useful for small buttons, selecting text and accurate positioning.

Balanced

Moderate initial speed with gradual acceleration. Recommended as the default.

Fast

High movement speed and faster acceleration for crossing large screens.

The user must be able to modify every preset and save custom profiles without editing the source code.

## 5. Scrolling and mouse-button behavior

Scrolling should have its own controller, rather than being treated as a special form of cursor movement.

### Scrolling features

- Vertical scrolling up and down.
- Adjustable wheel amount per action.
- Continuous scrolling while a key is held.
- Adjustable scroll repeat interval and acceleration.
- Horizontal scrolling, where supported by the target application.
- A slower scrolling mode for precise navigation.
- Independent movement and scrolling speed settings.

The engine should accumulate fractional movement and wheel deltas where necessary, rather than discarding small values. That avoids inconsistent behavior at very low settings.

### Clicking and dragging

The mouse engine must maintain explicit state for every mouse button.

For example, dragging should work as follows:

1. Press the configured drag key to hold the left mouse button.
2. Move the cursor using the direction keys.
3. Release the drag key to release the left mouse button.

We should also support a configurable drag-lock mode, but it should be off by default because it can cause accidental dragging.

If the user switches modes, disables the controller, or exits the program, all mouse buttons held by the program must be released. This behavior needs to be tested rather than assumed.

## 6. PySide6 GUI design

The interface should be designed as a small desktop application, not a single settings dialog with dozens of unrelated controls.

I recommend a dark, compact interface with a left navigation rail, a main content area and a persistent status indicator. It should also work at modest screen resolutions.

## NumPad Mouse

CONTROL CENTER

Illustrative UI

CONTROLLER

## Inactive

Keypad available normally

PROFILE

## Balanced

Customizable

### Quick controls

Movement

Buttons

Scrolling

Movement speed

8 px/update

Acceleration

2×

This is a layout concept, not a running controller. The finished application would have interactive controls and live status.

### Page 1 — Dashboard

The dashboard should show:

- Current controller state: Active or Inactive.
- Whether the keyboard hook is working.
- Current movement profile.
- Current Num Lock state.
- Quick activation and deactivation controls.
- A clickable keypad preview showing each assigned action.
- Current cursor speed and scroll settings.
- A button to restore the default configuration.

### Page 2 — Movement

This page controls:

- Base and maximum speed.
- Acceleration and acceleration curve.
- Key repeat delay.
- Update interval.
- Diagonal movement behavior.
- Precision mode and its activation key.
- Optional movement speed boost.
- Separate speed profiles for normal and precision operation.

Every adjustment should take effect immediately when safe to do so, without requiring a restart.

### Page 3 — Mouse buttons

Controls for:

- Left, right and middle clicks.
- Double-click behavior and timing.
- Drag key and drag-lock behavior.
- Button swapping.
- Click-and-hold behavior.
- Recovery of stuck buttons.

### Page 4 — Scrolling

Controls for:

- Vertical and horizontal scrolling.
- Scroll increment.
- Scroll repeat delay and interval.
- Scroll acceleration.
- Precision scrolling.

### Page 5 — Keymap editor

A table with these columns:

| Column          | Purpose                                            |
| --------------- | -------------------------------------------------- |
| Physical key    | The actual keypad key                              |
| Assigned action | What the key does                                  |
| Activation type | Tap, hold or toggle                                |
| Conflict status | Whether the assignment clashes with another action |

The editor should offer an interactive Press a key to assign mode, with cancel and reset buttons.

### Page 6 — Profiles

Users can create, rename, duplicate, edit and delete custom profiles. The application should ship with Precision, Balanced and Fast profiles.

### Page 7 — Background and startup

Settings for:

- Minimize to system tray.
- Close button minimizes instead of exiting.
- Start the controller automatically when Windows starts.
- Start with mouse mode disabled or enabled.
- Show or hide notifications.
- Global activation and emergency-stop shortcuts.

### Page 8 — Diagnostics

Display:

- Keyboard-hook status.
- Mouse-button state.
- Current keymap.
- Recent errors.
- Last successful configuration save.
- Approximate CPU usage and movement update statistics.

A Safe Mode button should disable keypad interception and release any held mouse buttons.

## 7. Background operation and system tray

This is a core feature, not an optional afterthought.

The application should remain running after the main settings window disappears. We can use PySide6's `QSystemTrayIcon` and a tray context menu.

## Proposed tray menu

Enable mouse mode

Intercept the keypad and activate the controller.

Disable mouse mode

Release held buttons and restore ordinary keypad input.

Open settings

Restore the GUI without restarting the background controller.

Emergency stop

Immediately stop movement, release all held buttons and disable interception.

Exit application

Release all inputs, remove the hook and close cleanly.

### What happens when the user closes the window?

The close event should be handled according to the settings:

- If Minimize to tray is enabled, hide the window and keep the controller running.
- If it is disabled, ask whether the user wants to minimize or exit.
- If the user chooses Exit, disable interception, release held mouse buttons, stop the background threads and save settings.

We should distinguish three states clearly:

1. Application running: The program exists in the background.
2. Mouse mode active: The keypad is intercepted.
3. Movement in progress: One or more direction keys are currently held.

That distinction prevents the confusing situation where the program is running but the keypad is unexpectedly disabled.

### Background startup

On Windows, the app can optionally launch at login using the user's Startup folder or a supported registry-based startup entry.

However, it should start with mouse mode disabled by default. The user should not unexpectedly lose numeric-keypad input after restarting Windows. Once the user has confirmed the setup works reliably, automatic activation can be an opt-in setting.

The app should also offer a normal windowed launch option for troubleshooting.

## 8. Technical architecture

The safest design separates the Windows input hook, movement engine and graphical interface. These components communicate through explicit state and thread-safe signals.

\#chatgpt-mermaid-\_r_r7\_{font-family:-apple-system-body,ui-sans-serif,-apple-system,system-ui,"Segoe UI",Helvetica,"Apple Color Emoji",Arial,sans-serif,"Segoe UI Emoji","Segoe UI Symbol";font-size:16px;fill:rgb(237, 237, 237);}@keyframes edge-animation-frame{from{stroke-dashoffset:0;}}@keyframes dash{to{stroke-dashoffset:0;}}#chatgpt-mermaid-\_r_r7\_ .edge-animation-slow{stroke-dasharray:9,5!important;stroke-dashoffset:900;animation:dash 50s linear infinite;stroke-linecap:round;}#chatgpt-mermaid-\_r_r7\_ .edge-animation-fast{stroke-dasharray:9,5!important;stroke-dashoffset:900;animation:dash 20s linear infinite;stroke-linecap:round;}#chatgpt-mermaid-\_r_r7\_ .error-icon{fill:rgb(48, 48, 48);}#chatgpt-mermaid-\_r_r7\_ .error-text{fill:rgb(237, 237, 237);stroke:rgb(237, 237, 237);}#chatgpt-mermaid-\_r_r7\_ .edge-thickness-normal{stroke-width:1px;}#chatgpt-mermaid-\_r_r7\_ .edge-thickness-thick{stroke-width:3.5px;}#chatgpt-mermaid-\_r_r7\_ .edge-pattern-solid{stroke-dasharray:0;}#chatgpt-mermaid-\_r_r7\_ .edge-thickness-invisible{stroke-width:0;fill:none;}#chatgpt-mermaid-\_r_r7\_ .edge-pattern-dashed{stroke-dasharray:3;}#chatgpt-mermaid-\_r_r7\_ .edge-pattern-dotted{stroke-dasharray:2;}#chatgpt-mermaid-\_r_r7\_ .marker{fill:rgb(175, 175, 175);stroke:rgb(175, 175, 175);}#chatgpt-mermaid-\_r_r7\_ .marker.cross{stroke:rgb(175, 175, 175);}#chatgpt-mermaid-\_r_r7\_ svg{font-family:-apple-system-body,ui-sans-serif,-apple-system,system-ui,"Segoe UI",Helvetica,"Apple Color Emoji",Arial,sans-serif,"Segoe UI Emoji","Segoe UI Symbol";font-size:16px;}#chatgpt-mermaid-\_r_r7\_ p{margin:0;}#chatgpt-mermaid-\_r_r7\_ .label{font-family:-apple-system-body,ui-sans-serif,-apple-system,system-ui,"Segoe UI",Helvetica,"Apple Color Emoji",Arial,sans-serif,"Segoe UI Emoji","Segoe UI Symbol";color:rgb(237, 237, 237);}#chatgpt-mermaid-\_r_r7\_ .cluster-label text{fill:rgb(237, 237, 237);}#chatgpt-mermaid-\_r_r7\_ .cluster-label span{color:rgb(237, 237, 237);}#chatgpt-mermaid-\_r_r7\_ .cluster-label span p{background-color:transparent;}#chatgpt-mermaid-\_r_r7\_ .label text,#chatgpt-mermaid-\_r_r7\_ span{fill:rgb(237, 237, 237);color:rgb(237, 237, 237);}#chatgpt-mermaid-\_r_r7\_ .node rect,#chatgpt-mermaid-\_r_r7\_ .node circle,#chatgpt-mermaid-\_r_r7\_ .node ellipse,#chatgpt-mermaid-\_r_r7\_ .node polygon,#chatgpt-mermaid-\_r_r7\_ .node path{fill:rgb(9, 23, 44);stroke:rgb(31, 78, 148);stroke-width:1px;}#chatgpt-mermaid-\_r_r7\_ .rough-node .label text,#chatgpt-mermaid-\_r_r7\_ .node .label text,#chatgpt-mermaid-\_r_r7\_ .image-shape .label,#chatgpt-mermaid-\_r_r7\_ .icon-shape .label{text-anchor:middle;}#chatgpt-mermaid-\_r_r7\_ .node .katex path{fill:#000;stroke:#000;stroke-width:1px;}#chatgpt-mermaid-\_r_r7\_ .rough-node .label,#chatgpt-mermaid-\_r_r7\_ .node .label,#chatgpt-mermaid-\_r_r7\_ .image-shape .label,#chatgpt-mermaid-\_r_r7\_ .icon-shape .label{text-align:center;}#chatgpt-mermaid-\_r_r7\_ .node.clickable{cursor:pointer;}#chatgpt-mermaid-\_r_r7\_ .root .anchor path{fill:rgb(175, 175, 175)!important;stroke-width:0;stroke:rgb(175, 175, 175);}#chatgpt-mermaid-\_r_r7\_ .arrowheadPath{fill:rgb(175, 175, 175);}#chatgpt-mermaid-\_r_r7\_ .edgePath .path{stroke:rgb(175, 175, 175);stroke-width:1px;}#chatgpt-mermaid-\_r_r7\_ .flowchart-link{stroke:rgb(175, 175, 175);fill:none;}#chatgpt-mermaid-\_r_r7\_ .edgeLabel{background-color:rgb(0, 0, 0);text-align:center;}#chatgpt-mermaid-\_r_r7\_ .edgeLabel p{background-color:rgb(0, 0, 0);}#chatgpt-mermaid-\_r_r7\_ .edgeLabel rect{opacity:0.5;background-color:rgb(0, 0, 0);fill:rgb(0, 0, 0);}#chatgpt-mermaid-\_r_r7\_ .labelBkg{background-color:rgba(0, 0, 0, 0.5);}#chatgpt-mermaid-\_r_r7\_ .cluster rect{fill:rgb(48, 48, 48);stroke:rgba(255, 255, 255, 0.15);stroke-width:1px;}#chatgpt-mermaid-\_r_r7\_ .cluster text{fill:rgb(237, 237, 237);}#chatgpt-mermaid-\_r_r7\_ .cluster span{color:rgb(237, 237, 237);}#chatgpt-mermaid-\_r_r7\_ div.mermaidTooltip{position:absolute;text-align:center;max-width:200px;padding:2px;font-family:-apple-system-body,ui-sans-serif,-apple-system,system-ui,"Segoe UI",Helvetica,"Apple Color Emoji",Arial,sans-serif,"Segoe UI Emoji","Segoe UI Symbol";font-size:12px;background:rgb(48, 48, 48);border:1px solid rgba(255, 255, 255, 0.15);border-radius:2px;pointer-events:none;z-index:100;}#chatgpt-mermaid-\_r_r7\_ .flowchartTitleText{text-anchor:middle;font-size:18px;fill:rgb(237, 237, 237);}#chatgpt-mermaid-\_r_r7\_ rect.text{fill:none;stroke-width:0;}#chatgpt-mermaid-\_r_r7\_ .icon-shape,#chatgpt-mermaid-\_r_r7\_ .image-shape{background-color:rgb(0, 0, 0);text-align:center;}#chatgpt-mermaid-\_r_r7\_ .icon-shape p,#chatgpt-mermaid-\_r_r7\_ .image-shape p{background-color:rgb(0, 0, 0);padding:2px;}#chatgpt-mermaid-\_r_r7\_ .icon-shape .label rect,#chatgpt-mermaid-\_r_r7\_ .image-shape .label rect{opacity:0.5;background-color:rgb(0, 0, 0);fill:rgb(0, 0, 0);}#chatgpt-mermaid-\_r_r7\_ .label-icon{display:inline-block;height:1em;overflow:visible;vertical-align:-0.125em;}#chatgpt-mermaid-\_r_r7\_ .node .label-icon path{fill:currentColor;stroke:revert;stroke-width:revert;}#chatgpt-mermaid-\_r_r7\_ .node .neo-node{stroke:rgb(31, 78, 148);}#chatgpt-mermaid-\_r_r7\_ [data-look="neo"].node rect,#chatgpt-mermaid-\_r_r7\_ [data-look="neo"].cluster rect,#chatgpt-mermaid-\_r_r7\_ [data-look="neo"].node polygon{stroke:url(#chatgpt-mermaid-\_r_r7\_-gradient);filter:drop-shadow( 1px 2px 2px rgba(185,185,185,1));}#chatgpt-mermaid-\_r_r7\_ [data-look="neo"].swimlane.cluster rect{filter:none;}#chatgpt-mermaid-\_r_r7\_ [data-look="neo"].node path{stroke:url(#chatgpt-mermaid-\_r_r7\_-gradient);stroke-width:1px;}#chatgpt-mermaid-\_r_r7\_ [data-look="neo"].node .outer-path{filter:drop-shadow( 1px 2px 2px rgba(185,185,185,1));}#chatgpt-mermaid-\_r_r7\_ [data-look="neo"].node .neo-line path{stroke:rgb(31, 78, 148);filter:none;}#chatgpt-mermaid-\_r_r7\_ [data-look="neo"].node circle{stroke:url(#chatgpt-mermaid-\_r_r7\_-gradient);filter:drop-shadow( 1px 2px 2px rgba(185,185,185,1));}#chatgpt-mermaid-\_r_r7\_ [data-look="neo"].node circle .state-start{fill:#000000;}#chatgpt-mermaid-\_r_r7\_ [data-look="neo"].icon-shape .icon{fill:url(#chatgpt-mermaid-\_r_r7\_-gradient);filter:drop-shadow( 1px 2px 2px rgba(185,185,185,1));}#chatgpt-mermaid-\_r_r7\_ [data-look="neo"].icon-shape .icon-neo path{stroke:url(#chatgpt-mermaid-\_r_r7\_-gradient);filter:drop-shadow( 1px 2px 2px rgba(185,185,185,1));}#chatgpt-mermaid-\_r_r7\_ .node text{font-size:14px;font-weight:600;letter-spacing:normal;fill:rgb(153, 206, 255);}#chatgpt-mermaid-\_r_r7\_ .edgeLabels text{font-size:13px;font-weight:600;letter-spacing:-0.08px;fill:rgb(153, 206, 255);}#chatgpt-mermaid-\_r_r7\_ .node tspan[font-weight="normal"],#chatgpt-mermaid-\_r_r7\_ .edgeLabels tspan[font-weight="normal"]{font-weight:600;}#chatgpt-mermaid-\_r_r7\_ .edgeLabel .label rect{opacity:1;rx:13px;ry:13px;fill:rgb(0, 14, 26);stroke:rgb(26, 62, 95);stroke-width:1px;}#chatgpt-mermaid-\_r_r7\_ .node rect,#chatgpt-mermaid-\_r_r7\_ .node circle,#chatgpt-mermaid-\_r_r7\_ .node ellipse,#chatgpt-mermaid-\_r_r7\_ .node polygon,#chatgpt-mermaid-\_r_r7\_ .node path{fill:rgb(0, 40, 77);stroke:rgba(255, 255, 255, 0.1);stroke-width:1px;}#chatgpt-mermaid-\_r_r7\_ .node rect{rx:16px;ry:16px;}#chatgpt-mermaid-\_r_r7\_ .node.mermaid-decision .label-container{fill:rgb(0, 14, 26);stroke:rgb(26, 62, 95);stroke-dasharray:2,2;}#chatgpt-mermaid-\_r_r7\_ .edgePaths .flowchart-link{stroke:rgb(175, 175, 175);stroke-width:1px;stroke-linecap:round;stroke-linejoin:round;}#chatgpt-mermaid-\_r_r7\_ .marker{fill:rgb(175, 175, 175);stroke:rgb(175, 175, 175);}#chatgpt-mermaid-\_r_r7\_ :root{--mermaid-font-family:-apple-system-body,ui-sans-serif,-apple-system,system-ui,"Segoe UI",Helvetica,"Apple Color Emoji",Arial,sans-serif,"Segoe UI Emoji","Segoe UI Symbol";}Physical numeric keypadWindows keyboard hookMouse mode active?Pass key to WindowsResolve keymap actionInput state managerMovement and timing engineWindows SendInput APICursor, clicks and scrollingPySide6 GUIConfiguration and profilemanagerBackground and tray controllerSafety controllerNoYes

### Recommended implementation stack

| Component             | Technology                                     | Reason                                               |
| --------------------- | ---------------------------------------------- | ---------------------------------------------------- |
| GUI                   | PySide6                                        | Flexible, modern desktop interface                   |
| Keyboard interception | Windows `WH_KEYBOARD_LL` hook through `ctypes` | Detect and suppress keypad events globally           |
| Mouse input           | Windows `SendInput`                            | Direct mouse movement, button and wheel events       |
| Background service    | `QSystemTrayIcon` plus a dedicated hook thread | Keep input control active while the window is hidden |
| Timing                | Monotonic clock and controlled update loop     | Avoid basing speed on assumed timer precision        |
| Configuration         | Python `json` module                           | Human-readable settings and keymaps                  |
| Logging               | Python `logging`                               | Diagnose hook failures and unexpected behavior       |
| Packaging             | PyInstaller                                    | Distribute a Windows executable                      |

The Windows-specific code should be isolated behind an input-backend interface. That will make it possible to replace or improve the input implementation later without rewriting the GUI.

### Threading model

The low-level keyboard hook requires a message loop and must respond quickly to incoming events. It should not perform expensive calculations, file operations or GUI updates inside its callback.

The recommended arrangement is:

- GUI thread: Draws the interface, manages settings and receives status updates.
- Keyboard-hook thread: Installs the hook, processes keyboard events and runs the required Windows message loop.
- Movement controller: Processes the current key state and generates mouse actions.
- Safety controller: Handles emergency-stop requests, hook health and shutdown cleanup.

Qt signals should be used to send updates to the GUI. Shared state should be protected or designed for safe communication between threads.

We should avoid creating a new thread for every key press or movement event. Long-lived components are more efficient and easier to shut down correctly.

### Why not use only PyAutoGUI or the `keyboard` package?

Libraries such as PyAutoGUI can help with mouse automation, and keyboard-hook libraries can simplify key detection. However, the requirement to suppress the original keypad input reliably, track held keys, manage low-level Windows events and cleanly restore normal input makes a direct Windows API backend a better primary design.

Higher-level libraries may still be useful for prototyping or optional functionality, but the core engine should not depend on a collection of loosely coordinated automation loops.

## 9. Project folder structure

The codebase should be divided into logical modules from the beginning.

```

numpad_mouse/
├── main.py
├── requirements.txt
├── README.md
├── config/
│   ├── defaults.json
│   └── keymaps/
│       └── default.json
├── app/
│   ├── application.py
│   ├── state.py
│   └── signals.py
├── core/
│   ├── keyboard_hook.py
│   ├── key_translator.py
│   ├── input_state.py
│   ├── mouse_engine.py
│   ├── movement_controller.py
│   ├── scroll_controller.py
│   └── safety_controller.py
├── platform/
│   └── windows/
│       ├── keyboard_backend.py
│       ├── mouse_backend.py
│       └── winapi.py
├── gui/
│   ├── main_window.py
│   ├── tray_manager.py
│   ├── theme.py
│   ├── widgets/
│   │   ├── keypad_preview.py
│   │   ├── status_card.py
│   │   └── keymap_editor.py
│   └── pages/
│       ├── dashboard.py
│       ├── movement.py
│       ├── buttons.py
│       ├── scrolling.py
│       ├── keymap.py
│       ├── profiles.py
│       ├── background.py
│       └── diagnostics.py
├── services/
│   ├── config_manager.py
│   ├── profile_manager.py
│   └── diagnostics.py
├── tests/
│   ├── test_keymaps.py
│   ├── test_movement.py
│   ├── test_config.py
│   ├── test_state.py
│   └── test_safety.py
└── assets/
    ├── icons/
    └── app.ico

```

The folder named `platform` here is a project package for Windows-specific code; it should be structured carefully to avoid conflicts with Python's standard-library modules.

### Responsibilities of the most important files

- `keyboard_hook.py`: Owns the hook lifecycle and converts raw keyboard events into normalized events.
- `key_translator.py`: Maps physical keys to configured actions.
- `input_state.py`: Tracks pressed keys and mouse buttons, including release events.
- `movement_controller.py`: Calculates cursor displacement, acceleration and diagonal normalization.
- `scroll_controller.py`: Produces vertical and horizontal wheel input.
- `mouse_engine.py`: Sends button, cursor and scrolling actions through the Windows backend.
- `safety_controller.py`: Provides emergency stop, cleanup and recovery.
- `config_manager.py`: Loads, validates and saves settings safely.
- `tray_manager.py`: Handles minimize, restore, activation, deactivation and exit.
- `keymap_editor.py`: Provides the interactive mapping interface.

## 10. Configuration and data model

The settings file should be versioned so future releases can add new settings without breaking existing configurations.

An example configuration schema:

```

{
  "schema_version": 1,
  "startup": {
    "launch_at_login": false,
    "mouse_mode_on_start": false,
    "minimize_to_tray": true
  },
  "controller": {
    "enabled": false,
    "num_lock_independent": true,
    "active_profile": "Balanced",
    "emergency_hotkey": "CTRL+ALT+ESC"
  },
  "movement": {
    "base_speed": 8,
    "maximum_speed": 60,
    "acceleration": 2.0,
    "repeat_delay_ms": 150,
    "update_interval_ms": 12,
    "precision_multiplier": 0.35,
    "diagonal_normalization": true
  },
  "scrolling": {
    "vertical_step": 3,
    "horizontal_enabled": true,
    "repeat_delay_ms": 200,
    "repeat_interval_ms": 50
  },
  "keymap": {
    "KP_8": "move_up",
    "KP_2": "move_down",
    "KP_4": "move_left",
    "KP_6": "move_right",
    "KP_5": "stop",
    "KP_7": "move_up_left",
    "KP_9": "move_up_right",
    "KP_1": "move_down_left",
    "KP_3": "move_down_right",
    "KP_0": "left_button_hold",
    "KP_DECIMAL": "left_click",
    "KP_DIVIDE": "right_click",
    "KP_MULTIPLY": "middle_click",
    "KP_ADD": "scroll_up",
    "KP_SUBTRACT": "scroll_down"
  }
}

```

This is a proposed schema, not an implemented or fully validated configuration. The finished program should validate every field and action before activating the hook.

### Saving settings safely

The configuration manager should:

1. Load defaults when no configuration exists.
2. Validate types, value ranges and action names.
3. Reject invalid or conflicting mappings with a useful message.
4. Save changes to a temporary file and then replace the old configuration atomically where possible.
5. Keep a backup of the last valid configuration.
6. Migrate older configuration versions when the schema changes.

A corrupted configuration should never prevent the user from opening the application and disabling mouse mode.

## 11. Safety and reliability requirements

This is where we need to be especially careful, because the application will intercept global input.

Emergency stop independent of the keypad

Use a configurable global shortcut that does not depend on numeric-keypad input. The default shortcut must be tested for conflicts with Windows and common applications.

Release every held mouse button

When the controller deactivates, the hook fails or the application exits, release any buttons held by the program. Cleanup must be idempotent so it can safely run more than once.

Restore normal keyboard input

Disabling the controller must stop suppressing keypad events. A hook failure should trigger a visible warning and attempt safe cleanup.

Detect stuck key states

Track key-down and key-up events, clear stale states when focus or controller state changes, and ensure a lost release event cannot keep movement running indefinitely.

Fail-safe startup

Start in normal keyboard mode, validate settings, then activate interception only when explicitly requested or when the user has enabled a tested automatic-start option.

### Important Windows limitations

- A low-level keyboard hook can fail or be removed by Windows if its callback does not return promptly. The program must detect loss of functionality as far as practical and recover safely.
- An ordinary application may not be able to control elevated applications or secure Windows screens. Running as administrator should not be the default workaround.
- Windows may report keypad keys differently depending on Num Lock, scan codes, keyboard layout and hardware. Both Num Lock states must be tested.
- The emergency shortcut must be registered and monitored independently of the keypad. No user-space application can guarantee control over every secure screen or catastrophic process failure.

These are reasons to test the implementation carefully, not reasons to abandon the project.

## 12. Development roadmap

I recommend building this in seven phases. Each phase should produce a working, testable result instead of attempting to implement every feature simultaneously.

## 1

Foundation and safety

First milestone

- Create the Python project and PySide6 window.
- Implement the Windows keyboard hook and input backend.
- Detect keypad presses and releases.
- Build activation/deactivation and emergency-stop behavior.
- Confirm normal keypad input is restored correctly.

## 2

Core mouse control

- Add eight-direction movement and the stop key.
- Implement left, right and middle clicks.
- Add drag-and-drop behavior and button-release cleanup.
- Implement basic vertical and horizontal scrolling.

## 3

Movement tuning

- Add acceleration and speed limits.
- Implement repeat delay and adjustable update intervals.
- Add precision mode and diagonal normalization.
- Tune responsiveness on the target PC.

## 4

Complete settings interface

- Build the dashboard and all settings pages.
- Add the interactive keymap editor.
- Add profiles, validation and persistent configuration.

## 5

Background operation

- Implement the system tray and minimize-to-tray behavior.
- Add global activation and emergency shortcuts.
- Add optional startup with Windows.
- Test closing and reopening the settings window without interrupting mouse control.

## 6

Testing and hardening

- Test Num Lock on and off.
- Test rapid presses, simultaneous keys and missed key releases.
- Test hook failure, shutdown and recovery.
- Measure idle CPU usage and input responsiveness.

## 7

Release

- Package a Windows executable with PyInstaller.
- Include application icons and a user guide.
- Test first launch, configuration storage and clean shutdown.
- Verify behavior on a fresh Windows account.

## 13. Testing plan and acceptance criteria

The project is not complete just because the cursor moves. It is complete when the entire workflow is reliable.

| Test                              | Expected result                                                   |
| --------------------------------- | ----------------------------------------------------------------- |
| Press a direction key             | Cursor moves in the correct direction                             |
| Hold a direction key              | Smooth continuous movement                                        |
| Hold two directions               | Normalized diagonal movement                                      |
| Release all direction keys        | Movement stops promptly                                           |
| Change speed in the GUI           | New setting takes effect without restarting                       |
| Press keypad digits in mouse mode | They do not type numbers                                          |
| Disable mouse mode                | Keypad returns to normal numeric input                            |
| Toggle Num Lock                   | Controller behaves according to its configured policy             |
| Drag and release                  | Mouse button is released correctly                                |
| Scroll while moving               | Both actions work without interfering                             |
| Minimize to tray                  | Controller continues working                                      |
| Restore the GUI                   | Existing controller state and settings are preserved              |
| Emergency stop                    | Movement stops, buttons are released and interception is disabled |
| Exit the application              | Hook is removed and normal input is restored                      |
| Load invalid settings             | Safe defaults or recovery options are offered                     |
| Leave the application idle        | CPU usage remains low                                             |

### Performance targets

These are engineering targets to measure during testing, not guarantees:

- Keyboard-hook processing should remain lightweight.
- The movement engine should target a 4–12 ms update interval when configured for high responsiveness, subject to Windows scheduling.
- Normal keypad input should resume immediately after successful deactivation.
- Idle CPU usage should be negligible on a typical Windows desktop.
- The application should not create a new thread or process for each movement action.

For responsiveness, we should measure the time between a physical key event and the resulting mouse event, as well as cursor movement consistency during sustained input. A shorter configured interval does not automatically mean better performance.

## 14. Installation and dependencies

For the first development version, I would target Windows 10/11 and use a supported Python version compatible with PySide6.

Install the core GUI dependency in a virtual environment:

```
mkdir numpad_mouse
cd numpad_mouse

py -m venv .venv
.venv\Scripts\activate

python -m pip install --upgrade pip
pip install PySide6
```

The Windows hook and mouse backends can initially use Python's `ctypes` and the Windows API, avoiding an unnecessary dependency on external input libraries.

For testing and packaging, add the relevant development dependencies later:

```
pip install pytest pytest-qt pyinstaller
```

`pytest-qt` is useful for GUI tests; `pyinstaller` is for packaging the finished application. We should pin and test dependency versions before creating a release.

## 15. Final definition of the finished application

The deliverable should be a complete Windows application, not just a Python script.

- A polished PySide6 settings window.
- A configurable keypad-to-mouse keymap.
- Exclusive keypad interception while mouse mode is active.
- Adjustable movement speed, acceleration and repeat timing.
- Eight-direction movement and precision mode.
- Left, right and middle clicks, plus dragging.
- Configurable vertical and horizontal scrolling.
- Presets and custom profiles.
- Persistent JSON configuration.
- System-tray background operation.
- Global activation and emergency-stop shortcuts.
- Reliable button release and hook cleanup.
- Diagnostics, tests and error recovery.
- Windows executable packaging and usage documentation.

My strongest recommendation: build and test the keyboard interception and safety mechanisms before investing time in the visual interface. If the core input system works correctly, the rest of the application becomes much easier to develop.

The ideal first release is a reliable controller with a good settings GUI, customizable movement and scrolling, and tray support. Profile import/export, per-application mappings and more advanced customization can follow without requiring a redesign of the core architecture.
