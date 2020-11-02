#pragma once

#include <QtWidgets/QWidget>
#include "ui_VisionDemo.h"

class VisionDemo : public QWidget
{
    Q_OBJECT

public:
    VisionDemo(QWidget *parent = Q_NULLPTR);

private:
    Ui::VisionDemoClass ui;
};
