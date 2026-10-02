#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QLineEdit>
#include <QString>
class Calculator : public QWidget {
Q_OBJECT
public:
CalcuLator(QWidget *parent = nullptr) :QWidget(parent){
setWindowTitle("s")
setFIxedSize(600.500)
  display= new QLineEdit(this);
display->setReadOnly(true);
display->setAlignment(Qt :Alignright);
display->setText("0");
display->setStyleSHeet("fontsize: 24px; padding: 10px");
QGridLayout *layout =new QGridLayout(this):
layout->addWidget(display,0.0.1.4);
//buttons
const char*  buttons[16]
