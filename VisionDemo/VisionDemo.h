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

	// �������ü��ƶ�����궯������
	void bottonSetStyleSheet();
	void closeEvent(QCloseEvent *event);
	void mousePressEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void mouseDoubleClickEvent(QMouseEvent *event);
	virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);

	// ������ʼ��
	void init();
	// �ź�-��-����
	void buildConnect();


private slots:
	// ���ڹ��ܺ���
	void moveWindow(QPoint m_pos);
	void shutDownWindow();
	void showPointDepth(int x, int y, float d);
	void minWindow();
	void maxWindow();

	// ����ͷ����
	void runCamera();
	// ֹͣ��������
	void stopView();
	// ��ʾ����ƽ�����
	void showAvgDepth(float depth0, float depth1);
	// ��ʾ��ʼ����
	void showSaveStart();
	// ��ʾ��ɱ���
	void showSaveDone();
	// ��ʾ֡��
	void showFrameRate(QString irRate, QString depthRate, QString rgbRate);
	// ��ʾԤ��״̬
	void showstatus(bool status);
	// ����ģ�����
	void setModuleIndexSlot();
	// ������Ӧ0
	void saveSlot0();
	// ������Ӧ1
	void saveSlot1();
	// ��ʾ����ͷ״̬
	void cameraInitFlag(bool camStop);
	// ������ʾIR
	void setIRShow();
	// ������ʾRGB
	void setRGBShow();
	// ������ʾ���
	void setDEPTHShow();
	// ������ʾ����ͼ��
	void setRDShow();
	// �������ͷ
	void flashCam();

signals:
	void sendImage(cv::Mat mat);
	void sendDisconnet();
	void sendCloseEven();
	void setModuleIndex(QString);
	void sendSaveDataSignal(int);
	void sendShowFlag(int, bool);
};
