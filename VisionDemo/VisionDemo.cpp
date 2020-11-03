#include <thread>
#include <opencv.hpp>
#include "VisionDemo.h"
#include "CameraDS.h"

VisionDemo::VisionDemo(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
}

VisionDemo::~VisionDemo()
{
}
