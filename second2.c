#include "USB.h"
#include "USBHIDKeyboard.h"

USBHIDKeyboard Keyboard;

void setup() {
  Keyboard.begin();
  USB.begin();

  // Wait for the PC to recognize the keyboard
  delay(2500);

  // Type the lines
  Keyboard.println("hello");
  delay(300);
  Keyboard.println("this");
  delay(300);
  Keyboard.println("is");
  delay(300);
  Keyboard.println("magical");
  delay(300);
  Keyboard.println("coder");
  delay(800);   // small pause before the combination

  // Press Ctrl + Alt + Delete
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press(KEY_LEFT_ALT);
  Keyboard.press(KEY_DELETE);
  delay(100);                 // hold briefly
  Keyboard.releaseAll();      // release all keys
}

void loop() {
  // nothing
}#include "USB.h"
#include "USBHIDKeyboard.h"

USBHIDKeyboard Keyboard;

void setup() {
  Keyboard.begin();
  USB.begin();

  // Wait for the PC to recognize the keyboard
  delay(2500);

  // Type the lines
  Keyboard.println("hello");
  delay(300);
  Keyboard.println("this");
  delay(300);
  Keyboard.println("is");
  delay(300);
  Keyboard.println("magical");
  delay(300);
  Keyboard.println("coder");
  delay(800);   // small pause before the combination

  // Press Ctrl + Alt + Delete
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press(KEY_LEFT_ALT);
  Keyboard.press(KEY_DELETE);
  delay(100);                 // hold briefly
  Keyboard.releaseAll();      // release all keys
}

void loop() {
  // nothing
}#include "USB.h"
#include "USBHIDKeyboard.h"

USBHIDKeyboard Keyboard;

void setup() {
  Keyboard.begin();
  USB.begin();

  // Wait for the PC to recognize the keyboard
  delay(2500);

  // Type the lines
  Keyboard.println("hello");
  delay(300);
  Keyboard.println("this");
  delay(300);
  Keyboard.println("is");
  delay(300);
  Keyboard.println("magical");
  delay(300);
  Keyboard.println("coder");
  delay(800);   // small pause before the combination

  // Press Ctrl + Alt + Delete
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press(KEY_LEFT_ALT);
  Keyboard.press(KEY_DELETE);
  delay(100);                 // hold briefly
  Keyboard.releaseAll();      // release all keys
}

void loop() {
  // nothing
}#include "USB.h"
#include "USBHIDKeyboard.h"

USBHIDKeyboard Keyboard;

void setup() {
  Keyboard.begin();
  USB.begin();

  // Wait for the PC to recognize the keyboard
  delay(2500);

  // Type the lines
  Keyboard.println("hello");
  delay(300);
  Keyboard.println("this");
  delay(300);
  Keyboard.println("is");
  delay(300);
  Keyboard.println("magical");
  delay(300);
  Keyboard.println("coder");
  delay(800);   // small pause before the combination

  // Press Ctrl + Alt + Delete
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press(KEY_LEFT_ALT);
  Keyboard.press(KEY_DELETE);
  delay(100);                 // hold briefly
  Keyboard.releaseAll();      // release all keys
}

void loop() {
  // nothing
}#include "USB.h"
#include "USBHIDKeyboard.h"

USBHIDKeyboard Keyboard;

void setup() {
  Keyboard.begin();
  USB.begin();

  // Wait for the PC to recognize the keyboard
  delay(2500);

  // Type the lines
  Keyboard.println("hello");
  delay(300);
  Keyboard.println("this");
  delay(300);
  Keyboard.println("is");
  delay(300);
  Keyboard.println("magical");
  delay(300);
  Keyboard.println("coder");
  delay(800);   // small pause before the combination

  // Press Ctrl + Alt + Delete
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press(KEY_LEFT_ALT);
  Keyboard.press(KEY_DELETE);
  delay(100);                 // hold briefly
  Keyboard.releaseAll();      // release all keys
}

void loop() {
  // nothing
}
