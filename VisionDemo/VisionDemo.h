#pragma once

#define EFE_FORMAT

#include <QtWidgets/QWidget>
#include <qlabel>
#include <highgui.hpp>
#include "ui_VisionDemo.h"

class VisionDemo : public QWidget
{
    Q_OBJECT

public:
    VisionDemo(QWidget *parent = Q_NULLPTR);
	virtual ~VisionDemo();
private slots:

private:
    Ui::VisionDemoClass ui;
};
