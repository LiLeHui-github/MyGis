#pragma once

#include <QtWidgets/QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MyGisWidgetClass; };
QT_END_NAMESPACE

class MyGisWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit MyGisWidget(QWidget *parent = nullptr);
    ~MyGisWidget() override;

private:
    Ui::MyGisWidgetClass *ui;
};
