# C++ + Qt Calculator — Complete Beginner-Friendly Breakdown

This document explains the calculator program from the ground up, as if you are starting with no programming background. It covers the C++ concepts, the Qt concepts, the calculator's internal state, how the functions connect to each other, and a complete execution trace.

---

# 1. First: what is this program doing?

The program creates a small calculator window that looks roughly like this:

```text
+----------------------+
|                  123 |
+----------------------+
|  7  |  8  |  9  | / |
|  4  |  5  |  6  | * |
|  1  |  2  |  3  | - |
|  0  |  C  |  =  | + |
+----------------------+
```

It allows you to:

- click numbers
- build a number such as `123`
- click an operator such as `+`
- enter another number
- click `=`
- see the result
- clear everything with `C`
- detect division by zero

For example:

```text
7
+
5
=
12
```

The important thing is that the calculator has to **remember things** while you're clicking.

For example, after you press:

```text
7 +
```

the program needs to remember:

```text
currentValue = 7
pendingOperator = "+"
waitingForOperand = true
```

Then when you enter `5`, it knows:

> "I already have 7 and the operator is +, so 5 is the second number."

That idea of **remembering its current state** is the heart of this program.

---

# 2. Before the calculator: what is C++?

Let's start with something very basic.

C++ is a programming language where you write instructions such as:

```cpp
int x = 5;
```

This means:

> Create a variable called `x` that stores the number `5`.

You can then do:

```cpp
x = x + 2;
```

Now `x` is:

```text
7
```

A program is basically a collection of instructions and data.

Our calculator has:

## Data

Things it needs to remember:

```cpp
double currentValue;
QString pendingOperator;
bool waitingForOperand;
```

## Actions

Things it can do:

```cpp
calculate();
onButtonClicked();
```

So conceptually:

```text
Calculator
│
├── remembers data
│   ├── currentValue
│   ├── pendingOperator
│   └── waitingForOperand
│
└── performs actions
    ├── onButtonClicked()
    └── calculate()
```

We'll come back to this constantly.

---

# 3. What is Qt?

The code uses **Qt**.

Qt is a C++ framework that provides ready-made tools for creating graphical applications.

Instead of having to manually create a window from scratch, Qt gives you things like:

```cpp
QWidget
QPushButton
QLineEdit
QGridLayout
QApplication
```

Think of Qt as a huge toolbox.

For example:

```text
QWidget       → window/container
QPushButton   → button
QLineEdit     → text box
QGridLayout   → organizes things into rows and columns
QApplication  → runs the GUI application
```

Your calculator is essentially assembling these Qt components.

---

# 4. The first lines: `#include`

The program begins with:

```cpp
#include <QApplication>
#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QString>
```

Let's understand exactly what this means.

## 4.1 What is `#include`?

`#include` tells C++:

> "I want to use definitions provided by this file/library."

For example:

```cpp
#include <QPushButton>
```

means:

> "I want to use the Qt `QPushButton` class."

Without it, the compiler wouldn't know what this is:

```cpp
QPushButton *btn;
```

It would basically say:

> What is `QPushButton`? I don't know that type.

---

# 5. What is a class?

Next we have:

```cpp
class Calculator : public QWidget {
```

This is extremely important.

A **class** is a blueprint.

Imagine you're designing a robot.

You could say:

```text
Robot
├── battery
├── wheels
├── speed
├── move()
└── stop()
```

Then an actual robot is an object built from that blueprint.

Same idea here.

We create a class called:

```cpp
Calculator
```

This class describes what a calculator object should contain and do.

---

# 6. Why `: public QWidget`?

The full line is:

```cpp
class Calculator : public QWidget {
```

The part:

```cpp
: public QWidget
```

means that `Calculator` **inherits from** `QWidget`.

This is called **inheritance**.

Think of it like:

```text
QWidget
   ↑
   │ inherits
   │
Calculator
```

`QWidget` is already a GUI component/window type supplied by Qt.

By inheriting from it, `Calculator` becomes a kind of QWidget.

So the calculator gets functionality from QWidget, such as:

```cpp
show();
setWindowTitle();
setFixedSize();
```

That's why later we can write:

```cpp
calc.show();
```

because `Calculator` inherited `show()` from `QWidget`.

---

# 7. The `Q_OBJECT` line

Inside the class:

```cpp
Q_OBJECT
```

This is a special Qt macro.

You don't need to think of it as normal C++ syntax.

It tells Qt:

> "This class uses Qt's special object/signals/slots system."

This becomes especially important because later we use:

```cpp
connect(...)
```

and:

```cpp
private slots:
```

Qt's **signals and slots** system is one of its main mechanisms for handling events.

We'll return to it.

---

# 8. `public:`

Next:

```cpp
public:
```

This is an access specifier.

C++ classes can control who can access their members.

Very roughly:

```text
public    → accessible from outside
private   → accessible only inside the class
protected → special inheritance-related access
```

The calculator constructor is public because we need to create a Calculator object from `main()`:

```cpp
Calculator calc;
```

If the constructor weren't accessible, `main()` couldn't create it normally.

---

# 9. The constructor

Now the biggest block:

```cpp
Calculator(QWidget *parent = nullptr) : QWidget(parent) {
```

This is the **constructor**.

A constructor is a special function that runs automatically when an object is created.

When this happens:

```cpp
Calculator calc;
```

C++ automatically calls:

```cpp
Calculator(...)
```

So you can think:

```text
Calculator calc;
      ↓
constructor runs
      ↓
calculator window is prepared
```

---

# 10. Understanding `QWidget *parent`

Let's break this:

```cpp
QWidget *parent = nullptr
```

There are several concepts here.

### `QWidget`

The type.

### `*`

This means it is a **pointer**.

A pointer stores the address of another object.

You don't need to master pointers immediately, but conceptually:

```text
QWidget object
     ↑
     │
pointer
```

### `parent = nullptr`

If no parent is supplied, the value is:

```cpp
nullptr
```

which basically means:

> "There is no parent object."

When we create:

```cpp
Calculator calc;
```

we don't provide a parent, so:

```text
parent = nullptr
```

---

# 11. Constructor initialization

This part:

```cpp
: QWidget(parent)
```

is called an **initializer list**.

Because `Calculator` inherits from `QWidget`, we need to initialize the QWidget part.

So conceptually:

```text
Create Calculator
     ↓
Initialize QWidget portion
     ↓
Run Calculator constructor body
```

The body is everything inside:

```cpp
{
    ...
}
```

---

# 12. Setting the window title

Inside the constructor:

```cpp
setWindowTitle("Simple Calculator");
```

This changes the title displayed in the window's title bar.

Because `Calculator` inherits `QWidget`, it can use QWidget's method:

```cpp
setWindowTitle(...)
```

So:

```text
Calculator
     │
     └── inherited from QWidget
             ↓
       setWindowTitle()
```

---

# 13. Setting the window size

Next:

```cpp
setFixedSize(300, 400);
```

This sets the window size:

```text
width  = 300
height = 400
```

The word **fixed** means the user cannot freely resize the window.

So:

```text
300 pixels wide
400 pixels tall
```

---

# 14. Creating the calculator display

Now we get:

```cpp
display = new QLineEdit(this);
```

This is a very important line.

`QLineEdit` is a Qt widget for displaying/editing a line of text.

Here we're using it as the calculator's display.

For example:

```text
+----------------------+
|                123   |
+----------------------+
```

That top region is the `QLineEdit`.

---

# 15. What does `new` mean?

This part:

```cpp
new QLineEdit(this)
```

creates a new QLineEdit object dynamically.

The result is a pointer to the new object.

That's why we have:

```cpp
display
```

which was declared later in the class as:

```cpp
QLineEdit *display;
```

So the relationship is:

```text
display
   │
   ↓
QLineEdit object
   │
   ├── shows "123"
   ├── shows "0"
   └── can contain calculator text
```

---

# 16. Why `this`?

Here:

```cpp
new QLineEdit(this)
```

`this` means:

> "the current Calculator object."

So you're essentially saying:

> Create this QLineEdit and make the current calculator its parent.

Qt uses parent-child ownership heavily.

Conceptually:

```text
Calculator
   │
   └── QLineEdit
```

That means Qt can manage the child widget's lifetime as part of the parent hierarchy.

---

# 17. Read-only display

Next:

```cpp
display->setReadOnly(true);
```

The arrow:

```cpp
->
```

is used to access something through a pointer.

For example:

```cpp
display->setReadOnly(true);
```

means:

> Call `setReadOnly()` on the QLineEdit object pointed to by `display`.

Because:

```cpp
display
```

is a pointer.

---

# 18. Why read-only?

Normally a `QLineEdit` lets the user type:

```text
hello
12345
abc
```

But this calculator doesn't want the user typing directly into the display.

It wants the buttons to control it.

Therefore:

```cpp
display->setReadOnly(true);
```

means:

```text
User keyboard typing → not allowed
Program changing display → allowed
```

For example the program can still do:

```cpp
display->setText("123");
```

---

# 19. Aligning the number to the right

```cpp
display->setAlignment(Qt::AlignRight);
```

This makes the display text appear on the right side, like a traditional calculator.

Instead of:

```text
| 123                 |
```

you get:

```text
|                 123 |
```

`Qt::AlignRight` is an enum/value provided by Qt.

---

# 20. Setting initial display text

```cpp
display->setText("0");
```

Initially the calculator shows:

```text
0
```

So immediately after creation:

```text
display = "0"
```

---

# 21. Setting the display style

```cpp
display->setStyleSheet("font-size: 24px; padding: 10px;");
```

Qt supports CSS-like styling.

Here:

```css
font-size: 24px;
padding: 10px;
```

means:

- text size is 24 pixels
- add 10 pixels of internal spacing

This affects appearance only.

It doesn't affect the calculation logic.

That's a useful distinction:

```text
GUI appearance
     ≠
calculation logic
```

---

# 22. Creating the grid layout

Now:

```cpp
QGridLayout *layout = new QGridLayout(this);
```

A `QGridLayout` organizes widgets in a grid.

Think of a spreadsheet:

```text
column 0    column 1    column 2    column 3
------------------------------------------------
row 0
row 1
row 2
row 3
row 4
```

This is perfect for a calculator because calculator buttons naturally form a grid.

---

# 23. Adding the display to the grid

```cpp
layout->addWidget(display, 0, 0, 1, 4);
```

This looks complicated, but it's mostly geometry.

The format is:

```cpp
addWidget(widget, row, column, rowSpan, columnSpan)
```

So:

```cpp
layout->addWidget(display, 0, 0, 1, 4);
```

means:

```text
widget = display
row = 0
column = 0
rowSpan = 1
columnSpan = 4
```

So the display occupies:

```text
row 0
columns 0,1,2,3
```

Visualized:

```text
+-----------------------------+
|          DISPLAY            |
|      spans 4 columns        |
+-----------------------------+
```

This leaves four columns for the buttons.

---

# 24. The button labels array

Now:

```cpp
const char* buttons[16] = {
    "7", "8", "9", "/",
    "4", "5", "6", "*",
    "1", "2", "3", "-",
    "0", "C", "=", "+"
};
```

This introduces **arrays**.

An array is a group of values stored under one variable name.

Here:

```cpp
buttons
```

contains 16 strings.

Imagine numbered boxes:

```text
index       value

 0          "7"
 1          "8"
 2          "9"
 3          "/"

 4          "4"
 5          "5"
 6          "6"
 7          "*"

 8          "1"
 9          "2"
10          "3"
11          "-"

12          "0"
13          "C"
14          "="
15          "+"
```

Remember that C++ arrays start at **index 0**.

---

# 25. What does `const char*` mean?

This is one of the more advanced-looking pieces:

```cpp
const char* buttons[16]
```

Let's simplify it.

`"7"` is a string literal.

In traditional C/C++ terminology, these are character arrays.

The important beginner-level idea is:

```cpp
const char* buttons[16]
```

means:

> `buttons` is an array containing 16 pointers to constant character strings.

You don't need to memorize the exact technical definition yet.

For this program, mentally treat it as:

```text
buttons = 16 text labels
```

---

# 26. `int pos = 0`

Next:

```cpp
int pos = 0;
```

This is an integer variable.

It will track which button label we're currently using.

Imagine:

```text
pos = 0
```

means:

> Start with `"7"`.

Then:

```text
pos = 1 → "8"
pos = 2 → "9"
pos = 3 → "/"
...
```

---

# 27. The nested loops

Now we reach:

```cpp
for (int row = 1; row <= 4; ++row) {
    for (int col = 0; col < 4; ++col) {
```

This creates the button grid.

There are:

```text
4 rows
4 columns
```

giving:

```text
4 × 4 = 16 buttons
```

Exactly the number of labels in:

```cpp
buttons[16]
```

That's an important connection.

---

# 28. Understanding the first `for`

```cpp
for (int row = 1; row <= 4; ++row)
```

You can read it as:

> Start row at 1, continue while row is <= 4, and increase row by 1 each time.

So:

```text
row = 1
row = 2
row = 3
row = 4
```

Then stop.

Why does it begin at row 1 instead of 0?

Because row 0 is already used by the display.

So:

```text
row 0 → display
row 1 → 7 8 9 /
row 2 → 4 5 6 *
row 3 → 1 2 3 -
row 4 → 0 C = +
```

---

# 29. Understanding the inner loop

```cpp
for (int col = 0; col < 4; ++col)
```

This gives:

```text
col = 0
col = 1
col = 2
col = 3
```

So each row has four buttons.

---

# 30. How the loops work together

This is worth understanding carefully.

The outer loop chooses the row.

The inner loop fills all columns in that row.

So the sequence is:

```text
row 1:
    col 0
    col 1
    col 2
    col 3

row 2:
    col 0
    col 1
    col 2
    col 3

row 3:
    col 0
    col 1
    col 2
    col 3

row 4:
    col 0
    col 1
    col 2
    col 3
```

That's 16 positions.

---

# 31. Creating a button

Inside the loop:

```cpp
QPushButton *btn = new QPushButton(buttons[pos], this);
```

This creates a new button.

Remember:

```cpp
buttons[pos]
```

gets the current label.

So when:

```text
pos = 0
```

this becomes effectively:

```cpp
new QPushButton("7", this)
```

Then:

```text
pos = 1
```

becomes:

```cpp
new QPushButton("8", this)
```

and so on.

---

# 32. The button size

```cpp
btn->setMinimumSize(60, 50);
```

Every button gets a minimum size of:

```text
60 wide
50 high
```

Again, this is GUI appearance/layout, not calculator logic.

---

# 33. Button font size

```cpp
btn->setStyleSheet("font-size: 18px;");
```

Makes button text larger.

So the buttons visually contain:

```text
7
8
9
/
```

rather than tiny text.

---

# 34. Putting the button into the grid

```cpp
layout->addWidget(btn, row, col);
```

This tells the grid:

> Put this button at this row and column.

For example:

```text
row=1, col=0 → 7
row=1, col=1 → 8
row=1, col=2 → 9
row=1, col=3 → /

row=2, col=0 → 4
row=2, col=1 → 5
row=2, col=2 → 6
row=2, col=3 → *
```

Because the loops generate those coordinates.

---

# 35. The most important Qt concept: `connect`

Now this line:

```cpp
connect(btn, &QPushButton::clicked, this, &Calculator::onButtonClicked);
```

is one of the most important lines in the whole program.

This is how the calculator reacts to clicks.

Qt uses a **signal and slot** system.

Think of it like this:

```text
Button gets clicked
        ↓
   signal emitted
        ↓
onButtonClicked() runs
```

The button produces the signal:

```cpp
QPushButton::clicked
```

Your calculator provides the function:

```cpp
Calculator::onButtonClicked
```

And `connect()` links them.

---

# 36. The four pieces of `connect`

Let's examine:

```cpp
connect(
    btn,
    &QPushButton::clicked,
    this,
    &Calculator::onButtonClicked
);
```

### First argument

```cpp
btn
```

The object sending the signal.

The button.

### Second argument

```cpp
&QPushButton::clicked
```

The signal.

It means:

> when this button is clicked.

### Third argument

```cpp
this
```

The object receiving the event.

That's the current `Calculator`.

### Fourth argument

```cpp
&Calculator::onButtonClicked
```

The function to call when the signal occurs.

So:

```text
btn
 ↓
clicked signal
 ↓
Calculator
 ↓
onButtonClicked()
```

---

# 37. Why does every button use the same function?

This is clever.

Instead of making:

```cpp
on7Clicked()
on8Clicked()
on9Clicked()
onPlusClicked()
...
```

we use only one:

```cpp
onButtonClicked()
```

Then inside that function, we determine:

> Which button did the user click?

This is where:

```cpp
sender()
```

comes in.

---

# 38. `++pos`

After each button:

```cpp
++pos;
```

This means:

```cpp
pos = pos + 1;
```

So the labels progress:

```text
7 → 8 → 9 → / → 4 → 5 → ...
```

After all 16 buttons are created:

```text
pos = 16
```

But that's okay because the loop is finished.

---

# 39. Setting the layout

After creating the buttons:

```cpp
setLayout(layout);
```

This tells the calculator window:

> "Use this grid layout to arrange your widgets."

Without this, you've created the layout, but haven't properly attached it to the calculator's layout system.

Conceptually:

```text
Calculator
    │
    └── GridLayout
          ├── Display
          ├── Button 7
          ├── Button 8
          ├── Button 9
          └── ...
```

---

# 40. Calculator state variables

Now:

```cpp
currentValue = 0.0;
pendingOperator = "";
waitingForOperand = true;
```

These three variables are the calculator's **memory/state**.

They are extremely important.

Let's understand each one.

---

# 41. `currentValue`

Declared later as:

```cpp
double currentValue;
```

A `double` is a floating-point number.

That means it can store values such as:

```text
5
5.2
3.14159
-17.8
```

The calculator uses this variable to remember the first/current calculation result.

For example after:

```text
7 +
```

we have:

```cpp
currentValue = 7;
```

---

# 42. `pendingOperator`

```cpp
QString pendingOperator;
```

This stores the operation we're waiting to perform.

It might contain:

```text
"+"
"-"
"*"
"/"
```

Or:

```text
""
```

when there is no pending operator.

For example:

```text
7 +
```

produces:

```cpp
currentValue = 7;
pendingOperator = "+";
```

---

# 43. `waitingForOperand`

```cpp
bool waitingForOperand;
```

A `bool` can only be:

```text
true
false
```

This variable answers a very important question:

> "Should the next digit replace the current display, or be appended to it?"

For example:

Suppose display contains:

```text
7
```

Then you press:

```text
+
```

We set:

```cpp
waitingForOperand = true;
```

Then you press `5`.

Because we're waiting for a new operand, we want:

```text
5
```

not:

```text
75
```

That's exactly what this variable controls.

---

# 44. The three state variables working together

Suppose user presses:

```text
7 + 5 =
```

After `7`:

```text
currentValue = 0
pendingOperator = ""
waitingForOperand = false
display = "7"
```

After `+`:

```text
currentValue = 7
pendingOperator = "+"
waitingForOperand = true
display = "7"
```

After `5`:

```text
currentValue = 7
pendingOperator = "+"
waitingForOperand = false
display = "5"
```

After `=`:

```cpp
calculate();
```

does:

```text
7 + 5
```

and produces:

```text
12
```

Then state becomes approximately:

```text
currentValue = 12
pendingOperator = ""
waitingForOperand = true
display = "12"
```

That is the core mechanism of the calculator.

---

# 45. Moving to `private slots:`

After the constructor:

```cpp
private slots:
```

This is Qt-specific.

The following function is a **slot**:

```cpp
void onButtonClicked()
```

A slot is basically a function that can respond to Qt signals.

Remember:

```cpp
connect(...)
```

created the relationship between:

```text
clicked
   ↓
onButtonClicked()
```

---

# 46. `void onButtonClicked()`

```cpp
void onButtonClicked() {
```

Let's break it down.

### `void`

The function returns nothing.

For example:

```cpp
int add()
```

might return a number.

But:

```cpp
void onButtonClicked()
```

doesn't return anything.

It simply performs actions.

---

# 47. Finding out which button was clicked

First line:

```cpp
QPushButton *btn = qobject_cast<QPushButton*>(sender());
```

This is probably the most confusing line for a beginner.

Let's split it.

---

## 47.1 `sender()`

Because Qt called this function in response to a signal, Qt can tell us:

> "Which object sent the signal?"

That is:

```cpp
sender()
```

For example, if the user clicked:

```text
7
```

then:

```cpp
sender()
```

refers to the `7` button.

If the user clicked:

```text
+
```

then `sender()` refers to the `+` button.

---

# 48. Why `qobject_cast`?

`sender()` returns a generic Qt object pointer.

We know that all our signals came from:

```cpp
QPushButton
```

So we convert that generic object into a `QPushButton*`:

```cpp
qobject_cast<QPushButton*>(sender())
```

The purpose is essentially:

> "Treat the object that sent the signal as a QPushButton."

Now:

```cpp
btn
```

points to the actual clicked button.

---

# 49. Safety check

Next:

```cpp
if (!btn) return;
```

The `!` means "not".

So:

```cpp
!btn
```

means:

> "btn is null?"

If the conversion failed, `btn` could be null.

Then:

```cpp
return;
```

means:

> exit the function immediately.

So this prevents the program from trying to use an invalid pointer.

---

# 50. Getting the button's text

```cpp
QString text = btn->text();
```

Every button has text.

For example:

```text
button "7" → text() returns "7"
button "+" → text() returns "+"
button "=" → text() returns "="
button "C" → text() returns "C"
```

So now:

```cpp
text
```

contains whatever the clicked button says.

This is how one function can handle every button.

---

# 51. First decision: was it a number?

```cpp
if (text >= "0" && text <= "9") {
```

The program asks:

> Is the clicked button a digit from 0 to 9?

The `&&` means **AND**.

So:

```cpp
text >= "0" && text <= "9"
```

means both conditions must be true.

For example:

```text
"5"
```

passes.

But:

```text
"+"
```

doesn't.

---

# 52. What happens when a number is clicked?

Inside:

```cpp
if (waitingForOperand) {
    display->setText(text);
    waitingForOperand = false;
}
else {
    display->setText(display->text() + text);
}
```

This is one of the most important pieces.

There are two situations.

---

# 53. Situation A: waiting for a new number

Suppose the calculator currently displays:

```text
7
```

and you just pressed:

```text
+
```

Then:

```cpp
waitingForOperand = true;
```

Now press:

```text
5
```

The program enters:

```cpp
if (waitingForOperand)
```

So:

```cpp
display->setText(text);
```

changes:

```text
7
```

into:

```text
5
```

Then:

```cpp
waitingForOperand = false;
```

Now it is no longer waiting for the first digit of a new operand.

---

# 54. Situation B: continue building the number

Suppose display currently says:

```text
5
```

and you press:

```text
3
```

Now:

```text
waitingForOperand = false
```

So the `else` runs:

```cpp
display->setText(display->text() + text);
```

Suppose:

```cpp
display->text()
```

returns:

```text
"5"
```

and:

```cpp
text
```

is:

```text
"3"
```

Then:

```text
"5" + "3"
```

becomes:

```text
"53"
```

So the display becomes:

```text
53
```

This is how you can enter:

```text
123456
```

one button at a time.

---

# 55. Notice that the display stores TEXT

This is an important concept.

The display doesn't initially store:

```text
123
```

as a `double`.

It stores:

```text
"123"
```

as a `QString`.

Later, when the calculator needs to calculate, it converts that text into a number.

That happens here:

```cpp
.toDouble()
```

We'll see it shortly.

So we have:

```text
User clicks
   ↓
button text
   ↓
QString display
   ↓
convert to double
   ↓
perform mathematics
```

---

# 56. The `C` button

Next:

```cpp
else if (text == "C") {
```

This asks:

> Did the user press C?

If yes:

```cpp
display->setText("0");
currentValue = 0.0;
pendingOperator = "";
waitingForOperand = true;
```

This completely resets the calculator.

---

# 57. Why reset all three variables?

Imagine the calculator was in this state:

```text
display = "25"
currentValue = 10
pendingOperator = "+"
waitingForOperand = false
```

If you only changed the display to `0`, the internal state would still remember:

```text
10 + ...
```

That would be wrong.

So `C` resets **both visible state and hidden state**:

```text
Display:
"0"

Memory:
currentValue = 0
pendingOperator = ""
waitingForOperand = true
```

This is a very important programming principle:

> When resetting an object, reset all variables that define its current state.

---

# 58. The `=` button

Next:

```cpp
else if (text == "=") {
    calculate();
    pendingOperator = "";
    waitingForOperand = true;
}
```

When `=` is clicked:

```cpp
calculate();
```

runs the actual arithmetic.

Then:

```cpp
pendingOperator = "";
```

clears the operator.

And:

```cpp
waitingForOperand = true;
```

says that the next number should begin a new operand.

---

# 59. What is `calculate()`?

Later in the code we have:

```cpp
void calculate() {
```

This is a helper function.

The author separated the arithmetic into its own function rather than stuffing everything into `onButtonClicked()`.

That's good organization.

The roles are:

```text
onButtonClicked()
    ↓
figures out WHAT button was pressed

calculate()
    ↓
figures out HOW to perform the arithmetic
```

That's a very important separation.

---

# 60. What happens when an operator is pressed?

Finally:

```cpp
else { // operator
```

At this point, we've already checked:

```text
number?
C?
=?
```

If none of those are true, then the button must be:

```text
+
-
*
/
```

So the program treats it as an operator.

---

# 61. Checking whether an operator already exists

```cpp
if (!pendingOperator.isEmpty()) {
    calculate();
}
else {
    currentValue = display->text().toDouble();
}
```

This is subtle.

The program asks:

> "Do I already have an operator waiting?"

`isEmpty()` checks whether the QString contains nothing.

For example:

```text
pendingOperator = ""
```

means empty.

But:

```text
pendingOperator = "+"
```

isn't empty.

So:

```cpp
!pendingOperator.isEmpty()
```

means:

> There is already an operator.

---

# 62. First operator

Suppose we type:

```text
7 +
```

When `+` is pressed:

```cpp
pendingOperator
```

is empty.

So the `else` executes:

```cpp
currentValue = display->text().toDouble();
```

The display currently contains:

```text
"7"
```

Then:

```cpp
.toDouble()
```

converts it:

```text
"7" → 7.0
```

So:

```cpp
currentValue = 7.0;
```

Then:

```cpp
pendingOperator = text;
```

So:

```text
pendingOperator = "+"
```

Finally:

```cpp
waitingForOperand = true;
```

So the calculator is now waiting for the second number.

---

# 63. Why is this called `pendingOperator`?

Because the operator hasn't been fully executed yet.

When you press:

```text
7 +
```

you haven't finished the operation.

You only said:

> I want to add the next number to 7.

So the operator is "pending".

Hence:

```cpp
pendingOperator = "+";
```

---

# 64. A more interesting example: `7 + 5 *`

Now let's see why this code checks for an existing operator.

Sequence:

```text
7
+
5
*
```

After `7 +`:

```text
currentValue = 7
pendingOperator = "+"
```

Then 5 is entered:

```text
display = 5
```

Then `*` is pressed.

Now:

```cpp
pendingOperator
```

is already `"+"`.

Therefore:

```cpp
calculate();
```

runs.

The calculator does:

```text
7 + 5 = 12
```

Then stores:

```text
currentValue = 12
```

After that:

```cpp
pendingOperator = "*";
```

So the new state is:

```text
currentValue = 12
pendingOperator = "*"
waitingForOperand = true
```

Then if you type `2` and `=`:

```text
12 * 2 = 24
```

This is why the calculator evaluates operations when a new operator is entered.

---

# 65. Now let's study `calculate()` deeply

The function begins:

```cpp
void calculate() {
```

Inside:

```cpp
double operand = display->text().toDouble();
double result = currentValue;
```

Two variables are created.

---

# 66. What is an operand?

In mathematics:

```text
7 + 5
```

the values `7` and `5` are operands.

The operator is:

```text
+
```

So:

```text
7    +    5
↑         ↑
operand   operand
```

The current display is treated as the current operand:

```cpp
double operand = display->text().toDouble();
```

---

# 67. Why `toDouble()`?

The display is text:

```text
"5"
```

Mathematics requires a number:

```text
5.0
```

So:

```cpp
display->text()
```

gives:

```text
"5"
```

and:

```cpp
.toDouble()
```

converts it to:

```text
5.0
```

So:

```cpp
double operand = ...
```

gets an actual number.

---

# 68. Initializing `result`

```cpp
double result = currentValue;
```

Initially, result is just a copy of `currentValue`.

For example:

```text
currentValue = 7
result = 7
```

Then the operator determines how `result` changes.

---

# 69. Addition

```cpp
if (pendingOperator == "+")
    result = currentValue + operand;
```

Suppose:

```text
currentValue = 7
operand = 5
pendingOperator = "+"
```

Then:

```cpp
result = 7 + 5;
```

So:

```text
result = 12
```

---

# 70. Subtraction

```cpp
else if (pendingOperator == "-")
    result = currentValue - operand;
```

Example:

```text
currentValue = 10
operand = 3
```

becomes:

```text
result = 7
```

---

# 71. Multiplication

```cpp
else if (pendingOperator == "*")
    result = currentValue * operand;
```

Example:

```text
4 × 6
```

becomes:

```text
24
```

---

# 72. Division

```cpp
else if (pendingOperator == "/") {
```

The code then performs an important safety check:

```cpp
if (operand == 0.0) {
```

This asks:

> Are we dividing by zero?

---

# 73. Why division by zero is special

Mathematically:

```text
10 / 0
```

is undefined.

So the calculator doesn't try to perform it.

Instead:

```cpp
display->setText("Error");
waitingForOperand = true;
pendingOperator = "";
return;
```

---

# 74. What exactly happens during division-by-zero?

Suppose you enter:

```text
10 / 0 =
```

When `calculate()` sees:

```cpp
operand == 0.0
```

it changes the display to:

```text
Error
```

Then:

```cpp
waitingForOperand = true;
```

So the calculator waits for fresh input.

Then:

```cpp
pendingOperator = "";
```

clears the operator.

Then:

```cpp
return;
```

immediately exits `calculate()`.

It does **not** continue to:

```cpp
result = currentValue / operand;
```

which would be invalid.

---

# 75. The normal division case

If the operand isn't zero:

```cpp
result = currentValue / operand;
```

For example:

```text
10 / 2
```

becomes:

```text
5
```

---

# 76. Updating the display

After calculation:

```cpp
display->setText(QString::number(result, 'g', 12));
```

This converts the numerical result back into text.

Remember our earlier pipeline:

```text
TEXT
 ↓
toDouble()
 ↓
NUMBER
 ↓
calculation
 ↓
NUMBER
 ↓
QString::number()
 ↓
TEXT
```

This is a very important connection.

---

# 77. `QString::number()`

Suppose:

```cpp
result = 12;
```

We need the display to show:

```text
"12"
```

But `result` is a `double`.

So:

```cpp
QString::number(...)
```

converts the number into a `QString`.

---

# 78. What does `'g', 12` mean?

This:

```cpp
QString::number(result, 'g', 12)
```

specifies how to format the number.

The `'g'` means general number formatting.

The `12` specifies the number of significant digits/precision used for the formatting.

For example, a result such as:

```text
3.14159265358979
```

may be displayed with controlled precision rather than an excessively long string.

This is mainly about **presentation**.

---

# 79. Updating `currentValue`

Finally:

```cpp
currentValue = result;
```

This is crucial.

Suppose:

```text
7 + 5
```

produced:

```text
12
```

Now the calculator remembers:

```text
currentValue = 12
```

So if the user then presses:

```text
*
2
=
```

the calculator can do:

```text
12 * 2 = 24
```

instead of forgetting the previous answer.

---

# 80. The three class variables

At the bottom:

```cpp
QLineEdit *display;
double currentValue;
QString pendingOperator;
bool waitingForOperand;
```

These are **member variables**.

They belong to every `Calculator` object.

Let's visualize the calculator object in memory conceptually:

```text
Calculator object
──────────────────────────────
display
   ↓
QLineEdit

currentValue
   = 12

pendingOperator
   = "*"

waitingForOperand
   = true
──────────────────────────────
```

These variables persist between button clicks.

That's extremely important.

---

# 81. Why do these variables need to be members?

Imagine we wrote:

```cpp
void onButtonClicked() {
    double currentValue;
}
```

Then `currentValue` would be local to that function.

When the function ends, that local variable is no longer available to the next button click in the same way we need.

But our calculator needs information to survive:

```text
click 7
   ↓
function ends

click +
   ↓
function runs again
```

The program needs to remember the `7`.

That's why:

```cpp
double currentValue;
```

is a **class member**, not just a temporary local variable.

---

# 82. The entire class structure

Now we can see the structure:

```text
Calculator
│
├── constructor
│   ├── create display
│   ├── create layout
│   ├── create buttons
│   ├── connect buttons
│   └── initialize state
│
├── onButtonClicked()
│   ├── find clicked button
│   ├── get its text
│   ├── process numbers
│   ├── process C
│   ├── process =
│   └── process operators
│
├── calculate()
│   ├── read operand
│   ├── inspect operator
│   ├── calculate result
│   ├── handle division by zero
│   └── update display/state
│
└── member variables
    ├── display
    ├── currentValue
    ├── pendingOperator
    └── waitingForOperand
```

That is essentially the entire architecture.

---

# 83. `#include "calculator.moc"`

At the end:

```cpp
#include "calculator.moc"
```

The comment says:

```cpp
// needed when using Q_OBJECT in a single .cpp file
```

This relates to Qt's **Meta-Object Compiler**, commonly called `moc`.

Remember:

```cpp
Q_OBJECT
```

tells Qt:

> This class participates in Qt's meta-object system.

Qt's build tools need generated code to support things such as its signal/slot mechanisms.

Including:

```cpp
calculator.moc
```

in a single-file implementation is one way to make the generated Qt meta-object code available.

You don't need to understand the generated code itself right now.

For a beginner, the practical relationship is:

```text
Q_OBJECT
   ↓
Qt needs meta-object machinery
   ↓
moc generates supporting code
   ↓
calculator.moc
```

---

# 84. Now the `main()` function

Finally:

```cpp
int main(int argc, char *argv[]) {
```

Every normal C++ executable starts execution at:

```cpp
main()
```

Think of it as:

```text
PROGRAM START
     ↓
main()
```

---

# 85. What are `argc` and `argv`?

These are command-line arguments.

### `argc`

Argument count.

### `argv`

Argument values.

For example, if a program were launched as:

```text
calculator.exe test hello
```

the program could access those arguments through `argv`.

For this GUI program, Qt uses them to initialize the application properly.

You don't need them to understand the calculator logic yet.

---

# 86. Creating the Qt application

Inside `main()`:

```cpp
QApplication app(argc, argv);
```

This creates the Qt application object.

It is responsible for managing the GUI application's environment and event processing.

Think of it as the **engine that keeps the graphical application alive and responsive**.

---

# 87. Creating the calculator

Next:

```cpp
Calculator calc;
```

This creates an actual Calculator object.

Remember:

```cpp
class Calculator
```

was the blueprint.

Now:

```cpp
Calculator calc;
```

creates a concrete object from that blueprint.

And when it's created:

```text
Calculator constructor runs
```

So all the constructor code executes:

```text
set title
set size
create display
create layout
create buttons
connect buttons
initialize variables
```

---

# 88. Showing the window

Next:

```cpp
calc.show();
```

This tells Qt:

> Show the calculator window.

Without this, the object exists, but the window wouldn't be shown normally.

---

# 89. The event loop

The last line:

```cpp
return app.exec();
```

is extremely important for GUI programs.

`exec()` starts Qt's **event loop**.

This is what allows the application to continually wait for things like:

```text
mouse click
keyboard input
window events
redrawing
etc.
```

Think of it as:

```text
while application is running:

    wait for event

    if button clicked:
        process click

    if window needs repaint:
        repaint

    if application closes:
        stop
```

Conceptually, that's what an event loop does.

---

# 90. Why is the event loop necessary?

Without it, the program would basically do:

```text
create application
create calculator
show calculator
program immediately ends
```

The window wouldn't remain interactive.

`app.exec()` keeps the program alive and listening for events.

---

# 91. The complete program flow

Let's now connect **everything**.

When you launch the program:

```text
main()
  ↓
QApplication app
  ↓
Calculator calc
  ↓
Calculator constructor
  ↓
create display
  ↓
create layout
  ↓
create 16 buttons
  ↓
connect each button's clicked signal
  ↓
initialize calculator state
  ↓
calc.show()
  ↓
app.exec()
  ↓
EVENT LOOP
```

Now the program waits.

---

# 92. What happens when you click `7`?

Let's trace the real execution.

You click:

```text
7
```

Qt detects the click.

The button emits:

```text
clicked
```

The connection created earlier catches it:

```cpp
connect(
    btn,
    &QPushButton::clicked,
    this,
    &Calculator::onButtonClicked
);
```

Therefore Qt calls:

```cpp
onButtonClicked()
```

---

# 93. Inside `onButtonClicked()` after clicking `7`

First:

```cpp
sender()
```

identifies the button.

Then:

```cpp
btn->text()
```

gives:

```text
"7"
```

Then:

```cpp
if (text >= "0" && text <= "9")
```

is true.

Suppose:

```text
waitingForOperand = true
```

So:

```cpp
display->setText("7");
```

and:

```cpp
waitingForOperand = false;
```

Now:

```text
display = "7"
```

---

# 94. Then you click `5`

Suppose you haven't pressed an operator.

The display already contains:

```text
7
```

and:

```text
waitingForOperand = false
```

Clicking `5` gives:

```text
text = "5"
```

Because we're not waiting for a new operand:

```cpp
display->setText(display->text() + text);
```

So:

```text
"7" + "5"
```

becomes:

```text
"75"
```

The display shows:

```text
75
```

---

# 95. Then you click `+`

Now `text` is:

```text
"+"
```

This isn't a digit.

Not C.

Not `=`.

So the `else` operator section runs.

Since there is no pending operator:

```cpp
currentValue = display->text().toDouble();
```

which means:

```text
"75" → 75
```

So:

```text
currentValue = 75
```

Then:

```cpp
pendingOperator = "+"
```

And:

```cpp
waitingForOperand = true
```

State:

```text
display = "75"
currentValue = 75
pendingOperator = "+"
waitingForOperand = true
```

---

# 96. Then you click `2`

`text` is:

```text
"2"
```

Because:

```text
waitingForOperand = true
```

the display is replaced:

```text
"75" → "2"
```

and:

```cpp
waitingForOperand = false;
```

Now state:

```text
display = "2"
currentValue = 75
pendingOperator = "+"
waitingForOperand = false
```

---

# 97. Then you click `=`

`text` is:

```text
"="
```

So:

```cpp
calculate();
```

runs.

Inside:

```cpp
double operand = display->text().toDouble();
```

gives:

```text
operand = 2
```

And:

```text
currentValue = 75
pendingOperator = "+"
```

So:

```cpp
result = currentValue + operand;
```

becomes:

```text
result = 75 + 2
```

therefore:

```text
result = 77
```

Then:

```cpp
display->setText(QString::number(result, 'g', 12));
```

changes:

```text
display = "77"
```

And:

```cpp
currentValue = result;
```

means:

```text
currentValue = 77
```

Then back in `onButtonClicked()`:

```cpp
pendingOperator = "";
waitingForOperand = true;
```

So the final state is:

```text
display = "77"
currentValue = 77
pendingOperator = ""
waitingForOperand = true
```

---

# 98. The most important mental model

You can understand almost the entire calculator using these four things:

```text
DISPLAY
   ↓
what the user currently sees

CURRENT VALUE
   ↓
number the calculator remembers

PENDING OPERATOR
   ↓
what operation it plans to perform

WAITING FOR OPERAND
   ↓
whether the next digit starts a new number
```

Think of the internal state as:

```text
┌────────────────────────────────────┐
│         CALCULATOR STATE            │
├────────────────────────────────────┤
│ display            "5"              │
│ currentValue       7                │
│ pendingOperator    "+"              │
│ waitingForOperand  false            │
└────────────────────────────────────┘
```

Those values change every time you click something.

---

# 99. Why the calculator can be described as a state machine

This program is actually a simple **state machine**.

For example, it can conceptually be in states like:

```text
START
  ↓
ENTERING NUMBER
  ↓
OPERATOR PRESSED
  ↓
WAITING FOR SECOND NUMBER
  ↓
ENTERING SECOND NUMBER
  ↓
CALCULATING
  ↓
RESULT
```

The variable:

```cpp
waitingForOperand
```

helps control these states.

And:

```cpp
pendingOperator
```

helps remember the transition/action.

This is a very useful programming concept that appears everywhere in software.

---

# 100. The code separated into responsibilities

A useful way to understand the architecture is to divide the code into **five sections**.

## Part 1 — Imports

```cpp
#include <QApplication>
#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QString>
```

Purpose:

```text
Bring Qt functionality into the program.
```

## Part 2 — Calculator class

```cpp
class Calculator : public QWidget {
```

Purpose:

```text
Define what a Calculator object is.
```

## Part 3 — Constructor / GUI creation

```cpp
Calculator(...) : QWidget(parent) {
    ...
}
```

Purpose:

```text
Build the calculator interface
and initialize its state.
```

## Part 4 — Event handling and calculation

```cpp
void onButtonClicked()
```

Purpose:

```text
React to user input.
```

and:

```cpp
void calculate()
```

Purpose:

```text
Perform arithmetic.
```

## Part 5 — `main()`

```cpp
int main(...)
```

Purpose:

```text
Start the application,
create the calculator,
show it,
and run the event loop.
```

---

# 101. The simplified architecture

Strip away Qt details and the program conceptually becomes:

```text
START PROGRAM
     ↓
CREATE CALCULATOR
     ↓
BUILD USER INTERFACE
     ↓
WAIT FOR USER
     ↓
USER CLICKS BUTTON
     ↓
WHICH BUTTON?
     │
     ├── number → update display
     │
     ├── C → reset everything
     │
     ├── operator → remember operator
     │
     └── = → calculate result
     ↓
UPDATE STATE
     ↓
WAIT FOR NEXT CLICK
```

And that cycle repeats until the user closes the window.

---

# 102. One thing to especially understand

The hardest part for a beginner is usually thinking that:

```cpp
onButtonClicked()
```

runs once and the entire calculation happens there.

It doesn't.

The program is **event-driven**.

The function is called over and over again at different moments.

For example:

```text
User clicks 7
    ↓
onButtonClicked()
    ↓
returns

User clicks +
    ↓
onButtonClicked()
    ↓
returns

User clicks 5
    ↓
onButtonClicked()
    ↓
returns

User clicks =
    ↓
onButtonClicked()
    ↓
calculate()
    ↓
returns
```

The variables:

```cpp
currentValue
pendingOperator
waitingForOperand
```

are what allow information to survive **between those separate function calls**.

That is one of the fundamental ideas of interactive programming.

---

# 103. A real-world analogy

Imagine a cashier taking an order.

They keep a little note:

```text
Customer order:
Current total = €7
Pending action = ADD
Waiting for next item = YES
```

Then the customer says:

```text
€5
```

The cashier updates:

```text
Current total = €12
```

The calculator does the same thing.

Its "note" is:

```cpp
currentValue
pendingOperator
waitingForOperand
```

The GUI is just the way the customer communicates with the cashier.

---

# 104. The three most important C++ concepts this example teaches

This little program teaches several major ideas.

## Objects and classes

```cpp
class Calculator
```

creates a blueprint.

Then:

```cpp
Calculator calc;
```

creates an object.

## Functions

```cpp
onButtonClicked()
calculate()
```

are blocks of reusable behavior.

## Variables/state

```cpp
currentValue
pendingOperator
waitingForOperand
```

store information that changes while the program runs.

---

# 105. The three most important Qt concepts

And it teaches three major Qt ideas.

## Widgets

```cpp
QPushButton
QLineEdit
QWidget
```

are graphical components.

## Layouts

```cpp
QGridLayout
```

organizes those components.

## Signals and slots

```cpp
connect(...)
```

allows GUI events to call your code.

This relationship is fundamental:

```text
Widget
  ↓
Signal
  ↓
Slot
  ↓
Your code
```

---

# 106. The entire calculator in one diagram

Here is the full mental model to remember:

```text
                         ┌─────────────────────┐
                         │     QApplication    │
                         │     Event Loop      │
                         └──────────┬──────────┘
                                    │
                                    │ events
                                    ▼
                         ┌─────────────────────┐
                         │     Calculator      │
                         │      QWidget        │
                         └──────────┬──────────┘
                                    │
                ┌───────────────────┼───────────────────┐
                │                   │                   │
                ▼                   ▼                   ▼
          ┌──────────┐        ┌───────────┐      ┌───────────┐
          │ Display  │        │  Buttons  │      │  Layout   │
          │QLineEdit │        │QPushButton│      │QGridLayout│
          └────┬─────┘        └─────┬─────┘      └───────────┘
               │                    │
               │                    │ clicked
               │                    ▼
               │             onButtonClicked()
               │                    │
               │        ┌───────────┼────────────┐
               │        │           │            │
               │        ▼           ▼            ▼
               │      number        C          operator / =
               │        │           │            │
               │        └───────────┴────────────┘
               │                    │
               │                    ▼
               │               calculate()
               │                    │
               │                    ▼
               │             currentValue
               │             pendingOperator
               │             waitingForOperand
               │                    │
               └────────────────────┘
```

---

# 107. If you are learning C++, learn these pieces in this order

This particular code contains quite a lot of concepts at once. Don't try to memorize everything simultaneously.

A sensible learning order from this code is:

```text
1. Variables
   int
   double
   bool

2. Strings
   QString

3. if / else

4. for loops

5. arrays

6. functions

7. pointers
   *

8. classes and objects

9. constructors

10. inheritance

11. Qt widgets

12. signals and slots

13. event loops
```

Once those concepts become familiar, this entire program becomes much less intimidating.

The most important transition is from seeing the code as **hundreds of unfamiliar symbols** to seeing it as a few cooperating systems:

```text
C++ language
      +
Qt GUI
      +
Calculator state
      +
Event handling
      ↓
Complete application
```

The calculator isn't actually doing anything magical. It is simply **maintaining state, responding to events, and updating the interface**.

---

# 108. Important limitation of this calculator

Now that we understand the code, we can also see what it **doesn't** do.

It doesn't support:

```text
decimal point
negative numbers
parentheses
scientific functions
backspace
keyboard input
operator precedence
```

For example:

```text
2 + 3 * 4
```

A normal mathematical calculator might interpret this according to precedence as:

```text
2 + (3 * 4)
= 14
```

But this calculator processes an already pending operator when another operator is entered, so it behaves more like a simple sequential calculator.

That behavior comes directly from:

```cpp
if (!pendingOperator.isEmpty()) {
    calculate();
}
```

This illustrates an important lesson:

> The program behaves exactly according to the logic you give it, not according to what we might intuitively expect a calculator to do.

---

# 109. Final mental model

The whole program can be reduced to one simple idea:

```text
                     USER
                      │
                      │ clicks
                      ▼
               ┌─────────────┐
               │   BUTTON    │
               └──────┬──────┘
                      │
                   signal
                      │
                      ▼
             ┌──────────────────┐
             │ onButtonClicked()│
             └────────┬─────────┘
                      │
              identifies action
                      │
       ┌──────────────┼──────────────┐
       │              │              │
       ▼              ▼              ▼
    number            C          operator / =
       │              │              │
       ▼              ▼              ▼
   update text      reset       update state
                                     │
                                     ▼
                                calculate()
                                     │
                                     ▼
                              update display
                                     │
                                     ▼
                                save result
                                     │
                                     ▼
                              wait for next click
```

And underneath all of that, the calculator remembers:

```text
currentValue
pendingOperator
waitingForOperand
```

Those variables are what make the separate button clicks feel like one continuous calculation.

---

# 110. The original code, kept for reference

```cpp
#include <QApplication>
#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QString>

class Calculator : public QWidget {
    Q_OBJECT

public:
    Calculator(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("Simple Calculator");
        setFixedSize(300, 400);

        display = new QLineEdit(this);
        display->setReadOnly(true);
        display->setAlignment(Qt::AlignRight);
        display->setText("0");
        display->setStyleSheet("font-size: 24px; padding: 10px;");

        QGridLayout *layout = new QGridLayout(this);
        layout->addWidget(display, 0, 0, 1, 4);

        // Button labels
        const char* buttons[16] = {
            "7", "8", "9", "/",
            "4", "5", "6", "*",
            "1", "2", "3", "-",
            "0", "C", "=", "+"
        };

        int pos = 0;
        for (int row = 1; row <= 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                QPushButton *btn = new QPushButton(buttons[pos], this);
                btn->setMinimumSize(60, 50);
                btn->setStyleSheet("font-size: 18px;");
                layout->addWidget(btn, row, col);

                connect(btn, &QPushButton::clicked, this, &Calculator::onButtonClicked);
                ++pos;
            }
        }

        setLayout(layout);
        currentValue = 0.0;
        pendingOperator = "";
        waitingForOperand = true;
    }

private slots:
    void onButtonClicked() {
        QPushButton *btn = qobject_cast<QPushButton*>(sender());
        if (!btn) return;

        QString text = btn->text();

        if (text >= "0" && text <= "9") {
            if (waitingForOperand) {
                display->setText(text);
                waitingForOperand = false;
            } else {
                display->setText(display->text() + text);
            }
        }
        else if (text == "C") {
            display->setText("0");
            currentValue = 0.0;
            pendingOperator = "";
            waitingForOperand = true;
        }
        else if (text == "=") {
            calculate();
            pendingOperator = "";
            waitingForOperand = true;
        }
        else { // operator
            if (!pendingOperator.isEmpty()) {
                calculate();
            } else {
                currentValue = display->text().toDouble();
            }
            pendingOperator = text;
            waitingForOperand = true;
        }
    }

private:
    void calculate() {
        double operand = display->text().toDouble();
        double result = currentValue;

        if (pendingOperator == "+")
            result = currentValue + operand;
        else if (pendingOperator == "-")
            result = currentValue - operand;
        else if (pendingOperator == "*")
            result = currentValue * operand;
        else if (pendingOperator == "/") {
            if (operand == 0.0) {
                display->setText("Error");
                waitingForOperand = true;
                pendingOperator = "";
                return;
            }
            result = currentValue / operand;
        }

        display->setText(QString::number(result, 'g', 12));
        currentValue = result;
    }

    QLineEdit *display;
    double currentValue;
    QString pendingOperator;
    bool waitingForOperand;
};

#include "calculator.moc"   // needed when using Q_OBJECT in a single .cpp file

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Calculator calc;
    calc.show();
    return app.exec();
}
```

---

# 111. Technical references

The Qt terminology and behavior discussed above were checked against Qt's official documentation, particularly its documentation for `QWidget` and the Qt signals-and-slots system.

- Qt `QWidget` documentation: <https://doc.qt.io/qtforpython-6/PySide6/QtWidgets/QWidget.html>
- Qt Signals & Slots overview: <https://doc.qt.io/qtforpython-6/overviews/qtcore-signalsandslots.html>
- Qt Signals and Slots tutorial: <https://doc.qt.io/qtforpython-6/tutorials/basictutorial/signals_and_slots.html>
- Qt `QObject` documentation: <https://doc.qt.io/qtforpython-6/PySide6/QtCore/QObject.html>

---

# Final takeaway

The best way to understand this calculator is not to memorize every line. Understand the relationships:

```text
C++
 │
 ├── class → defines Calculator
 ├── object → creates Calculator instance
 ├── variables → store state
 ├── functions → perform actions
 ├── pointers → refer to objects
 ├── loops → create repeated widgets
 └── if/else → choose behavior

Qt
 │
 ├── QWidget → base GUI object/window
 ├── QLineEdit → calculator display
 ├── QPushButton → calculator buttons
 ├── QGridLayout → arranges widgets
 ├── QApplication → application environment
 ├── signals/slots → react to GUI events
 └── event loop → keeps the application running

Calculator logic
 │
 ├── display
 ├── currentValue
 ├── pendingOperator
 └── waitingForOperand
```

Once you see those three layers working together, the code stops looking like a collection of random symbols and starts looking like a system with clear responsibilities.
