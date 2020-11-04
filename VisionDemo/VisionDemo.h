#pragma once

#define EFE_FORMAT

#include <QtWidgets/QWidget>
#include <qlabel>
#include <highgui.hpp>
#include <memory.h>
#include <Mutex>
#include "datasaver.h"
#include "imagereader.h"
#include "saveDialog.h"
#include "ui_VisionDemo.h"

class VisionDemo : public QWidget
{
    Q_OBJECT

public:
    VisionDemo(QWidget *parent = Q_NULLPTR);
	virtual ~VisionDemo();

private:
    Ui::VisionDemoClass ui;

	QRect m_areaMovable;
	QPoint m_ptPress;
	bool m_bPressed;
	bool cameraReady = false;
	imageReader *imgRdr;
	saveDialog* sDialog;
	bool readDataStart = false;
	int camIndex = -1;
protected:

	// 界面设置及移动，鼠标动作函数
	void bottonSetStyleSheet();
	void closeEvent(QCloseEvent *event);
	void mousePressEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void mouseDoubleClickEvent(QMouseEvent *event);
	virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);

	// 变量初始化
	void init();
	// 信号-槽-链接
	void buildConnect();


private slots:
	// 窗口功能函数
	void moveWindow(QPoint m_pos);
	void shutDownWindow();
	void showPointDepth(int x, int y, float d);
	void minWindow();
	void maxWindow();

	// 摄像头运行
	void runCamera();
	// 停止画面设置
	void stopView();
	// 显示区域平均深度
	void showAvgDepth(float depth0, float depth1);
	// 显示开始保存
	void showSaveStart();
	// 显示完成保存
	void showSaveDone();
	// 显示帧率
	void showFrameRate(QString irRate, QString depthRate, QString rgbRate);
	// 显示预览状态
	void showstatus(bool status);
	// 设置模组序号
	void setModuleIndexSlot();
	// 保存响应0
	void saveSlot0();
	// 保存响应1
	void saveSlot1();
	// 显示摄像头状态
	void cameraInitFlag(bool camStop);
	// 设置显示IR
	void setIRShow();
	// 设置显示RGB
	void setRGBShow();
	// 设置显示深度
	void setDEPTHShow();
	// 设置显示符合图像
	void setRDShow();
	// 检测摄像头
	void flashCam();

signals:
	void sendImage(cv::Mat mat);
	void sendDisconnet();
	void sendCloseEven();
	void setModuleIndex(QString);
	void sendSaveDataSignal(int);
	void sendShowFlag(int, bool);
};
