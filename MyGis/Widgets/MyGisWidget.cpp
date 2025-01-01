#include "MyGisWidget.h"

#include "ui_MyGisWidget.h"

MyGisWidget::MyGisWidget(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MyGisWidgetClass())
{
    ui->setupUi(this);
}

MyGisWidget::~MyGisWidget()
{
    delete ui;
}
