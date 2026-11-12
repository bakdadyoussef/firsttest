# NumPad Mouse Control

## Complete Product, Architecture, UX, Safety, and Development Plan

**Document type:** Software project specification and implementation
roadmap\
**Target platform:** Windows 10/11\
**Language:** Python\
**GUI framework:** PySide6 / Qt\
**Primary purpose:** Turn the numeric keypad into a configurable mouse
controller while preventing keypad keys from typing numbers whenever
mouse mode is active.

------------------------------------------------------------------------

## 1. Executive Summary

NumPad Mouse Control is a Windows desktop accessibility utility for
users who do not have a physical mouse available or who prefer to
control the pointer from the numeric keypad. It should offer
significantly more configurable behavior than Windows' built-in Mouse
Keys.

The application will intercept keypad input only while its mouse mode is
enabled, translate key events into pointer movement, mouse-button
actions, dragging, and scrolling, and provide a polished PySide6 control
panel. It must be able to remain active in the background through a
system-tray icon while its main window is hidden.

The most important success criterion is not the number of features. It
is **reliable control and reliable recovery**. The user must always have
a practical way to disable interception, restore normal numeric-keypad
input, and release mouse buttons if something goes wrong.

### Product goals

1.  Convert numeric-keypad input into mouse actions.
2.  Suppress original keypad input only while mouse mode is enabled.
3.  Provide adjustable speed, acceleration, repeat delay, update
    interval, precision movement, and scrolling.
4.  Support clicking, double-clicking, holding, dragging, and multiple
    mouse buttons.
5.  Allow key remapping and custom movement profiles through the GUI.
6.  Minimize to the system tray and continue working in the background.
7.  Save and validate settings reliably.
8.  Keep idle CPU use low and avoid blocking the keyboard hook.
9.  Include emergency-stop, recovery, diagnostics, tests, and packaging.

### Non-goals for the first release

-   Controlling secure Windows screens such as the UAC secure desktop.
-   Guaranteeing control over applications running at a higher integrity
    level than the controller.
-   Supporting Linux or macOS in the first release.
-   Using AI or cloud services.
-   Executing arbitrary Python code from user-created keymaps.
-   Requiring administrator privileges for normal operation.

------------------------------------------------------------------------

## 2. Core Design Principles

### 2.1 Safety before convenience

The application must have a global emergency shortcut that does not
depend on the numeric keypad. Emergency stop should stop movement,
release any mouse buttons held by the application, disable keypad
interception, and return the controller to a known state.

### 2.2 GUI and input processing must be separated

The GUI must not be responsible for processing every key event or
movement tick. A frozen settings window must not prevent input cleanup.
Use separate, well-defined components and communicate with the GUI
through Qt signals or other thread-safe mechanisms.

### 2.3 Input handling must be explicit

Track key-down and key-up events. Do not rely solely on repeated
key-down events because repeat behavior varies by keyboard and Windows
settings. Movement should stop when relevant keys are released or when a
safety reset clears the input state.

### 2.4 Configuration must be recoverable

Validate settings before activating the hook. Save configuration
atomically where possible and retain a last-known-good backup. Invalid
settings must never prevent the user from opening the application and
disabling mouse mode.

### 2.5 Start safely

The recommended default is to launch the application with mouse mode
disabled. Automatic activation at login should be an explicit opt-in
setting after the user has tested the controller.

### 2.6 Keep the implementation maintainable

Keep Windows API calls in a Windows-specific backend. Keep movement
mathematics separate from the GUI. Use clear interfaces so that the
backend or GUI can be improved without rewriting the whole application.

------------------------------------------------------------------------

## 3. Feature Specification

The project is divided into eight main modules.

### A. Mouse engine

Responsibilities:

-   Move the pointer in eight directions.
-   Support simultaneous direction keys and diagonal movement.
-   Perform left, right, and middle clicks.
-   Support double-clicking.
-   Hold and release mouse buttons.
-   Support drag-and-drop.
-   Generate vertical and horizontal scroll input.
-   Track buttons held by the controller.
-   Release held buttons during deactivation and shutdown.

### B. Keyboard engine

Responsibilities:

-   Install and remove the Windows keyboard hook.
-   Detect keypad key-down and key-up events.
-   Normalize raw Windows events into stable internal key names.
-   Distinguish keypad keys from the top-row number keys.
-   Suppress original keypad input while mouse mode is enabled.
-   Pass keypad input through when mouse mode is disabled.
-   Handle Num Lock state according to the configured policy.
-   Detect and recover from stale key states.

### C. Movement and timing controller

Responsibilities:

-   Calculate movement based on elapsed time.
-   Support base speed and maximum speed.
-   Apply configurable acceleration curves.
-   Support initial repeat delay and continuous movement.
-   Normalize diagonal speed.
-   Implement precision mode.
-   Support optional temporary speed boost.
-   Keep movement smooth under minor Windows scheduling delays.

### D. Settings and keymap manager

Responsibilities:

-   Store and validate configuration.
-   Provide a configurable key-to-action map.
-   Detect conflicting assignments.
-   Support unassigned keys.
-   Restore default mappings.
-   Import and export profiles.
-   Migrate older configuration schema versions.

### E. PySide6 GUI

Responsibilities:

-   Present dashboard and settings pages.
-   Show controller and hook status.
-   Show the current keypad map.
-   Edit speed, acceleration, timing, and scrolling settings.
-   Manage mouse-button behavior and profiles.
-   Provide accessible controls, keyboard navigation, and clear status
    messages.

### F. Background and tray controller

Responsibilities:

-   Minimize to the Windows notification area.
-   Restore the main window without restarting the controller.
-   Provide tray actions to enable or disable mouse mode.
-   Provide emergency stop and exit actions.
-   Optionally start with Windows.
-   Keep application state clear when the GUI is hidden.

### G. Safety and recovery

Responsibilities:

-   Provide a global emergency-stop shortcut.
-   Release all mouse buttons held by the application.
-   Stop movement and clear key state.
-   Disable interception on fatal hook errors.
-   Detect hook health where practical.
-   Ensure cleanup can run safely more than once.
-   Offer a Safe Mode that restores normal keypad input.

### H. Configuration and diagnostics

Responsibilities:

-   Load and save JSON configuration.
-   Validate ranges, types, and action names.
-   Maintain logs for unexpected errors.
-   Report hook status, configuration status, and recent errors.
-   Measure approximate CPU use and movement-update statistics.
-   Support troubleshooting without exposing unnecessary sensitive data.

### Feature priority

  -----------------------------------------------------------------------
  Priority                            Features
  ----------------------------------- -----------------------------------
  Essential                           Keyboard interception, input
                                      restoration, movement, clicking,
                                      scrolling, emergency stop

  High                                Acceleration, configurable timing,
                                      dragging, tray operation,
                                      persistent settings

  Advanced                            Custom keymaps, multiple profiles,
                                      precision mode, horizontal
                                      scrolling

  Later                               Per-application profiles, startup
                                      integration, portable packaging,
                                      extra accessibility features
  -----------------------------------------------------------------------

------------------------------------------------------------------------

## 4. Default Keypad Layout

The first release should ship with a practical default layout. Every
assignment must remain configurable.

### Direction keys

  Keypad key   Default action
  ------------ -----------------
  KP 7         Move up-left
  KP 8         Move up
  KP 9         Move up-right
  KP 4         Move left
  KP 5         Stop movement
  KP 6         Move right
  KP 1         Move down-left
  KP 2         Move down
  KP 3         Move down-right

**Design choice:** KP 5 should stop movement by default rather than
click. This gives the user an immediate way to stop the cursor without
accidentally clicking an object.

### Button and scrolling keys

  Keypad key    Default action
  ------------- -----------------------------------
  KP 0          Hold left mouse button while held
  KP Decimal    Left click
  KP Divide     Right click
  KP Multiply   Middle click
  KP Add        Scroll up
  KP Subtract   Scroll down

The exact physical key identity can differ by keyboard and Num Lock
state. The implementation must use Windows scan-code and virtual-key
information carefully and test both Num Lock states. The keymap editor
should display the actual key detected during assignment.

### Required keymap capabilities

-   Reassign any supported keypad key to a supported action.
-   Leave a key unassigned.
-   Choose tap, hold, or toggle activation where the action supports it.
-   Detect duplicate or conflicting assignments.
-   Cancel an in-progress "press a key to assign" operation.
-   Restore the default keymap.
-   Export and import a validated mapping.
-   Never interpret a keymap file as executable code.

### Click and drag behavior

Recommended defaults:

-   KP Decimal performs a left click.
-   KP 0 holds the left button while physically held, enabling drag when
    movement keys are pressed at the same time.
-   Releasing KP 0 releases the left button.
-   Optional drag-lock mode is off by default.
-   Emergency stop always releases all buttons held by the application.

A configurable double-click action may be added. If double-click is
assigned to a key, define its timing clearly and ensure it cannot
accidentally trigger repeated double-clicks while the key is held.

------------------------------------------------------------------------

## 5. Movement Engine Design

Movement quality is the core of the project. The controller should not
merely move the cursor by a fixed number of pixels every time a timer
callback occurs.

### 5.1 Event-driven input state

The keyboard hook records key-down and key-up events. A state manager
tracks the set of direction keys currently held. The movement controller
reads this state and calculates movement only while one or more
direction keys are active.

The hook callback must remain short. It should not perform expensive
calculations, write logs synchronously, save files, or update GUI
widgets directly.

### 5.2 Time-based movement

Use a monotonic clock to measure elapsed time. This prevents movement
speed from depending entirely on how often a timer happens to fire.

A conceptual acceleration model is:

`v(t) = min(v_max, v_0 + a * t)`

Where:

-   `v_0` is the initial speed.
-   `a` is the acceleration rate.
-   `v_max` is the maximum speed.
-   `t` is the time the direction key has been held.

This is a conceptual model. The implementation should choose units
consistently and calculate displacement from elapsed time. A smooth
acceleration curve may feel better than a sudden speed jump.

### 5.3 Diagonal normalization

When two directions are held simultaneously, combine their horizontal
and vertical components into a direction vector and normalize it.
Without normalization, diagonal movement can be faster than horizontal
or vertical movement.

The controller should also accumulate fractional pixel movement instead
of discarding fractional values every update. This improves precision at
low speeds.

### 5.4 Proposed configurable ranges

These are initial product-design ranges, not guaranteed hardware
performance limits.

  Setting                                              Proposed range
  ---------------------------- --------------------------------------
  Base speed                       1--30 pixels per update-equivalent
  Maximum speed                  10--100 pixels per update-equivalent
  Acceleration multiplier                                       0--5×
  Repeat delay                                              0--800 ms
  Movement update interval                                   4--40 ms
  Precision speed multiplier                                0.1--1.0×
  Diagonal normalization                           Enabled by default

The UI labels must explain units clearly. Internally, prefer a
time-based speed unit such as pixels per second so changing the update
interval does not unintentionally change the overall movement speed.

### 5.5 Repeat behavior

Support two related but distinct settings:

-   **Repeat delay:** How long the key must remain held before
    continuous movement behavior begins, if the selected mode uses a
    delay.
-   **Update interval:** How often the controller checks state and
    generates movement while a key is held.

The first key-down should feel responsive. The application should not
artificially wait a long time before the cursor begins to move.

### 5.6 Profiles

Ship with three editable presets:

**Precision** - Slow initial movement. - Low maximum speed. - Gentle or
no acceleration. - Low precision multiplier. - Intended for small
targets, text selection, and accurate positioning.

**Balanced** - Moderate initial speed. - Gradual acceleration. - Medium
maximum speed. - Recommended default profile.

**Fast** - Higher initial speed. - Faster acceleration. - Higher maximum
speed. - Intended for crossing large screens quickly.

Every preset should be editable. Users should be able to save custom
profiles without editing source code.

### 5.7 Responsiveness targets

The implementation should target a configurable 4--12 ms movement-update
interval for high-responsiveness operation, subject to Windows
scheduling and actual hardware. A lower interval does not automatically
guarantee better perceived performance.

Measure at least:

-   Time from key event to generated mouse event.
-   Movement consistency during sustained key holds.
-   CPU usage while idle and during movement.
-   Behavior when Windows briefly delays callbacks.

------------------------------------------------------------------------

## 6. Scrolling and Mouse-Button Behavior

### 6.1 Scrolling

Required features:

-   Vertical scrolling up and down.
-   Configurable wheel amount per action.
-   Continuous scrolling while a key is held.
-   Independent scroll repeat delay and interval.
-   Optional scroll acceleration.
-   Horizontal scrolling where supported by the target application.
-   Precision scrolling mode.
-   Separate movement and scrolling speed settings.

Accumulate fractional wheel deltas when necessary rather than discarding
small values. Windows wheel input has defined units, but applications
may interpret wheel events differently. Test behavior in browsers, file
managers, text editors, and other common applications.

### 6.2 Mouse-button state

The engine should maintain explicit state for each button it presses or
releases. Do not assume that a key-up event will always arrive normally.

For dragging:

1.  Press the configured drag key.
2.  Generate a left-button-down event.
3.  Move using the direction keys.
4.  Release the drag key.
5.  Generate a left-button-up event.

If the controller deactivates, the hook fails, or the application exits,
release every mouse button held by the controller.

### 6.3 Click behavior

Support:

-   Left, right, and middle click.
-   Optional double-click.
-   Hold-to-drag.
-   Optional drag lock.
-   Optional left/right button swap.
-   Configurable button assignments.
-   Safe release on cancellation.

Keep drag-lock disabled by default because it can cause accidental
dragging.

------------------------------------------------------------------------

## 7. PySide6 GUI Specification

The GUI should be a compact desktop control center, not a single dialog
containing every setting. Use a navigation rail or sidebar, a main
content panel, consistent spacing, readable text, and clear status
indicators. The interface should remain usable on modest screen
resolutions.

### 7.1 Visual direction

Suggested visual style:

-   Dark neutral background.
-   Deep emerald accent, approximately `#155B3A`.
-   White or light-gray text with accessible contrast.
-   Clear active/inactive status indicators.
-   Consistent spacing, button sizes, and typography.
-   Avoid excessive animation and unnecessary visual effects.
-   Support keyboard navigation through all settings.
-   Provide visible focus indicators and meaningful accessible names.

### 7.2 Dashboard

Display:

-   Controller state: Active or Inactive.
-   Keyboard-hook health.
-   Current movement profile.
-   Num Lock state.
-   Quick activation and deactivation controls.
-   Interactive keypad preview.
-   Current movement and scroll settings.
-   Restore-default-configuration action.
-   A clear warning if the hook fails or configuration is invalid.

### 7.3 Movement page

Controls:

-   Base speed.
-   Maximum speed.
-   Acceleration amount and curve.
-   Repeat delay.
-   Update interval.
-   Diagonal normalization.
-   Precision speed multiplier.
-   Precision-mode activation key.
-   Optional temporary speed boost.
-   Profile selector.

Safe adjustments should take effect immediately without restarting the
application.

### 7.4 Mouse Buttons page

Controls:

-   Left, right, and middle click assignments.
-   Double-click behavior and timing.
-   Drag key.
-   Drag-lock mode.
-   Button swapping.
-   Click-and-hold behavior.
-   Recovery of stuck buttons.

### 7.5 Scrolling page

Controls:

-   Vertical and horizontal scrolling.
-   Scroll increment.
-   Scroll repeat delay.
-   Scroll repeat interval.
-   Scroll acceleration.
-   Precision scrolling mode.

### 7.6 Keymap Editor page

Use a table with:

  -----------------------------------------------------------------------
  Column                              Purpose
  ----------------------------------- -----------------------------------
  Physical key                        Detected keypad key

  Assigned action                     Action triggered by the key

  Activation type                     Tap, hold, or toggle where
                                      supported

  Conflict status                     Whether the assignment conflicts
                                      with another mapping
  -----------------------------------------------------------------------

Include a "Press a key to assign" interaction, cancel action, unassign
action, conflict explanation, and "Restore default keymap" button.

The assignment dialog should time out or be cancellable. It must not
leave the application in a mode where arbitrary keys are unexpectedly
intercepted.

### 7.7 Profiles page

Allow users to:

-   Create a profile.
-   Rename a profile.
-   Duplicate a profile.
-   Edit settings.
-   Delete custom profiles.
-   Restore built-in presets.
-   Import and export profile files.

Protect required built-in profiles from accidental deletion, or allow
restoring them easily.

### 7.8 Background and Startup page

Settings:

-   Minimize to system tray.
-   Close button minimizes rather than exits.
-   Start automatically with Windows.
-   Start with mouse mode disabled or enabled.
-   Show or hide notifications.
-   Configure activation shortcut.
-   Configure emergency-stop shortcut.

Default startup behavior should leave mouse mode disabled. Automatic
activation must be an explicit, tested opt-in setting.

### 7.9 Diagnostics page

Display:

-   Keyboard-hook status.
-   Mouse-button state.
-   Current keymap.
-   Recent errors.
-   Last successful configuration save.
-   Approximate CPU usage.
-   Movement-update statistics.
-   Current application and configuration version.

Provide a Safe Mode button that disables interception and releases held
buttons.

------------------------------------------------------------------------

## 8. Background Operation and System Tray

Background operation is a core requirement.

Use `QSystemTrayIcon` and a tray context menu. Hiding the main window
must not stop the keyboard hook or movement controller.

### Proposed tray menu

1.  **Enable mouse mode** --- activate interception and mouse control.
2.  **Disable mouse mode** --- release held buttons and restore ordinary
    keypad input.
3.  **Open settings** --- restore the GUI without restarting the
    controller.
4.  **Emergency stop** --- stop movement, release held buttons, and
    disable interception.
5.  **Exit application** --- clean up all input state, remove the hook,
    stop threads, save settings, and quit.

### Close-window behavior

When the user closes the main window:

-   If "Minimize to tray" is enabled, hide the window and keep the
    controller running.
-   If it is disabled, offer a clear choice between minimizing and
    exiting, or follow a documented user preference.
-   On exit, release held buttons, disable interception, stop movement
    processing, remove the hook, stop threads, and save settings.

### Distinguish three states

1.  **Application running:** The process remains active in the
    background.
2.  **Mouse mode active:** The keypad is intercepted.
3.  **Movement in progress:** One or more movement keys are held.

The interface and tray icon should make these states clear.

### Startup with Windows

Use the user's Startup folder or a supported per-user startup mechanism.
Do not require administrator privileges for normal startup integration.

On first installation, launch with mouse mode disabled. Allow users to
opt into automatic activation only after confirming that the controller
works on their system.

------------------------------------------------------------------------

## 9. Technical Architecture

### 9.1 Proposed architecture

``` text
Physical numeric keypad
          |
          v
Windows low-level keyboard hook
          |
          v
Check whether mouse mode is active
          |
      +---+---+
      |       |
   Inactive  Active
      |       |
 Pass input   Resolve keymap action
 to Windows          |
                      v
                Input state manager
                      |
                      v
             Movement / button / scroll
                   controllers
                      |
                      v
               Windows SendInput
                      |
                      v
              Cursor and mouse actions

PySide6 GUI <--> Configuration / Profile Manager
       |                    |
       v                    v
Tray controller       Validated settings

Safety controller --> stop movement, release buttons,
                      disable interception, cleanup
```

### 9.2 Recommended technology stack

  -----------------------------------------------------------------------
  Component               Technology              Reason
  ----------------------- ----------------------- -----------------------
  GUI                     PySide6                 Modern and flexible
                                                  desktop interface

  Keyboard interception   Windows                 Global key detection
                          `WH_KEYBOARD_LL` hook   and suppression
                          via `ctypes`            

  Mouse input             Windows `SendInput` API Pointer, button, and
                                                  wheel events

  Background operation    `QSystemTrayIcon` plus  Run while the main
                          hook thread             window is hidden

  Timing                  Monotonic clock and     Consistent movement
                          controlled update loop  calculations

  Configuration           Python `json` module    Human-readable
                                                  configuration

  Logging                 Python `logging`        Diagnostics and error
                                                  investigation

  Testing                 `pytest`, optionally    Automated logic and GUI
                          `pytest-qt`             tests

  Packaging               PyInstaller             Build a Windows
                                                  executable
  -----------------------------------------------------------------------

### 9.3 Why use the Windows API directly?

Higher-level libraries can help with early prototypes. However, reliable
suppression of original keypad input, tracking of key-down and key-up
events, precise button state, and orderly cleanup are central
requirements.

The primary backend should use the Windows API directly through a
carefully tested wrapper. Higher-level libraries may be used for
optional features, but the core should not depend on multiple loosely
coordinated automation loops.

### 9.4 Threading model

**GUI thread** - Draws the interface. - Manages settings and profiles. -
Receives status updates. - Never blocks on lengthy hook or input
operations.

**Keyboard-hook thread** - Installs and owns the hook. - Runs the
required Windows message loop. - Processes key events quickly. - Does
not write files or perform expensive operations inside the hook
callback.

**Movement controller** - Reads normalized input state. - Calculates
movement using elapsed time. - Produces mouse input through the
backend. - Sleeps or waits when there is no movement to process.

**Safety controller** - Handles emergency stop. - Coordinates
deactivation and shutdown. - Clears stale state and releases buttons. -
Reports failures to the GUI through thread-safe signals.

Use Qt signals or other explicit thread-safe mechanisms for
communication. Avoid creating a thread for every key event or movement
tick.

### 9.5 Backend interface

Keep Windows-specific API code behind an input-backend interface. The
interface should provide operations conceptually equivalent to:

-   Install keyboard hook.
-   Remove keyboard hook.
-   Send relative mouse movement.
-   Press or release a mouse button.
-   Send vertical or horizontal wheel input.
-   Report backend status.
-   Perform safe cleanup.

This keeps the GUI and movement algorithms independent from low-level
Windows details.

------------------------------------------------------------------------

## 10. Project Folder Structure

``` text
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

**Important packaging note:** A project package named `platform` can
shadow Python's standard-library `platform` module. Prefer a safer
package name such as `backends/windows/` instead. The recommended final
structure is therefore:

``` text
backends/
└── windows/
    ├── keyboard_backend.py
    ├── mouse_backend.py
    └── winapi.py
```

### File responsibilities

-   `main.py`: Application entry point and top-level exception handling.
-   `application.py`: Coordinates startup, shutdown, and major
    components.
-   `state.py`: Stores application mode and high-level state.
-   `signals.py`: Defines Qt signals used for safe communication.
-   `keyboard_hook.py`: Owns hook lifecycle and normalizes events.
-   `key_translator.py`: Converts normalized keys to configured actions.
-   `input_state.py`: Tracks held keys and mouse buttons.
-   `mouse_engine.py`: Sends mouse actions through the backend.
-   `movement_controller.py`: Calculates speed, acceleration, and
    diagonal movement.
-   `scroll_controller.py`: Generates vertical and horizontal scrolling.
-   `safety_controller.py`: Handles emergency stop and cleanup.
-   `config_manager.py`: Loads, validates, saves, and migrates settings.
-   `profile_manager.py`: Manages built-in and custom profiles.
-   `tray_manager.py`: Handles tray menu, window restoration, and exit.
-   `keymap_editor.py`: Provides interactive key assignment.
-   `diagnostics.py`: Collects status, logs, and performance
    measurements.

------------------------------------------------------------------------

## 11. Configuration and Data Model

The configuration should be versioned. New releases must be able to add
settings without breaking existing user files.

### 11.1 Example configuration

The following is a proposed schema, not a finished or fully validated
configuration. The implementation must define precise units, validate
all fields, and normalize key identities.

``` json
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

### 11.2 Validation rules

Validate at least:

-   Schema version is supported.
-   Required fields exist or can be supplied from defaults.
-   Numeric values fall within allowed ranges.
-   Booleans are actual booleans.
-   Action names belong to a known action registry.
-   Key names belong to the normalized key-name registry.
-   Mappings do not violate conflict rules.
-   Hotkey combinations are valid and do not conflict with reserved
    actions.
-   Profiles contain only supported settings.

Invalid configuration should result in a clear message and recovery
options. Do not activate interception using unvalidated settings.

### 11.3 Safe saving

1.  Load defaults if no configuration exists.
2.  Parse and validate the current configuration.
3.  Write changes to a temporary file.
4.  Flush and close the file.
5.  Replace the previous configuration atomically where supported.
6.  Keep a backup of the last valid configuration.
7.  Migrate older versions using explicit migration functions.
8.  If a save fails, preserve the previous valid file and report the
    error.

Store user configuration in an appropriate per-user application-data
directory rather than assuming the current working directory is
writable.

------------------------------------------------------------------------

## 12. Safety, Recovery, and Windows Limitations

### 12.1 Emergency stop

Use a configurable global shortcut independent of keypad keys. Test the
default shortcut for conflicts with Windows and common applications.

When triggered:

1.  Stop movement updates.
2.  Clear all active direction keys.
3.  Release every mouse button held by the controller.
4.  Disable keypad interception.
5.  Update tray and GUI status.
6.  Log the event without blocking the input callback.

### 12.2 Button-release cleanup

Cleanup must be idempotent: calling it more than once must be safe.
Track only the button states owned by the controller, and do not blindly
generate unrelated input.

Run cleanup on:

-   Normal deactivation.
-   Emergency stop.
-   Application exit.
-   Fatal backend error.
-   Hook failure.
-   Any recovery path that clears input state.

### 12.3 Stale key states

A key-up event may be missed because of unusual focus changes, device
behavior, or application state changes. Use explicit pressed-key
tracking and conservative recovery rules. Clear stale states when mouse
mode is disabled, when the backend is reset, or when a verified safety
condition requires it.

Avoid aggressive timeouts that interrupt legitimate long key holds.

### 12.4 Hook health

A low-level Windows hook can fail or be removed if the callback does not
return promptly. Keep callbacks short, monitor hook health where
practical, and make failures visible. If the hook is known to be
unusable, stop movement, release held buttons, and enter a safe state
rather than pretending that the controller remains operational.

### 12.5 Privilege boundaries

An ordinary process may not be able to inject input into every elevated
application or control the secure desktop. Do not automatically elevate
the application as a workaround. Document the limitation and avoid
implying that all Windows screens can be controlled.

### 12.6 Num Lock and hardware differences

Test both Num Lock states. Windows may report different virtual-key
values or scan-code information depending on keyboard layout and state.
Prefer reliable physical-key identification where possible and provide
an interactive mapping capture flow.

### 12.7 Startup safety

Start with mouse mode disabled by default. Automatic activation at login
should require explicit opt-in. Include a way to launch with
interception disabled for troubleshooting.

### 12.8 Logging and privacy

Logs should record operational errors and state transitions without
collecting unrelated keystrokes. Never log arbitrary keyboard text.
Avoid retaining more diagnostic data than is needed to debug the
controller.

------------------------------------------------------------------------

## 13. Development Roadmap

Build and test in seven phases. Every phase should produce a working
result before the next begins.

### Phase 1 --- Foundation and safety

Tasks:

-   Create the Python project and virtual environment.
-   Create a minimal PySide6 window.
-   Implement the Windows API wrapper.
-   Install and remove the keyboard hook.
-   Detect keypad down/up events.
-   Add activation, deactivation, and emergency stop.
-   Confirm normal keypad input is restored correctly.
-   Test cleanup after simulated errors.

**Exit criteria:** The application can enable and disable interception
safely, restore normal keypad input, and remove its hook.

### Phase 2 --- Core mouse control

Tasks:

-   Implement eight-direction movement.
-   Add the stop key.
-   Implement left, right, and middle clicks.
-   Add hold-to-drag behavior.
-   Add button-release cleanup.
-   Implement basic vertical and horizontal scrolling.
-   Test Num Lock on and off.

**Exit criteria:** The keypad can control the cursor and mouse buttons
without typing digits while mouse mode is active.

### Phase 3 --- Movement tuning

Tasks:

-   Add base and maximum speed.
-   Add acceleration and repeat timing.
-   Implement elapsed-time-based movement.
-   Add diagonal normalization.
-   Add precision mode.
-   Add scrolling repeat behavior.
-   Measure responsiveness and CPU use.

**Exit criteria:** Movement feels predictable at slow and fast settings
and remains reasonably consistent under normal Windows scheduling.

### Phase 4 --- Complete settings interface

Tasks:

-   Build dashboard and settings pages.
-   Add interactive keypad preview.
-   Implement keymap editing and conflict detection.
-   Add profiles.
-   Add persistent configuration and validation.
-   Add restore-default options.
-   Add import/export if time permits.

**Exit criteria:** Normal usage does not require source-code edits to
change behavior.

### Phase 5 --- Background operation

Tasks:

-   Implement system-tray icon and context menu.
-   Add minimize-to-tray behavior.
-   Add global activation and emergency shortcuts.
-   Add optional startup integration.
-   Ensure GUI restoration does not restart the input backend.
-   Test close, minimize, restore, and exit flows.

**Exit criteria:** The application can run in the tray and continue
controlling the cursor without its window visible.

### Phase 6 --- Testing and hardening

Tasks:

-   Test rapid presses and simultaneous keys.
-   Test key-up loss and stale-state recovery.
-   Test hook failure and cleanup.
-   Test invalid configuration recovery.
-   Test emergency stop in every application state.
-   Measure idle CPU use.
-   Test multiple screen sizes and display scaling.
-   Test elevated-app limitations and document them.

**Exit criteria:** No known common workflow leaves keypad interception
active unexpectedly or leaves controller-held mouse buttons stuck.

### Phase 7 --- Release

Tasks:

-   Package with PyInstaller.
-   Include icon assets and user documentation.
-   Test a clean installation and first launch.
-   Verify configuration storage and permissions.
-   Verify startup integration and clean shutdown.
-   Produce a release checklist and troubleshooting guide.

**Exit criteria:** A Windows executable can be launched and used without
a Python development environment.

------------------------------------------------------------------------

## 14. Testing Plan and Acceptance Criteria

The project is not complete just because the cursor moves. It is
complete when the entire workflow is reliable.

  -----------------------------------------------------------------------
  Test                                Expected result
  ----------------------------------- -----------------------------------
  Press a direction key               Cursor moves in the correct
                                      direction

  Hold a direction key                Smooth continuous movement

  Hold two directions                 Normalized diagonal movement

  Release direction keys              Movement stops promptly

  Change speed in GUI                 New setting takes effect safely

  Press keypad digits in mouse mode   They do not type numbers

  Disable mouse mode                  Keypad returns to normal numeric
                                      input

  Toggle Num Lock                     Behavior matches configured policy

  Hold drag key and move              Object can be dragged

  Release drag key                    Left button is released

  Scroll while moving                 Actions do not interfere
                                      unexpectedly

  Minimize to tray                    Controller continues working

  Restore GUI                         State and settings remain correct

  Emergency stop                      Movement stops, buttons release,
                                      interception disables

  Exit application                    Hook removed and normal input
                                      restored

  Load invalid settings               Safe defaults or recovery options
                                      appear

  Leave application idle              CPU usage remains low

  Hook/backend failure                Controller enters safe state and
                                      reports failure

  Rapid key transitions               No persistent stuck movement or
                                      button state

  Multiple monitors                   Pointer movement behaves
                                      consistently across displays
  -----------------------------------------------------------------------

### Performance targets

Targets to measure, not guarantees:

-   Keyboard-hook callback remains lightweight.
-   High-responsiveness mode targets a configurable 4--12 ms update
    interval.
-   Input latency is measured from key event to generated mouse event.
-   Normal keypad input resumes after successful deactivation.
-   Idle CPU usage is negligible on a typical desktop.
-   No thread or process is created for each movement action.

A lower timer interval alone does not guarantee lower end-to-end
latency. Measure real behavior rather than relying on the configured
interval as a proxy.

### Suggested automated tests

-   Keymap parsing and conflict validation.
-   Movement-vector calculation.
-   Diagonal normalization.
-   Acceleration and elapsed-time calculations.
-   Scroll accumulation.
-   Configuration validation and migration.
-   Input-state reset behavior.
-   Cleanup idempotency.
-   Profile create/edit/delete operations.
-   GUI interactions using `pytest-qt` where useful.

Low-level Windows hook behavior requires integration tests on an actual
Windows environment in addition to unit tests.

------------------------------------------------------------------------

## 15. Installation and Development Environment

Target Windows 10/11 and a Python version supported by the selected
PySide6 release.

Open PowerShell:

``` powershell
mkdir numpad_mouse
cd numpad_mouse

py -m venv .venv
.venv\Scripts\activate

python -m pip install --upgrade pip
pip install PySide6
```

For testing and packaging, add development tools:

``` powershell
pip install pytest pytest-qt pyinstaller
```

Use `ctypes` and the Windows API for the initial input backend, avoiding
unnecessary dependencies. Pin and test dependency versions before
release.

A `requirements.txt` should distinguish runtime dependencies from
optional development dependencies. Since the core backend uses the
Python standard library for Windows API access, PySide6 is the primary
runtime dependency.

------------------------------------------------------------------------

## 16. Packaging and Release Requirements

Use PyInstaller after the application has passed integration tests.

Release tasks:

-   Build a Windows executable.
-   Include the application icon and required assets.
-   Confirm Qt plugins are included correctly.
-   Test on a clean Windows account or machine.
-   Verify configuration is written to a per-user data directory.
-   Verify the app starts with mouse mode disabled on first launch.
-   Test tray behavior when the window is hidden.
-   Test application exit and hook removal.
-   Document limitations related to elevated applications and secure
    screens.
-   Provide troubleshooting steps for missing tray icons, hook errors,
    and configuration corruption.

An installer can be added later. The first release can be a tested
executable with a clear README.

------------------------------------------------------------------------

## 17. Definition of Done

The first stable release must include:

-   [ ] A polished PySide6 settings window.
-   [ ] Configurable numeric-keypad-to-mouse mapping.
-   [ ] Exclusive keypad interception while mouse mode is active.
-   [ ] Normal keypad input restored after deactivation.
-   [ ] Adjustable movement speed, acceleration, and repeat timing.
-   [ ] Eight-direction movement and precision mode.
-   [ ] Diagonal normalization.
-   [ ] Left, right, and middle clicks.
-   [ ] Double-click support or a clearly scoped implementation
    decision.
-   [ ] Hold-to-drag behavior.
-   [ ] Configurable vertical and horizontal scrolling.
-   [ ] Editable presets and custom profiles.
-   [ ] Persistent, validated JSON configuration.
-   [ ] System-tray background operation.
-   [ ] Global activation and emergency-stop shortcuts.
-   [ ] Reliable button release and hook cleanup.
-   [ ] Diagnostics and error recovery.
-   [ ] Automated tests for core logic.
-   [ ] Windows integration testing.
-   [ ] Executable packaging and user documentation.

------------------------------------------------------------------------

## 18. Recommended Implementation Order

The strongest recommendation is to build and verify keyboard
interception and safety before investing heavily in the GUI.

The practical order is:

1.  **Prove that the Windows hook works.** Detect keypad events and
    suppress them only while enabled.
2.  **Prove that normal input is restored.** Deactivation and exit must
    work correctly.
3.  **Implement movement and mouse actions.** Keep the engine
    independent of the GUI.
4.  **Implement emergency stop and cleanup.** Test failure cases early.
5.  **Tune movement and scrolling.** Measure real responsiveness.
6.  **Build the PySide6 settings pages.** Bind controls to validated
    settings.
7.  **Add the system tray and startup behavior.** Ensure the controller
    remains stable when hidden.
8.  **Add profiles, diagnostics, and packaging.** Finish with
    integration tests and documentation.

The best first release is a reliable controller with a clear settings
GUI, configurable movement and scrolling, a safe keymap editor, and
system-tray support. Advanced features such as per-application profiles
can be added later without redesigning the core architecture.

------------------------------------------------------------------------

## 19. Final Product Vision

NumPad Mouse Control should feel like a small, dependable Windows
accessibility application rather than a fragile automation script.

Its defining qualities should be:

-   **Responsive:** Cursor speed and acceleration are tunable.
-   **Predictable:** Key behavior and profiles are clearly defined.
-   **Configurable:** Users can remap keys and save their preferred
    settings.
-   **Unobtrusive:** It can run in the background without keeping a
    window open.
-   **Safe:** Emergency stop and cleanup are designed into the core.
-   **Recoverable:** Invalid settings or hook failures do not leave the
    user without a route back to normal input.
-   **Maintainable:** GUI, movement, input backend, configuration, and
    safety code are separated.
-   **Testable:** Core logic has automated tests, and Windows
    integration behavior is verified.

**Final recommendation:** Treat the Windows input backend and its
recovery behavior as the foundation of the entire project. Once those
are reliable, the PySide6 interface, profiles, background operation, and
advanced controls can be added in a structured and maintainable way.
