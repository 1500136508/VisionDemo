#pragma once

#define EFE_FORMAT

#include <QtWidgets/QWidget>
#include <qlabel>
#include <highgui.hpp>
#include <memory.h>
#include <Mutex>
#include "datasaver.h"
#include "ui_VisionDemo.h"

class VisionDemo : public QWidget
{
    Q_OBJECT

public:
    VisionDemo(QWidget *parent = Q_NULLPTR);
	virtual ~VisionDemo();
private slots:
	void on_ShowImage_clicked();
private:
	void buildConnect();
	int thread_camera();
	void read_pd_data();
private:
    Ui::VisionDemoClass ui;

	std::shared_ptr<QLabel> spLabImage;

	int frameHeight = 400;
	int frameWidth = 640;
#ifdef EFE_FORMAT
	int frameHeightR = 480;
	int frameWidthR = 848;
#else
	int frameHeightR = 400;
	int frameWidthR = 640;
#endif

	uchar *datagroup;
	cv::Mat RGBFrame;
	dataSaver *dsaver;

	std::mutex m_mutex;
};
