#include <QApplication>
#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QString>
class Calculator : public QWidget {
Q_Object
public:
Calculator(QWidget *parent = nullptr) : QWidget(parent) {
  setWindowTitle("Simple");
  setFixedSize(600,500);
  display =new QLineEdit(this);
  display->setReadOnly(true);
  display->setAlignment(Qt :AlignRight);
  display->setText("0")
 display->setStyleSheet("fontsize: 24px; padding: 18px ");
 QGridLayout *layout = new QGridLayout(this);
layout->addWidget(display,0 ,0, 1, 4);
//buttons
