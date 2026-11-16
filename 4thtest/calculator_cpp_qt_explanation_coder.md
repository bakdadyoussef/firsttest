# Professional C++ / Qt Breakdown — Calculator Example

## Audience

This document is for a programmer who already understands general programming concepts—variables, functions, loops, objects, conditionals, references, and basic GUI/event-driven ideas—but is learning **C++ and Qt**.

The goal is not merely to explain what each line does. The goal is to explain:

- what each line means in C++ terms,
- what Qt API is being used,
- what object is being created or modified,
- how ownership and lifetime work,
- why the code is structured this way,
- how the pieces interact at runtime,
- what happens when a user clicks each kind of button,
- and what design limitations or simplifications exist in this implementation.

The code being analyzed is:

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

# 1. First understand the architecture

At a high level, the program has four layers of responsibility:

```text
┌────────────────────────────────────────────┐
│                main()                      │
│ creates QApplication + Calculator          │
└──────────────────────┬─────────────────────┘
                       │
                       ▼
┌────────────────────────────────────────────┐
│              QApplication                  │
│ owns the application's event loop          │
└──────────────────────┬─────────────────────┘
                       │ user events
                       ▼
┌────────────────────────────────────────────┐
│               Calculator                   │
│ QWidget subclass / application logic       │
│                                            │
│  ┌──────────────┐  ┌────────────────────┐  │
│  │ GUI widgets  │  │ calculator state   │  │
│  │ display      │  │ currentValue       │  │
│  │ buttons      │  │ pendingOperator    │  │
│  │ grid layout  │  │ waitingForOperand  │  │
│  └──────────────┘  └────────────────────┘  │
│                  │                         │
│                  ▼                         │
│          onButtonClicked()                 │
│                  │                         │
│                  ▼                         │
│             calculate()                   │
└────────────────────────────────────────────┘
```

The important distinction is:

- **Qt GUI infrastructure** handles windows, widgets, events, and layout.
- **The Calculator class** owns the calculator's state and behavior.
- **Signals and slots** connect UI events to application logic.
- **`main()`** bootstraps the whole application and enters the event loop.

---

# 2. Header inclusion

## Line 1

```cpp
#include <QApplication>
```

### C++ role

`#include` is a **preprocessor directive**. Before normal compilation, the preprocessor processes this directive and makes the declarations from the referenced header available to the translation unit.

The angle brackets normally indicate an external/system-style include path rather than a project-local quoted include.

### Qt role

`QApplication` is the application object used for a traditional Qt Widgets GUI application. It initializes application-wide state and, most importantly for this example, provides the event-processing infrastructure through `exec()`.

Without the declaration for `QApplication`, this line later in the program would not compile:

```cpp
QApplication app(argc, argv);
```

---

## Line 2

```cpp
#include <QWidget>
```

This makes the `QWidget` class available.

`QWidget` is a fundamental Qt Widgets class. A widget can represent a visible GUI object or serve as a parent/container for other widgets.

The calculator inherits from `QWidget`:

```cpp
class Calculator : public QWidget
```

So this header is essential for the base-class declaration.

---

## Line 3

```cpp
#include <QGridLayout>
```

Provides the `QGridLayout` class.

A layout controls the geometry and placement of child widgets. `QGridLayout` arranges widgets using rows and columns, which maps naturally to a calculator keypad.

---

## Line 4

```cpp
#include <QPushButton>
```

Provides the `QPushButton` class.

Each calculator key is represented by one `QPushButton` object.

---

## Line 5

```cpp
#include <QLineEdit>
```

Provides the `QLineEdit` class.

Here the program uses a `QLineEdit` as the calculator display. It is configured as read-only so the user cannot type directly into it.

---

## Line 6

```cpp
#include <QString>
```

Provides Qt's `QString` class.

`QString` is Qt's string type and is used here for:

- button text,
- the pending operator,
- display text,
- formatted numerical results.

For a Qt application, using `QString` is often preferable to manually mixing C strings and standard library strings throughout the UI layer because Qt APIs are designed around Qt types.

---

# 3. Declaring the calculator class

## Line

```cpp
class Calculator : public QWidget {
```

This declares a class named `Calculator` that publicly derives from `QWidget`.

There are several C++ concepts here.

## `class Calculator`

This introduces a user-defined type.

The class groups:

- state/data,
- functions/behavior,
- access control.

An instance of the class is later created with:

```cpp
Calculator calc;
```

---

## `: public QWidget`

This is inheritance syntax.

The relationship is:

```text
QWidget
   ▲
   │ public inheritance
   │
Calculator
```

Public inheritance means a `Calculator` is a `QWidget` from an interface/substitutability perspective.

This is what allows the calculator to use inherited widget functionality such as:

```cpp
show();
setWindowTitle(...);
setFixedSize(...);
setLayout(...);
```

Conceptually, the class has its own behavior while reusing the Qt widget infrastructure.

---

# 4. `Q_OBJECT`

```cpp
Q_OBJECT
```

`Q_OBJECT` is a Qt macro used in classes participating in Qt's meta-object system.

The meta-object system supports functionality such as:

- signals,
- slots,
- runtime type information provided by Qt,
- dynamic invocation and related Qt mechanisms.

The important connection in this program is that `Calculator` contains a slot:

```cpp
private slots:
    void onButtonClicked()
```

and is used as the receiver in:

```cpp
connect(btn, &QPushButton::clicked, this, &Calculator::onButtonClicked);
```

The Qt build system uses a tool commonly called **moc** (Meta-Object Compiler) to generate supporting code for classes using this system.

### Build-system note

The explicit:

```cpp
#include "calculator.moc"
```

at the bottom is one traditional way to include generated moc output when the class and implementation are arranged in a single `.cpp` file.

In a modern CMake project, `AUTOMOC` is commonly enabled and the generated moc code is managed by the build system, so a manual `.moc` include is often unnecessary. The exact requirement depends on how the project is configured.

---

# 5. `public:`

```cpp
public:
```

This starts a public access section of the class.

Members declared here can be accessed through the class's public interface.

The constructor is public because `main()` must be able to do:

```cpp
Calculator calc;
```

If the constructor were private, normal code outside the class could not construct a calculator instance this way.

---

# 6. The constructor

```cpp
Calculator(QWidget *parent = nullptr) : QWidget(parent) {
```

This line contains several C++ concepts.

## Constructor name

```cpp
Calculator(...)
```

A constructor has the same name as the class and no return type.

It runs when an object is created.

For:

```cpp
Calculator calc;
```

the constructor is executed before `main()` continues to `calc.show()`.

---

## Parameter: `QWidget *parent`

```cpp
QWidget *parent
```

This declares a pointer to a `QWidget`.

The parameter allows a parent widget to be supplied when constructing a `Calculator`.

---

## Default argument: `= nullptr`

```cpp
QWidget *parent = nullptr
```

If the caller does not provide a parent, `parent` receives `nullptr`.

Therefore:

```cpp
Calculator calc;
```

is equivalent, for this constructor's parameter, to conceptually calling it with no parent.

---

## Constructor initializer list

```cpp
: QWidget(parent)
```

This is the constructor's **member/base initializer list**.

Because `Calculator` derives from `QWidget`, the `QWidget` base subobject must be initialized. This expression invokes the `QWidget` constructor with the supplied `parent` pointer.

The order is conceptually:

```text
construct Calculator object
        │
        ▼
construct QWidget base subobject
        │
        ▼
execute Calculator constructor body
```

The initializer list is the correct place to initialize base classes and data members that require initialization.

---

# 7. Window title

```cpp
setWindowTitle("Simple Calculator");
```

This invokes a `QWidget` member function inherited by `Calculator`.

Because `Calculator` is a `QWidget`, it can call the widget API directly.

The string literal:

```cpp
"Simple Calculator"
```

is passed to Qt, which accepts it through its string conversion facilities for the API involved.

Effect: the top-level window gets the title `Simple Calculator`.

---

# 8. Fixed window size

```cpp
setFixedSize(300, 400);
```

This sets the widget/window's fixed size.

The values are interpreted as width and height in device-independent Qt widget coordinates for the applicable widget configuration.

The intent is:

```text
width  = 300
height = 400
```

A fixed size prevents normal user resizing of the window.

### Why this choice?

For a tiny demo calculator, fixed dimensions simplify layout reasoning. A production application might instead prefer responsive layout behavior and appropriate size hints.

---

# 9. Creating the display

```cpp
display = new QLineEdit(this);
```

This line simultaneously involves:

- a pointer member,
- dynamic allocation,
- object construction,
- Qt parenting.

The member is declared later as:

```cpp
QLineEdit *display;
```

So `display` stores a pointer to a `QLineEdit` object.

---

## What `new` does

```cpp
new QLineEdit(this)
```

allocates and constructs a `QLineEdit` object dynamically and returns a pointer to it.

The resulting pointer is assigned to the class member:

```text
display
   │
   ▼
QLineEdit object
```

---

## Why pass `this`?

Here `this` is the pointer to the current `Calculator` object.

So the code is effectively saying:

> Create this `QLineEdit` and give the current Calculator widget as its Qt parent.

Qt uses parent-child ownership semantics for widgets.

Conceptually:

```text
Calculator
   │
   └── QLineEdit
```

When a parent widget owns a child widget in this Qt sense, Qt manages the child's lifetime when the parent is destroyed.

That is one reason the program does not need an explicit:

```cpp
delete display;
```

in this example.

### Important C++/Qt distinction

The pointer itself is just a C++ pointer. The lifetime management behavior comes from **QObject/QWidget parent-child ownership**, not from raw C++ pointers automatically deleting things.

---

# 10. Read-only display

```cpp
display->setReadOnly(true);
```

The `->` operator accesses a member through a pointer.

Equivalent conceptual idea:

```cpp
(*display).setReadOnly(true);
```

though `->` is the conventional and readable form.

`setReadOnly(true)` makes the `QLineEdit` non-editable by normal user input.

### Why?

The calculator is designed so that input comes from the calculator buttons, not direct text editing.

The program itself can still modify the widget with:

```cpp
display->setText(...);
```

---

# 11. Right alignment

```cpp
display->setAlignment(Qt::AlignRight);
```

This configures the text alignment of the `QLineEdit`.

`Qt::AlignRight` is a Qt alignment flag/value representing right alignment.

The calculator display therefore behaves visually like a conventional calculator display:

```text
+----------------------+
|                  123 |
+----------------------+
```

rather than left-aligning the number.

---

# 12. Initial display value

```cpp
display->setText("0");
```

This changes the visible text to `0`.

The display internally holds text, not a calculator `double` value.

That distinction is important because later the code explicitly converts display text to numeric data using:

```cpp
display->text().toDouble()
```

So the data path is often:

```text
QString in GUI
     │
     ▼
.toDouble()
     │
     ▼
double in calculation logic
```

and after calculation:

```text
double result
     │
     ▼
QString::number(...)
     │
     ▼
QString in GUI
```

---

# 13. Styling the display

```cpp
display->setStyleSheet("font-size: 24px; padding: 10px;");
```

Qt Widgets support stylesheet syntax that is intentionally similar to CSS.

The string specifies two visual properties:

```css
font-size: 24px;
padding: 10px;
```

This is presentation-layer configuration.

It does not modify the calculator's arithmetic state.

A useful separation is:

```text
business/application logic  → currentValue, operator, calculate()
view/presentation            → font, size, alignment, styling
```

---

# 14. Creating the grid layout

```cpp
QGridLayout *layout = new QGridLayout(this);
```

This creates a `QGridLayout` dynamically and stores a pointer to it in `layout`.

The constructor receives `this`, which establishes the layout on the current widget in Qt's layout system.

A grid is a natural fit for this calculator:

```text
column 0   column 1   column 2   column 3

row 0      display spans all four columns
row 1      7          8          9          /
row 2      4          5          6          *
row 3      1          2          3          -
row 4      0          C          =          +
```

---

# 15. Add the display to the grid

```cpp
layout->addWidget(display, 0, 0, 1, 4);
```

`addWidget()` places a widget into the layout.

The arguments represent, in this overload:

```text
widget
row
column
row span
column span
```

Therefore:

```cpp
layout->addWidget(display, 0, 0, 1, 4);
```

means:

- row = 0
- column = 0
- span 1 row
- span 4 columns

So the display occupies the entire top row.

Visual model:

```text
┌──────────────────────────────┐
│           DISPLAY            │
│       4 columns wide         │
└──────────────────────────────┘
┌──────┬──────┬──────┬──────┐
│      │      │      │      │
└──────┴──────┴──────┴──────┘
```

---

# 16. Button label array

```cpp
const char* buttons[16] = {
    "7", "8", "9", "/",
    "4", "5", "6", "*",
    "1", "2", "3", "-",
    "0", "C", "=", "+"
};
```

This declares a C-style array of 16 elements.

## `buttons[16]`

The array contains 16 elements, indexed from `0` through `15`.

The mapping is:

```text
index   value
-----   -----
0       "7"
1       "8"
2       "9"
3       "/"
4       "4"
5       "5"
6       "6"
7       "*"
8       "1"
9       "2"
10      "3"
11      "-"
12      "0"
13      "C"
14      "="
15      "+"
```

---

## Why `const char*`?

`"7"`, `"8"`, and the other entries are string literals.

The declaration:

```cpp
const char* buttons[16]
```

means each array element is a pointer to constant character data.

At this stage, the program is using a traditional C-style representation for literal text.

An alternative design could use Qt types directly, for example a collection of `QString`s. The current approach is simple and sufficient because the labels are compile-time string literals and `QPushButton` can consume the supplied text through Qt's API conversions.

---

# 17. Position counter

```cpp
int pos = 0;
```

This is an integer index into the `buttons` array.

The code uses one linear index (`pos`) while using two-dimensional coordinates (`row`, `col`) for the GUI layout.

This creates a mapping:

```text
linear array index
       ↓
     buttons[pos]
       ↓
 button at (row, col)
```

This is a simple way to avoid manually writing 16 separate button declarations.

---

# 18. Outer loop

```cpp
for (int row = 1; row <= 4; ++row) {
```

This loops through the four button rows.

The loop has three parts:

```cpp
int row = 1
```

Initialization: begin at row 1.

```cpp
row <= 4
```

Condition: continue while row is 4 or less.

```cpp
++row
```

Increment: increase the row by one after each iteration.

The row starts at 1 because row 0 is occupied by the display.

---

# 19. Inner loop

```cpp
for (int col = 0; col < 4; ++col) {
```

This loops through the four columns:

```text
0
1
2
3
```

The nesting gives exactly 16 iterations:

```text
4 rows × 4 columns = 16 buttons
```

This exactly matches:

```cpp
buttons[16]
```

---

# 20. Creating each button

```cpp
QPushButton *btn = new QPushButton(buttons[pos], this);
```

This is executed once for every grid position.

The constructor arguments are conceptually:

```text
button text = buttons[pos]
parent      = this
```

For the first iteration:

```text
pos = 0
buttons[pos] = "7"
```

so the operation is conceptually:

```cpp
new QPushButton("7", this);
```

The second iteration uses `"8"`, and so on.

Again, passing `this` establishes the calculator as the Qt parent of the button.

Therefore the ownership structure is approximately:

```text
Calculator
├── QLineEdit display
├── QPushButton 7
├── QPushButton 8
├── QPushButton 9
├── ...
└── QPushButton +
```

---

# 21. Minimum button size

```cpp
btn->setMinimumSize(60, 50);
```

This configures a minimum width and height for the button.

It is a presentation/layout constraint.

The layout manager still decides the final geometry within its rules, but it must respect the widget's size constraints as applicable.

---

# 22. Button stylesheet

```cpp
btn->setStyleSheet("font-size: 18px;");
```

The button text is styled with an 18-pixel font size.

Again, this is UI configuration, not application logic.

---

# 23. Add the button to the grid

```cpp
layout->addWidget(btn, row, col);
```

This inserts the newly created button at the current grid coordinates.

Because `row` and `col` are generated by the nested loops, the result is:

```text
row 1: 7   8   9   /
row 2: 4   5   6   *
row 3: 1   2   3   -
row 4: 0   C   =   +
```

The array is linear; the layout coordinates are two-dimensional.

---

# 24. Connecting the button to the slot

```cpp
connect(btn, &QPushButton::clicked, this, &Calculator::onButtonClicked);
```

This is the central Qt event-handling line.

The modern Qt function-pointer syntax makes the connection explicit and strongly typed.

The four arguments are:

```text
1. sender object
2. signal
3. receiver object
4. slot/member function
```

So here:

```text
sender   = btn
signal   = QPushButton::clicked
receiver = this (Calculator)
slot     = Calculator::onButtonClicked
```

Conceptually:

```text
QPushButton::clicked
          │
          ▼
  Calculator::onButtonClicked()
```

---

## Why this is useful

The calculator creates 16 buttons but does not need 16 handlers.

Instead of:

```text
on7Clicked()
on8Clicked()
on9Clicked()
...
onPlusClicked()
```

every button routes to the same handler:

```cpp
onButtonClicked()
```

The handler can then inspect which button emitted the signal.

---

## Why `&QPushButton::clicked`?

The `&Class::member` syntax forms a pointer to a member function. In this case Qt's `connect()` overload uses the member-function pointer to identify the signal.

`clicked` is a Qt signal provided by `QAbstractButton`/`QPushButton`'s interface.

The signal includes a boolean argument in its standard form (`checked`), while the receiver here has no parameters. Qt's signal/slot connection rules permit a receiver to accept fewer parameters than the signal provides, provided the connection is otherwise compatible.

---

# 25. Advance the button index

```cpp
++pos;
```

This increments the label index after each button is created.

Equivalent arithmetic idea:

```cpp
pos = pos + 1;
```

The sequence therefore becomes:

```text
0 → "7"
1 → "8"
2 → "9"
...
15 → "+"
```

---

# 26. `setLayout(layout)`

```cpp
setLayout(layout);
```

This associates the layout with the current widget.

### Important implementation detail

The code previously constructed the layout as:

```cpp
new QGridLayout(this)
```

Passing the widget as the layout's parent already integrates it with the widget's layout hierarchy in the standard Qt Widgets model. Consequently, calling `setLayout(layout)` afterwards is generally **redundant** in this exact implementation.

In a cleaner version, one would normally choose one approach rather than doing both.

For example:

```cpp
QGridLayout *layout = new QGridLayout;
setLayout(layout);
```

or construct it directly with the widget parent where appropriate.

The line is retained in the given code, but it is not necessary to understand the calculator's behavior.

---

# 27. Initializing calculator state

```cpp
currentValue = 0.0;
pendingOperator = "";
waitingForOperand = true;
```

These three lines initialize the application's logical state.

They are different from the widget setup above.

The widget setup establishes **what the user sees**.

These variables establish **what the calculator believes is happening**.

---

## `currentValue`

Declared later as:

```cpp
double currentValue;
```

A `double` is a floating-point arithmetic type.

It stores the current accumulated value used by the calculator.

---

## `pendingOperator`

Declared later as:

```cpp
QString pendingOperator;
```

This stores the operator waiting to be applied:

```text
"+"
"-"
"*"
"/"
```

An empty string means there is no pending operator.

---

## `waitingForOperand`

Declared as:

```cpp
bool waitingForOperand;
```

This is a state flag.

`true` means the next digit should normally start a new operand.

`false` means a digit should normally be appended to the existing display text.

This one bit of state prevents a sequence such as:

```text
7 + 5
```

from accidentally turning `5` into `75`.

---

# 28. Qt slot declaration

```cpp
private slots:
```

This is a Qt-specific access-section form associated with Qt's meta-object system.

The function below is intended to be a slot that can receive a Qt signal.

The slot is private because it is an internal implementation detail of `Calculator`; outside code does not need to call it as part of the calculator's public API.

---

# 29. Button event handler

```cpp
void onButtonClicked() {
```

This defines a member function.

## `void`

The function returns no value.

## `onButtonClicked`

The name describes the event being processed.

## No arguments

The function does not need the clicked button passed as an explicit argument because it obtains the signal sender dynamically using:

```cpp
sender()
```

---

# 30. Identify the sending button

```cpp
QPushButton *btn = qobject_cast<QPushButton*>(sender());
```

This is a very Qt-specific line and deserves careful treatment.

## `sender()`

Inside a Qt slot being invoked as the result of a signal, `sender()` can identify the QObject that emitted the signal.

Because every calculator button is connected to the same slot, this allows the slot to determine which button triggered the event.

Conceptually:

```text
user clicks + button
       │
       ▼
QPushButton::clicked emitted
       │
       ▼
onButtonClicked()
       │
       ▼
sender() → + button
```

---

## `qobject_cast<QPushButton*>`

`sender()` returns a `QObject*`-compatible Qt object reference.

The calculator knows that, in this design, the signal sources are `QPushButton` objects, so the code attempts to cast the sender to `QPushButton*` using Qt's runtime-aware `qobject_cast` mechanism.

This is preferable to a raw C-style cast because Qt can validate the QObject type through its meta-object information.

---

## Why the result is a pointer

The result is assigned to:

```cpp
QPushButton *btn
```

which lets the code do:

```cpp
btn->text();
```

and access the button's API.

---

# 31. Null check

```cpp
if (!btn) return;
```

This is a guard clause.

`!btn` is true when `btn` is null.

If the cast fails, the function returns immediately instead of dereferencing an invalid pointer.

The same idea can be written more verbosely as:

```cpp
if (btn == nullptr) {
    return;
}
```

The compact version is idiomatic C++.

---

# 32. Read the button label

```cpp
QString text = btn->text();
```

`btn->text()` returns the text displayed by the button as a `QString`.

Examples:

```text
"7"
"+"
"C"
"="
```

Now the generic event handler has converted a GUI event into a simple piece of application data:

```text
clicked widget → text label
```

That is what allows the following conditional logic to classify the action.

---

# 33. Detecting numeric buttons

```cpp
if (text >= "0" && text <= "9") {
```

This checks whether the text is within the range from `"0"` to `"9"` using QString comparisons.

Because every numeric button label in this program is exactly one character, the comparison works for the intended set of inputs.

### Important nuance

These are **string comparisons**, not numeric comparisons.

The code is relying on lexicographic ordering of the single-character strings. It is safe for this exact set of one-character digit labels, but it is not a general-purpose numeric parser.

A more explicit approach in a production implementation might inspect a character or attempt numeric conversion directly.

---

# 34. If we are waiting for an operand

```cpp
if (waitingForOperand) {
```

This checks the calculator's state flag.

Suppose the user just pressed `+`.

Then the calculator sets:

```cpp
waitingForOperand = true;
```

The next digit should therefore replace the displayed value instead of being appended to it.

---

# 35. Replace the display

```cpp
display->setText(text);
```

Suppose the display currently contains:

```text
7
```

and the user has pressed `+`, making `waitingForOperand == true`.

Then pressing `5` causes:

```text
"7" → "5"
```

instead of:

```text
"7" + "5" → "75"
```

This is state-dependent input handling.

---

# 36. Stop waiting

```cpp
waitingForOperand = false;
```

Once the first digit of the new operand has been entered, the calculator is no longer waiting for its first digit.

If another digit is clicked, it should be appended.

---

# 37. Continue building a number

```cpp
else {
    display->setText(display->text() + text);
}
```

Here `waitingForOperand` is false.

Therefore the new digit is concatenated to the existing text.

For example:

```text
current display = "5"
new text        = "3"

"5" + "3" = "53"
```

The `+` here is **string concatenation**, because both operands are `QString` objects.

This is an important example of operator overloading / type-dependent operator behavior in C++ and Qt.

The code is not performing arithmetic addition at this point.

---

# 38. The clear button

```cpp
else if (text == "C") {
```

This branch handles the clear operation.

Because the digit test already failed, the code now checks whether the clicked text equals `"C"`.

---

## Reset the visible value

```cpp
display->setText("0");
```

The UI returns to the initial display state.

---

## Reset numerical state

```cpp
currentValue = 0.0;
```

The accumulated numeric state is reset.

---

## Reset operator state

```cpp
pendingOperator = "";
```

No operation remains pending.

---

## Reset input mode

```cpp
waitingForOperand = true;
```

The next digit is treated as the beginning of a fresh operand.

This is a proper state reset because the code resets both:

- what the user sees,
- and what the calculator remembers internally.

---

# 39. The equals button

```cpp
else if (text == "=") {
```

This identifies the calculate/execute command.

---

## Perform calculation

```cpp
calculate();
```

This calls the calculator's private helper function.

This is a good separation of responsibilities:

```text
onButtonClicked()
    → handles user interaction and command routing

calculate()
    → performs arithmetic logic
```

The event handler does not contain all arithmetic details itself.

---

## Clear the pending operator after `=`

```cpp
pendingOperator = "";
```

The operation has been consumed, so the pending operator is cleared.

---

## Prepare for fresh input

```cpp
waitingForOperand = true;
```

The next digit normally starts a new operand rather than extending the previous result.

---

# 40. The operator branch

```cpp
else { // operator
```

At this point the input was not:

- a digit,
- `C`,
- or `=`.

Given the set of button labels in this program, the remaining possibilities are arithmetic operators:

```text
+
-
*
/
```

The comment documents that assumption.

---

# 41. Existing pending operator check

```cpp
if (!pendingOperator.isEmpty()) {
    calculate();
}
```

This asks whether there is already an operator waiting to be applied.

`QString::isEmpty()` returns true when the string contains no characters.

So:

```cpp
!pendingOperator.isEmpty()
```

means:

> the operator string is not empty.

---

# 42. Why calculate when another operator is pressed?

Consider:

```text
7 + 5 *
```

After `7 +`:

```text
currentValue = 7
pendingOperator = "+"
waitingForOperand = true
```

After `5`:

```text
display = "5"
```

When `*` is pressed, `pendingOperator` is already `+`.

Therefore the program calls:

```cpp
calculate();
```

which evaluates:

```text
7 + 5 = 12
```

Then the code replaces the pending operator with `*`.

This produces sequential calculator behavior rather than full mathematical precedence handling.

---

# 43. First operator case

The other branch is:

```cpp
else {
    currentValue = display->text().toDouble();
}
```

This happens when no operator is currently pending.

For example, the user has entered:

```text
75
```

and now presses `+`.

The display stores the text:

```text
"75"
```

but the calculation state needs a numeric value:

```text
75.0
```

So the code calls:

```cpp
display->text().toDouble()
```

---

# 44. `toDouble()` conversion

The expression:

```cpp
display->text()
```

returns a `QString`.

Then:

```cpp
toDouble()
```

parses it as a `double`.

Conceptually:

```text
QString("75")
      │
      ▼
toDouble()
      │
      ▼
75.0
```

### Conversion note

The overload used here does not inspect a success flag. In production code where invalid display text is possible, checking parsing success would be more robust. In this program, the display is mostly controlled by the program itself, so the author assumes it contains a valid numerical representation unless it contains the special error text.

That assumption becomes relevant to some edge cases discussed later.

---

# 45. Store the selected operator

```cpp
pendingOperator = text;
```

Now that the first operand is stored and the operator is known, the calculator records the command to perform later.

Example:

```text
currentValue    = 75
pendingOperator = "+"
```

---

# 46. Mark the next input as a new operand

```cpp
waitingForOperand = true;
```

This ensures the next digit starts the second operand.

For:

```text
75 + 2
```

a click on `2` should replace the display rather than produce `752`.

---

# 47. The `calculate()` function

```cpp
void calculate() {
```

This function is private and returns no value.

It modifies calculator state and the display directly.

Its job is to take the current state:

```text
currentValue
pendingOperator
display text
```

and turn it into an updated result.

---

# 48. Read the current operand

```cpp
double operand = display->text().toDouble();
```

The display is treated as the second operand.

Example:

```text
display = "5"
```

becomes:

```text
operand = 5.0
```

This is why the program distinguishes the UI representation (`QString`) from the arithmetic representation (`double`).

---

# 49. Initialize `result`

```cpp
double result = currentValue;
```

This initializes the result with the current accumulated value.

For a supported operator, the code then overwrites `result` with the actual calculation.

The initial assignment is useful for the case where there is no recognized operator: the current value remains unchanged.

It also means the variable always has a defined value before the branches finish.

---

# 50. Addition branch

```cpp
if (pendingOperator == "+")
    result = currentValue + operand;
```

The code checks the stored operator.

If it is `+`, ordinary C++ numeric addition occurs because both operands are `double` values.

Example:

```text
currentValue = 7.0
operand      = 5.0

result = 7.0 + 5.0
       = 12.0
```

---

# 51. Subtraction branch

```cpp
else if (pendingOperator == "-")
    result = currentValue - operand;
```

For:

```text
10 - 3
```

the result becomes:

```text
7
```

The `-` operator here is C++ floating-point subtraction because `currentValue` and `operand` are `double`s.

---

# 52. Multiplication branch

```cpp
else if (pendingOperator == "*")
    result = currentValue * operand;
```

For:

```text
4 * 6
```

the calculation is:

```text
24
```

Again, it is standard arithmetic on `double` values.

---

# 53. Division branch

```cpp
else if (pendingOperator == "/") {
```

The division case uses braces because it contains multiple statements, including validation.

---

# 54. Division-by-zero guard

```cpp
if (operand == 0.0) {
```

The calculator explicitly tests for zero before dividing.

This prevents the code from blindly executing:

```cpp
currentValue / operand
```

when the divisor is zero.

### Note about floating-point equality

Testing floating-point values for exact equality is generally something programmers should think about carefully. Here, the comparison is being used specifically to detect a parsed input of zero, and for the digit-only UI without decimal input, the relevant values are straightforward. The program is not doing approximate numerical analysis.

---

# 55. Show the error

```cpp
display->setText("Error");
```

The visible display becomes the string:

```text
Error
```

This is no longer a numerical representation.

That has an important consequence: the program has entered an error/display state rather than a normal numeric state.

---

# 56. Recover into a fresh input state

```cpp
waitingForOperand = true;
```

The next digit starts a new operand.

---

# 57. Remove the invalid pending operation

```cpp
pendingOperator = "";
```

The failed division is cleared.

---

# 58. Early return

```cpp
return;
```

This exits `calculate()` immediately.

That is crucial because without the return, the function could continue to the normal formatting and state-update code.

The control flow is therefore:

```text
zero divisor
   │
   ├── display "Error"
   ├── reset input state
   ├── clear operator
   └── return immediately
```

---

# 59. Normal division

If the operand is not zero:

```cpp
result = currentValue / operand;
```

For example:

```text
10 / 2
→ 5
```

The operation is ordinary `double` division.

---

# 60. Convert the result back to display text

```cpp
display->setText(QString::number(result, 'g', 12));
```

This line reverses the earlier `QString → double` conversion.

`result` is numeric, but the GUI display expects text.

So:

```text
double result
      │
      ▼
QString::number(...)
      │
      ▼
QString
      │
      ▼
QLineEdit::setText()
```

---

# 61. `QString::number`

```cpp
QString::number(result, 'g', 12)
```

This is a static member function of `QString` that formats a number into a string.

The general intent of the arguments here is:

- `result` — the number to format,
- `'g'` — general formatting,
- `12` — requested precision/significant-digit formatting according to the chosen representation.

The important design decision is that the result is converted to display text at the boundary between model/calculation state and GUI presentation.

---

# 62. Store the result as the new current value

```cpp
currentValue = result;
```

This is essential for chained operations.

Example:

```text
7 + 5 = 12
```

After calculating:

```text
currentValue = 12
```

So a following operation can use 12 as its starting value.

Without this line, the display might show 12 while the internal state still contained 7, producing incorrect subsequent calculations.

---

# 63. Member variables

The class ends with:

```cpp
QLineEdit *display;
double currentValue;
QString pendingOperator;
bool waitingForOperand;
```

These are **non-static data members** of the `Calculator` object.

Every `Calculator` instance would have its own copies.

---

## `QLineEdit *display`

Pointer to the display widget.

It allows every member function to access the same display object after the constructor has created it.

Why a member instead of a local constructor variable?

Because `onButtonClicked()` and `calculate()` need access to the display later, long after the constructor has finished.

---

## `double currentValue`

Stores the numeric state of the calculator.

A member is necessary because the value must survive across multiple event-handler calls.

---

## `QString pendingOperator`

Stores the current operator that is waiting to be applied.

This is part of the calculator's state machine.

---

## `bool waitingForOperand`

Determines how the next digit should be interpreted.

The two modes are roughly:

```text
false → append digits to the current display
true  → first digit starts a new operand
```

---

# 64. Why the state has to be stored as members

This is one of the most important architectural points in the whole example.

The application is event-driven.

The function:

```cpp
onButtonClicked()
```

runs for one event and then returns.

Later, another click causes another invocation.

For example:

```text
click 7
    ↓
onButtonClicked()
    ↓
return

click +
    ↓
onButtonClicked()
    ↓
return

click 5
    ↓
onButtonClicked()
    ↓
return

click =
    ↓
onButtonClicked()
    ↓
calculate()
```

Local variables created inside one invocation are not the right mechanism for preserving the ongoing calculation state across separate events.

The member variables persist with the `Calculator` object.

That is why the object effectively carries state through time:

```text
Calculator object
│
├── currentValue
├── pendingOperator
├── waitingForOperand
└── display pointer
```

---

# 65. End of class

```cpp
};
```

This terminates the class definition.

The semicolon is required after a class definition in C++.

---

# 66. The `calculator.moc` include

```cpp
#include "calculator.moc"
```

This includes generated Qt meta-object implementation code in setups where the moc output is expected to be included into the source file.

The associated reason is the earlier:

```cpp
Q_OBJECT
```

### Why quoted include syntax?

```cpp
#include "calculator.moc"
```

uses quotes, conventionally indicating a project/local generated file lookup.

### Modern project note

A modern CMake Qt project often uses an automatic moc mechanism, so you may not manually write this include depending on the project structure. Do not mechanically copy this line into every Qt source file without understanding your build system.

---

# 67. Program entry point

```cpp
int main(int argc, char *argv[]) {
```

`main()` is the program's entry point.

The operating system starts the executable, and C++ execution begins in `main()` for a normal hosted application.

---

## Return type: `int`

The program returns an integer exit status to the operating system.

A successful exit normally uses zero.

---

## `argc`

The argument count.

It tells the application how many command-line arguments were supplied.

---

## `argv`

The argument vector.

It points to the command-line argument strings.

Qt accepts these inputs because application initialization may need to inspect command-line arguments.

---

# 68. Create `QApplication`

```cpp
QApplication app(argc, argv);
```

This creates the application object.

It initializes the Qt Widgets application infrastructure and becomes the central application object for this process.

The lifetime is automatic because `app` is a local automatic object in `main()`.

When `main()` exits normally, C++ destroys local objects in reverse construction order, including `app`.

---

# 69. Construct the calculator

```cpp
Calculator calc;
```

This creates an automatic `Calculator` object on the stack frame of `main()`.

This is different from the dynamically allocated Qt child widgets created with `new`.

The important distinction is:

```text
Calculator calc;
→ automatic storage duration
→ destroyed automatically when main() leaves this scope
```

while:

```cpp
new QPushButton(...)
```

creates dynamically allocated child widgets whose lifecycle is managed through Qt's parent-child object ownership.

---

# 70. What happens during `Calculator calc;`?

The constructor runs immediately.

That constructor:

1. initializes the QWidget base,
2. sets the window title,
3. sets the window size,
4. creates the display,
5. configures the display,
6. creates the grid layout,
7. creates all 16 buttons,
8. connects every button's `clicked` signal,
9. initializes calculator state.

When the constructor returns, the `calc` object is fully initialized.

---

# 71. Show the window

```cpp
calc.show();
```

This requests that the calculator widget become visible.

The object already exists; this changes its GUI visibility state.

The window is not yet interacting because the application has to enter its event-processing loop.

---

# 72. Enter Qt's event loop

```cpp
return app.exec();
```

This is the line that turns the initialized GUI into an interactive application.

`app.exec()` starts Qt's event loop.

Conceptually, the loop behaves like:

```text
while application is running:
    wait for an event
    dispatch the event
    process the event
```

Real GUI frameworks do much more than this simplified pseudocode, but the mental model is useful.

Events can include things such as:

- mouse input,
- button clicks,
- repaint requests,
- window-manager events,
- timers,
- application shutdown events.

---

# 73. The entire runtime construction sequence

The application startup can now be seen as:

```text
OS launches executable
        │
        ▼
     main()
        │
        ├── QApplication app(argc, argv)
        │
        ├── Calculator calc
        │        │
        │        └── Calculator constructor
        │              ├── QWidget base initialization
        │              ├── display creation
        │              ├── layout creation
        │              ├── 16 button creation
        │              ├── signal/slot connections
        │              └── state initialization
        │
        ├── calc.show()
        │
        └── app.exec()
                 │
                 ▼
            EVENT LOOP
```

After `exec()` starts, execution is effectively event-driven rather than a simple linear sequence of calculator commands.

---

# 74. Complete click flow: `7 + 5 =`

Now trace an actual user interaction.

Initial state after construction:

```text
display            = "0"
currentValue       = 0.0
pendingOperator    = ""
waitingForOperand  = true
```

---

## Click `7`

The `7` button emits `clicked`.

Because of:

```cpp
connect(btn, &QPushButton::clicked, this, &Calculator::onButtonClicked);
```

Qt invokes:

```cpp
onButtonClicked()
```

Inside:

```cpp
sender()
```

identifies the `7` button.

Then:

```cpp
QString text = btn->text();
```

produces:

```text
"7"
```

Digit branch is selected.

Because:

```text
waitingForOperand == true
```

this executes:

```cpp
display->setText("7");
waitingForOperand = false;
```

State becomes:

```text
display           = "7"
currentValue      = 0.0
pendingOperator   = ""
waitingForOperand = false
```

---

## Click `+`

The text is:

```text
"+"
```

This is not a digit, not `C`, and not `=`.

The operator branch runs.

No operator is pending, so:

```cpp
currentValue = display->text().toDouble();
```

becomes:

```text
"7" → 7.0
```

Then:

```cpp
pendingOperator = "+";
waitingForOperand = true;
```

State:

```text
display           = "7"
currentValue      = 7.0
pendingOperator   = "+"
waitingForOperand = true
```

---

## Click `5`

The digit branch runs.

Since:

```text
waitingForOperand == true
```

it replaces the display:

```text
"7" → "5"
```

and sets:

```text
waitingForOperand = false
```

State:

```text
display           = "5"
currentValue      = 7.0
pendingOperator   = "+"
waitingForOperand = false
```

---

## Click `=`

The equals branch calls:

```cpp
calculate();
```

Inside `calculate()`:

```cpp
double operand = display->text().toDouble();
```

produces:

```text
operand = 5.0
```

The pending operator is `+`, so:

```cpp
result = currentValue + operand;
```

becomes:

```text
result = 7.0 + 5.0
       = 12.0
```

Then:

```cpp
display->setText(QString::number(result, 'g', 12));
```

updates the GUI to:

```text
"12"
```

and:

```cpp
currentValue = result;
```

stores:

```text
currentValue = 12.0
```

Finally, back in `onButtonClicked()`:

```cpp
pendingOperator = "";
waitingForOperand = true;
```

Final state:

```text
display           = "12"
currentValue      = 12.0
pendingOperator   = ""
waitingForOperand = true
```

---

# 75. Why the display and `currentValue` are separate

A programmer coming from other languages may initially ask:

> Why not simply keep the number in one variable and derive the display from it?

The author chose a simpler GUI-oriented architecture where:

```text
display = QString
```

is the immediate text representation and:

```text
currentValue = double
```

is the accumulated numeric state.

This makes button-driven text construction easy:

```cpp
display->setText(display->text() + text);
```

Then arithmetic can use:

```cpp
display->text().toDouble();
```

It is not the only possible design. A more elaborate calculator could maintain a stronger separation between model state and display state, especially once decimals, unary operators, expressions, history, precedence, and error states are added.

---

# 76. The program as a state machine

The calculator can be understood as a small finite-state machine.

One useful abstraction is:

```text
┌──────────────────┐
│ Starting / clear │
└────────┬─────────┘
         │ digit
         ▼
┌──────────────────┐
│ Entering number  │
└────────┬─────────┘
         │ operator
         ▼
┌────────────────────────┐
│ Waiting for next number│
└────────────┬───────────┘
             │ digit
             ▼
┌──────────────────┐
│ Entering operand │
└────────┬─────────┘
         │ = / next operator
         ▼
┌──────────────────┐
│ Calculate result │
└────────┬─────────┘
         ▼
      Result
```

The actual code encodes this state using the variables:

```cpp
currentValue
pendingOperator
waitingForOperand
```

There is no explicit `enum class State`, but the same conceptual idea exists.

---

# 77. Sequential operator behavior

Consider:

```text
2 + 3 * 4 =
```

A full mathematical expression evaluator normally gives multiplication precedence:

```text
2 + (3 * 4) = 14
```

This calculator does something different.

After:

```text
2 + 3
```

when `*` is clicked, `pendingOperator` is already `+`, so this runs:

```cpp
calculate();
```

which evaluates:

```text
2 + 3 = 5
```

Then `*` becomes the new pending operator.

Then `4` and `=` produce:

```text
5 * 4 = 20
```

So this code implements a **sequential / immediate-execution calculator model**, not a full expression parser.

This behavior follows directly from the control flow; it is not an accidental C++ property.

---

# 78. Important edge cases and limitations

Understanding a program professionally also means understanding where the design is intentionally incomplete.

## 78.1 No decimal point

There is no `.` button and no decimal-entry logic.

The calculator can still produce decimal results through division:

```text
5 / 2 = 2.5
```

but the user cannot directly type a decimal operand such as `3.14` through the UI.

---

## 78.2 Negative numbers are not directly entered

There is no unary minus button.

Negative results can occur through subtraction, but negative-number entry as an operand is not directly modeled.

---

## 78.3 No backspace

There is no way to remove only the last digit.

---

## 78.4 No real expression parser

There is no tokenization, AST, operator precedence, parentheses, or expression grammar.

The application is a stateful immediate-operation calculator.

---

## 78.5 Error state is simplistic

After division by zero, the display contains:

```text
Error
```

The program sets `waitingForOperand = true`, so a following digit can replace the error text. This is a simple recovery strategy.

However, any path that attempts to interpret the current display as a number while it still contains `"Error"` would depend on how the conversion behaves. A production implementation would usually represent the error state explicitly rather than allowing special display text to double as application state.

---

## 78.6 Parsing success is not checked

This line:

```cpp
display->text().toDouble();
```

can be made more robust by using the conversion form that reports whether parsing succeeded.

That becomes especially important once the display can contain nonnumeric states such as `Error`.

---

## 78.7 No keyboard input

The line edit is read-only and there is no keyboard event handler, so normal keyboard digit/operator entry is not implemented.

---

# 79. Why use a single slot instead of 16 slots?

The design:

```cpp
connect(btn, &QPushButton::clicked, this, &Calculator::onButtonClicked);
```

for every button is compact and scalable for this tiny UI.

The handler performs dynamic dispatch based on the button's text.

The trade-off is that behavior is string-driven:

```text
"7"
"+"
"="
"C"
```

A larger application might prefer a more structured representation, such as storing an action type or using dedicated command metadata instead of comparing UI strings.

For a small educational calculator, the current design is straightforward.

---

# 80. Why use `QGridLayout` instead of absolute positions?

The code never does something like:

```cpp
btn->move(100, 200);
```

Instead it uses a layout:

```cpp
layout->addWidget(btn, row, col);
```

This delegates geometry management to Qt.

Advantages include:

- less manual coordinate calculation,
- automatic layout adaptation,
- better separation of widget creation from positioning,
- easier changes to spacing or size policies.

For GUI development, layout managers are usually preferable to hard-coded pixel positions.

---

# 81. Why dynamically allocate widgets with `new`?

The code uses:

```cpp
new QLineEdit(this)
new QGridLayout(this)
new QPushButton(buttons[pos], this)
```

A Qt Widgets hierarchy commonly uses dynamically created child widgets because the widgets need to survive beyond the current constructor statement and be managed as part of the GUI object tree.

The key Qt-specific point is that parent-child ownership provides lifetime management for `QObject`-based objects.

The code does not manually delete every button because the parent hierarchy is intended to own them.

---

# 82. Automatic vs dynamic storage in this program

There are two distinct lifetime patterns.

## Automatic object

```cpp
Calculator calc;
```

The C++ language automatically destroys `calc` when `main()`'s scope ends.

## Dynamically allocated child objects

```cpp
new QPushButton(...)
new QLineEdit(...)
```

These are heap allocations, but Qt's parent-child system is used so the parent can manage their destruction.

This is a useful example because raw `new` in C++ does not automatically imply a manual `delete` everywhere; ownership semantics matter.

---

# 83. C++ features demonstrated by this code

This small program touches several important C++ concepts.

### Preprocessor inclusion

```cpp
#include <...>
```

### Classes

```cpp
class Calculator
```

### Public inheritance

```cpp
: public QWidget
```

### Constructors

```cpp
Calculator(...)
```

### Constructor initializer lists

```cpp
: QWidget(parent)
```

### Pointers

```cpp
QWidget *parent
QLineEdit *display
QPushButton *btn
```

### Dynamic allocation

```cpp
new QPushButton(...)
```

### Arrays

```cpp
const char* buttons[16]
```

### Loops

```cpp
for (...)
```

### Conditionals

```cpp
if / else if / else
```

### Member functions

```cpp
calculate()
onButtonClicked()
```

### Member variables

```cpp
currentValue
pendingOperator
waitingForOperand
```

### Static/member-function access

```cpp
QString::number(...)
```

### Operator overloading / type-dependent operators

```cpp
QString + QString
```

### Runtime object casting through Qt

```cpp
qobject_cast<QPushButton*>(...)
```

### `nullptr`

```cpp
QWidget *parent = nullptr
```

### Boolean state

```cpp
true
false
```

---

# 84. Qt concepts demonstrated by this code

The Qt-specific ideas are equally important.

### Widgets

```cpp
QWidget
QPushButton
QLineEdit
```

### Layout management

```cpp
QGridLayout
```

### Parent-child object ownership

```cpp
new QPushButton(..., this)
```

### Signals and slots

```cpp
connect(...)
```

### Signal sender lookup

```cpp
sender()
```

### Qt runtime casting

```cpp
qobject_cast<...>()
```

### Qt meta-object system

```cpp
Q_OBJECT
```

### Event loop

```cpp
app.exec()
```

### Qt strings

```cpp
QString
```

---

# 85. One complete conceptual trace

A useful way to see the program is to follow one complete click from hardware input to arithmetic result.

```text
PHYSICAL MOUSE CLICK
        │
        ▼
Qt event system
        │
        ▼
QPushButton receives click
        │
        ▼
QPushButton::clicked signal
        │
        ▼
Qt connection created by connect()
        │
        ▼
Calculator::onButtonClicked()
        │
        ├── sender()
        │      ↓
        │   identify button
        │
        ├── btn->text()
        │      ↓
        │   identify command
        │
        ├── update display/state
        │
        └── for '=' or chained operators
               ↓
           calculate()
               │
               ├── read QString display
               ├── convert to double
               ├── inspect pendingOperator
               ├── perform arithmetic
               ├── format result as QString
               └── update currentValue
```

This is the fundamental end-to-end pipeline.

---

# 86. Responsibilities of each function

## Constructor

```cpp
Calculator(...)
```

Responsible for **construction and initialization**:

- initialize the QWidget base,
- create child widgets,
- create the layout,
- populate buttons,
- establish signal/slot connections,
- initialize state.

---

## `onButtonClicked()`

Responsible for **input interpretation and event routing**:

- determine which widget emitted the signal,
- inspect the button label,
- update input state,
- invoke calculation when appropriate.

---

## `calculate()`

Responsible for **arithmetic state transition**:

- obtain the operand,
- apply the pending operator,
- handle division by zero,
- format the result,
- store the new current value.

---

## `main()`

Responsible for **application bootstrap and lifetime**:

- create the Qt application object,
- create the calculator,
- show it,
- run the event loop.

---

# 87. A cleaner mental model of the state

You can imagine the calculator object carrying a record like this:

```text
Calculator State
────────────────────────────────
display            = "5"
currentValue       = 7.0
pendingOperator    = "+"
waitingForOperand  = false
────────────────────────────────
```

The important point is that `display` and `currentValue` are not always identical.

For:

```text
7 + 5
```

before `=`:

```text
display           = "5"
currentValue      = 7.0
pendingOperator   = "+"
```

The display contains the **new operand being entered**, while `currentValue` contains the **previous accumulated value**.

That's why the program needs both.

---

# 88. Why the order of code matters

The constructor has an intentional setup sequence.

```text
1. Create/configure window
2. Create/configure display
3. Create layout
4. Add display
5. Create buttons
6. Add buttons to layout
7. Connect button signals
8. Initialize calculator state
```

There are dependencies between these operations.

For example:

```cpp
layout->addWidget(display, ...)
```

requires `display` and `layout` to already exist.

Likewise:

```cpp
connect(btn, ...)
```

requires the button object to exist before it can be connected.

Understanding these dependencies is more useful than memorizing the exact order blindly.

---

# 89. A few design choices that are reasonable for a demo

The code makes several choices that are appropriate for a small educational example:

```text
single Calculator class
single event handler
simple QString operator storage
three state variables
QGridLayout
button creation in loops
simple arithmetic dispatch
```

This keeps the example compact.

A production-quality calculator would likely introduce additional abstractions once the feature set grows, but doing so here would obscure the core Qt concepts.

---

# 90. What you should notice as a C++ learner

This code is useful because it shows that C++ is not just about syntax. It combines:

```text
C++ object model
        +
RAII / object lifetime concepts
        +
Pointers and dynamic allocation
        +
Inheritance
        +
Member state
        +
Function calls
        +
Qt's QObject/meta-object system
        +
Event-driven GUI programming
```

The Qt framework supplies a large amount of infrastructure, but the application logic is still ordinary C++.

For example:

```cpp
if (pendingOperator == "+")
    result = currentValue + operand;
```

is ordinary C++.

Whereas:

```cpp
connect(btn, &QPushButton::clicked,
        this, &Calculator::onButtonClicked);
```

is C++ syntax being used with Qt's signal/slot framework.

---

# 91. The single most important distinction: framework vs application logic

When learning Qt, avoid mentally treating everything as equally "special."

## Mostly ordinary C++

```cpp
if
else
for
int
double
bool
class
constructor
member functions
pointers
return
```

## Qt-specific framework concepts

```cpp
QWidget
QLineEdit
QPushButton
QGridLayout
QApplication
Q_OBJECT
slots
connect
sender
qobject_cast
```

Learning the two layers separately makes Qt much easier to understand.

---

# 92. Final architecture summary

The program can be summarized as this pipeline:

```text
                    STARTUP
                       │
                       ▼
                    main()
                       │
          ┌────────────┴────────────┐
          ▼                         ▼
 QApplication                  Calculator
          │                         │
          │                         ├── QLineEdit display
          │                         ├── QGridLayout
          │                         ├── 16 QPushButtons
          │                         ├── currentValue
          │                         ├── pendingOperator
          │                         └── waitingForOperand
          │
          └──────────────┬──────────┘
                         ▼
                    app.exec()
                         │
                         ▼
                    EVENT LOOP
                         │
                         ▼
                  user clicks button
                         │
                         ▼
                QPushButton::clicked
                         │
                         ▼
               onButtonClicked()
                         │
           ┌─────────────┼─────────────┐
           ▼             ▼             ▼
        number           C          operator/=
           │             │             │
           ▼             ▼             ▼
      update text      reset        state update
                                         │
                                         ▼
                                    calculate()
                                         │
                                         ▼
                                   update result
```

The calculator is therefore a small **event-driven state machine implemented as a Qt `QWidget` subclass**.

---

# 93. The code's core algorithm in pseudocode

Ignoring all Qt syntax, the logical algorithm is approximately:

```text
START
    display = "0"
    currentValue = 0
    pendingOperator = none
    waitingForOperand = true

WHEN BUTTON IS CLICKED
    identify clicked button

    IF button is a digit
        IF waitingForOperand
            display = digit
            waitingForOperand = false
        ELSE
            append digit to display

    ELSE IF button is C
        reset display and all state

    ELSE IF button is =
        calculate
        clear pending operator
        wait for next operand

    ELSE IF button is an operator
        IF an operator is already pending
            calculate previous operation
        ELSE
            currentValue = numeric value of display

        pendingOperator = clicked operator
        waitingForOperand = true

CALCULATE
    operand = numeric value of display

    IF operator is +
        result = currentValue + operand
    ELSE IF operator is -
        result = currentValue - operand
    ELSE IF operator is *
        result = currentValue * operand
    ELSE IF operator is /
        IF operand == 0
            show Error
            reset input/operator state
            stop
        ELSE
            result = currentValue / operand

    display = formatted result
    currentValue = result
```

This pseudocode is the application logic hidden underneath the C++/Qt syntax.

---

# 94. What to learn next from this code

To become comfortable reading this as a C++ programmer, the next concepts worth mastering are:

1. **Pointers and references** — especially `T*`, `this`, dereferencing, and ownership.
2. **Constructor initializer lists** — including initialization order and base-class initialization.
3. **Inheritance and virtual dispatch** — why a `Calculator` can be used as a `QWidget`.
4. **Member-function pointers** — the `&QPushButton::clicked` and `&Calculator::onButtonClicked` syntax.
5. **RAII and object lifetime** — automatic objects versus dynamically allocated Qt objects.
6. **Qt's QObject parent-child ownership model**.
7. **Signals and slots** — the core Qt event mechanism.
8. **The Qt event loop** — how GUI applications remain interactive.
9. **QString and Qt container/string APIs**.
10. **CMake + Qt build systems** — especially `AUTOMOC`, `AUTOUIC`, and `AUTORCC` in larger projects.

Once those are familiar, this calculator becomes a fairly straightforward combination of C++ and Qt rather than a collection of unrelated syntax.

---

# 95. Final professional interpretation

The most accurate high-level description of this program is:

> A `QWidget`-derived calculator application that constructs a button-driven GUI using Qt Widgets, connects all buttons to a single event handler through Qt's signal/slot mechanism, maintains calculator state in persistent member variables, and performs immediate sequential arithmetic through a dedicated `calculate()` method.

The key architectural relationships are:

```text
QApplication
    ↓
Event loop
    ↓
Calculator (QWidget)
    ↓
Qt widgets + layout
    ↓
button click signals
    ↓
onButtonClicked()
    ↓
calculator state transitions
    ↓
calculate()
    ↓
updated numeric state + display
```

The most important C++ lesson is that the **object persists while individual event-handler calls come and go**. The most important Qt lesson is that the GUI is **event-driven**: widgets emit signals, the application connects those signals to behavior, and the event loop drives the process.

That combination is the foundation on which much larger Qt C++ applications are built.
