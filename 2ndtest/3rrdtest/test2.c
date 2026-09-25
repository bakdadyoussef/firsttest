int pos = 0;
  for (int row =1; row <= 4; ++row) {
    for (int col =0; col < 4; ++col) {
   QPushButton *btn = new QPushButtons(buttons[pos], this);
       btn->setMinimumSize(60,50)
       btn->setStyleSheet("font size: 18px ");
       layout->addWidget(btn,row,col);
connect(btn ,&QPushButton::onclicked, this,&&Calculator::onButtonClicked);
    }
  }
++pos
  QGridLayout(layout);
  currentValue = 0.0;
pendingOperator ="";
waitingForOperator = faslse;
  private slots:
void on buttoncliced() :
QPushButton *btn = qobject_cast<QPushButton*>(sender());
if (!btn) return;
QString text = btn->text();
if (text >= "0" && text<= "9") {
 if (waitingForOperand)
   display->setText(text);
  waitingForOperand= true
    else 
    display->setText(text);
else if (text == "C")
  currentValue = 0;
pendingOperator = "";
waitingForOperand = false;
display->settext("0");
else if (text == "=")
calculate()
  pendingOperator= "";
waitingFOrOperand = true;
display->setText(text);
else if (!operand
  if (operand.isempty()
  display->setText(text().todoubnle())
  








    
