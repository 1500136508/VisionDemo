#include <thread>
#include <opencv.hpp>
#include <QDebug>
#include <QMessageBox>
#include "VisionDemo.h"
#include "CameraDS.h"
#include "../dense_tencent/dsense_interface.h"


VisionDemo::VisionDemo(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

	/*setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);*/
	m_areaMovable = QRect(0, 0, this->size().width(), 30);
	m_bPressed = false;

	qRegisterMetaType<cv::Mat>("cv::Mat");
	qRegisterMetaType<std::string>("std::string");

	init();
	buildConnect();

	this->setMouseTracking(true);

	bottonSetStyleSheet();
	flashCam();
	setModuleIndexSlot();
	ui.label->setVisible(false);
	ui.label_2->setVisible(false);
	ui.saveButton2->setVisible(false);
	ui.comboBoxModule->setVisible(true);
	setIRShow();
	setRGBShow();
	setDEPTHShow();
	setRDShow();
}

VisionDemo::~VisionDemo()
{
	sDialog->close();
	imgRdr->release();
}

void VisionDemo::mousePressEvent(QMouseEvent *e)
{
	//鼠标左键
	if (e->button() == Qt::LeftButton)
	{
		m_ptPress = e->pos();
		m_areaMovable = QRect(0, 0, this->size().width(), 30);
		m_bPressed = m_areaMovable.contains(m_ptPress);
	}
}

void VisionDemo::mouseMoveEvent(QMouseEvent *e)
{
	if (m_bPressed)
	{
		move(pos() + e->pos() - m_ptPress);
	}
}

void VisionDemo::mouseReleaseEvent(QMouseEvent *)
{
	m_bPressed = false;
}

void VisionDemo::mouseDoubleClickEvent(QMouseEvent * event)
{
	m_areaMovable = QRect(0, 0, this->size().width(), 30);
	if (event->button() == Qt::LeftButton && m_areaMovable.contains(event->pos())) {
		maxWindow();
	}
}

bool VisionDemo::nativeEvent(const QByteArray & eventType, void * message, long * result)
{
	int g_nBorder = 5;
	MSG* pMsg = (MSG*)message;
	switch (pMsg->message)
	{
	case WM_NCHITTEST:
	{
		QPoint pos = mapFromGlobal(QPoint(LOWORD(pMsg->lParam), HIWORD(pMsg->lParam)));
		bool bHorLeft = pos.x() < g_nBorder;
		bool bHorRight = pos.x() > this->width() - g_nBorder;
		bool bVertTop = pos.y() < g_nBorder;
		bool bVertBottom = pos.y() > this->height() - g_nBorder;
		if (bHorLeft && bVertTop)
		{
			*result = HTTOPLEFT;
		}
		else if (bHorLeft && bVertBottom)
		{
			*result = HTBOTTOMLEFT;
		}
		else if (bHorRight && bVertTop)
		{
			*result = HTTOPRIGHT;
		}
		else if (bHorRight && bVertBottom)
		{
			*result = HTBOTTOMRIGHT;
		}
		else if (bHorLeft)
		{
			*result = HTLEFT;
		}
		else if (bHorRight)
		{
			*result = HTRIGHT;
		}
		else if (bVertTop)
		{
			*result = HTTOP;
		}
		else if (bVertBottom)
		{
			*result = HTBOTTOM;
		}
		else
		{
			return false;
		}
		return true;
	}
	break;
	default:
		break;
	}

	return QWidget::nativeEvent(eventType, message, result);
}

void VisionDemo::buildConnect()
{
	connect(ui.closeButton, SIGNAL(clicked()), this, SLOT(shutDownWindow()));
	connect(ui.minButton, SIGNAL(clicked()), this, SLOT(minWindow()));
	connect(ui.showLabel, SIGNAL(moveWindow(QPoint)), this, SLOT(moveWindow(QPoint)));
	connect(ui.showLabel, SIGNAL(maxWindow()), this, SLOT(maxWindow()));
	connect(this, SIGNAL(sendImage(cv::Mat)), ui.showLabel, SLOT(setImage(cv::Mat)));
	connect(imgRdr, SIGNAL(sendImage(cv::Mat)), ui.showLabel, SLOT(setImage(cv::Mat)));
	connect(this, SIGNAL(sendDisconnet()), ui.showLabel, SLOT(loadDefualtPNG()));
	connect(ui.modeButton, SIGNAL(clicked()), this, SLOT(runCamera()));
	connect(ui.optionButton, SIGNAL(clicked()), this, SLOT(showSaveStart()));
	connect(ui.saveButton2, SIGNAL(clicked()), this, SLOT(showSaveStart()));
	connect(imgRdr, SIGNAL(sendSaveDone()), this, SLOT(showSaveDone()));
	connect(ui.showLabel, SIGNAL(sendPos(int, int)), imgRdr, SLOT(acceptLocation(int, int)));
	connect(ui.showLabel,SIGNAL(moveWindow(QPoint)), this, SLOT(moveWindow(QPoint)));
	connect(imgRdr, SIGNAL(sendLocationDepth(int, int, float)), this, SLOT(showPointDepth(int, int, float)));

	connect(ui.showLabel, SIGNAL(sendArea(int, int, int, int, int)), imgRdr, SLOT(acceptArea(int, int, int, int, int)));
	connect(ui.showLabel,SIGNAL(moveWindow(QPoint)), this, SLOT(moveWindow(QPoint)));
	connect(imgRdr, SIGNAL(sendAreaAvg(float, float)), this, SLOT(showAvgDepth(float, float)));

	connect(imgRdr, SIGNAL(sendFrameRate(QString, QString, QString)), this, SLOT(showFrameRate(QString, QString, QString)));

	connect(imgRdr, SIGNAL(sendStopSignal(bool)), this, SLOT(showstatus(bool)));
	connect(this, SIGNAL(sendCloseEven()), imgRdr, SLOT(stopingProgram()));

	connect(ui.comboBoxModule, SIGNAL(currentTextChanged(QString)), this, SLOT(setModuleIndexSlot()));
	connect(this, SIGNAL(setModuleIndex(QString)), imgRdr, SLOT(readXMLData(QString)));

	connect(ui.optionButton, SIGNAL(clicked()), this, SLOT(saveSlot0()));
	connect(ui.saveButton2, SIGNAL(clicked()), this, SLOT(saveSlot1()));

	 connect(this, SIGNAL(sendSaveDataSignal(int)), imgRdr, SLOT(saveDataSlot(int)));

	connect(imgRdr, SIGNAL(sendLostServer(bool)), this, SLOT(cameraInitFlag(bool)));

	connect(ui.checkBoxIR, SIGNAL(stateChanged(int)), this, SLOT(setIRShow()));
	connect(ui.checkBoxRGB, SIGNAL(stateChanged(int)), this, SLOT(setRGBShow()));
	connect(ui.checkBoxDepth, SIGNAL(stateChanged(int)), this, SLOT(setDEPTHShow()));
	connect(ui.checkBoxRD, SIGNAL(stateChanged(int)), this, SLOT(setRDShow()));

	connect(this, SIGNAL(sendShowFlag(int, bool)), imgRdr, SLOT(setShowPic(int, bool)));


	connect(sDialog, SIGNAL(sendSavePath(QString)), imgRdr, SLOT(savePathSlot(QString)));
	connect(sDialog, SIGNAL(sendSaveStart(int)), imgRdr, SLOT(saveDataSlot(int)));
	connect(sDialog, SIGNAL(sendSaveStop()), imgRdr, SLOT(saveStopSlot()));
	connect(sDialog, SIGNAL(sendSaveMode(int)), imgRdr, SLOT(saveModeSetSlot(int)));
	connect(imgRdr, SIGNAL(sendSaveInfo(QString)), sDialog, SLOT(showInfo(QString)));


	connect(ui.flashCamBtn, SIGNAL(clicked()), this, SLOT(flashCam()));
}

void VisionDemo::init()
{
	imgRdr = new imageReader(NULL);
	sDialog = new saveDialog(NULL);
}

void VisionDemo::maxWindow()
{
	this->isMaximized() ? this->showNormal() : this->showMaximized();
}

void VisionDemo::moveWindow(QPoint m_pos)
{
	move(pos() + m_pos);
}

void VisionDemo::shutDownWindow()
{
	this->close();
}

void VisionDemo::showPointDepth(int x, int y, float d)
{
	if (x != -1 && y != -1) {
		QString info;
		info.sprintf("    %d, %d", x, y);
		ui.labelCoordinate->setText(" Coordinate");
		ui.labelxy->setText(info);
		info.sprintf("    %.2f\n", d);
		ui.labeldeptht->setText(" Depth");
		ui.labeldepth->setText(info);
	}
	else {
		ui.labelCoordinate->clear();
		ui.labelxy->clear();
		ui.labeldeptht->clear();
		ui.labeldepth->clear();
	}
}

void VisionDemo::minWindow()
{
	this->showMinimized();
}

void VisionDemo::closeEvent(QCloseEvent *event)
{
	sDialog->close();
	emit sendCloseEven();
}

void VisionDemo::bottonSetStyleSheet()
{
	ui.modeButton->setStyleSheet(
		QLatin1String("QPushButton{border-image: url(:/haisi_UI_demo/ppppp/star_click.png);}"
			"QPushButton:hover{border-image: url(:/haisi_UI_demo/ppppp/star_nor.png);}")
	);
	ui.optionButton->setStyleSheet(
		QLatin1String("QPushButton{border-image: url(:/haisi_UI_demo/ppppp/save_click.png);}"
			"QPushButton:hover{border-image: url(:/haisi_UI_demo/ppppp/save_nor.png);}")
	);

	ui.saveButton2->setStyleSheet(
		QLatin1String("QPushButton{border-image: url(:/haisi_UI_demo/ppppp/save_click.png);}"
			"QPushButton:hover{border-image: url(:/haisi_UI_demo/ppppp/save_nor.png);}")
	);


	ui.closeButton->setStyleSheet(
		QString::fromUtf8("QPushButton{image: url(:/haisi_UI_demo/ppppp/btl/Close_normal.png);border-image: url(:/haisi_UI_demo/ppppp/\350\203\214\346\231\257\345\233\276.png);}"
			"QPushButton:hover{image: url(:/haisi_UI_demo/ppppp/btl/Close_hover.png);border-image: url(:/haisi_UI_demo/ppppp/\350\203\214\346\231\257\345\233\276.png);}"
		)
	);
	ui.minButton->setStyleSheet(
		QString::fromUtf8("QPushButton{image: url(:/haisi_UI_demo/ppppp/btl/Minimize_normal.png);border-image: url(:/haisi_UI_demo/ppppp/\350\203\214\346\231\257\345\233\276.png);}"
			"QPushButton:hover{image: url(:/haisi_UI_demo/ppppp/btl/Minimize_hover.png);border-image: url(:/haisi_UI_demo/ppppp/\350\203\214\346\231\257\345\233\276.png);}"
		)
	);
}

void VisionDemo::runCamera()
{
	imgRdr->run(camIndex);
	//    ui.modeButton->setStyleSheet(
	//        QLatin1String("QPushButton{border-image: url(:/haisi_UI_demo/ppppp/stop_click.png);}"
	//            "QPushButton:hover{border-image: url(:/haisi_UI_demo/ppppp/stop_nor.png);}")
	//                );
}

void VisionDemo::stopView()
{
	qDebug() << "stop!";
	//    ui.modeButton->setStyleSheet(
	//        QLatin1String("QPushButton{border-image: url(:/haisi_UI_demo/ppppp/star_click.png);}"
	//            "QPushButton:hover{border-image: url(:/haisi_UI_demo/ppppp/star_nor.png);}")
		//    );
}

void VisionDemo::showAvgDepth(float depth0, float depth1)
{
	QString depthStr;
	depthStr.sprintf("%.2f %.2f", depth0, depth1);
	ui.labelDepthAvg->setText(depthStr);
}

void VisionDemo::showSaveStart()
{
	//    ui.labelArea->setText("Saving...");
}

void VisionDemo::showSaveDone()
{
	//    ui.labelArea->setText("Save Done!");
}

void VisionDemo::showFrameRate(QString irRate, QString depthRate, QString rgbRate)
{
	QString info = "IR:" + irRate + " " + "Depth:" + irRate + " " + "RGB:" + rgbRate;
	ui.labelFrameRate->setText(info);

	FILE* fp = fopen("frameRate.log", "a");
	std::time_t t = std::time(0);
	char buf[128] = { 0 };
	strftime(buf, 64, "%Y-%m-%d %H:%M:%S", localtime(&t));
	std::string timeSTR = std::string(buf);

	std::string writeInfo = "[" + timeSTR + "] " + info.toStdString() + "\n";
	fwrite(writeInfo.c_str(), 1, writeInfo.size(), fp);
	fclose(fp);
}

void VisionDemo::showstatus(bool status)
{
	if (!cameraReady) return;
	if (status)
		ui.labelModeText->setText(QString::fromLocal8Bit("预览进行中"));
	else
		ui.labelModeText->setText(QString::fromLocal8Bit("预览中止"));
}

void VisionDemo::setModuleIndexSlot()
{
	// QString xml = "D:/KnightFish/haisi_UI_demo/haisi_UI_demo/calibResult/W04.xml";
	// QString xml = "calibResult/"+ui.comboBoxModule->currentText()+".xml";
	// emit setModuleIndex(xml);
	camIndex = ui.comboBoxModule->currentText().toInt();
}

void VisionDemo::saveSlot0()
{
	emit sendSaveDataSignal(0);
	sDialog->open();
}

void VisionDemo::saveSlot1()
{
	emit sendSaveDataSignal(1);
}

void VisionDemo::cameraInitFlag(bool camStop)
{
	cameraReady = !camStop;
	if (!cameraReady)
		ui.labelModeText->setText(QString::fromLocal8Bit("摄像头未连接"));
}

void VisionDemo::setIRShow()
{
	if (!ui.checkBoxIR->isChecked() && !ui.checkBoxDepth->isChecked() && !ui.checkBoxRGB->isChecked() && !ui.checkBoxRD->isChecked())
		ui.checkBoxIR->setCheckState(Qt::CheckState::Checked);
	emit sendShowFlag(0, ui.checkBoxIR->isChecked());
}

void VisionDemo::setRGBShow()
{
	if (!ui.checkBoxIR->isChecked() && !ui.checkBoxDepth->isChecked() && !ui.checkBoxRGB->isChecked() && !ui.checkBoxRD->isChecked())
		ui.checkBoxRGB->setCheckState(Qt::CheckState::Checked);
	emit sendShowFlag(2, ui.checkBoxRGB->isChecked());
}

void VisionDemo::setDEPTHShow()
{
	if (!ui.checkBoxIR->isChecked() && !ui.checkBoxDepth->isChecked() && !ui.checkBoxRGB->isChecked() && !ui.checkBoxRD->isChecked())
		ui.checkBoxDepth->setCheckState(Qt::CheckState::Checked);
	emit sendShowFlag(1, ui.checkBoxDepth->isChecked());
}

void VisionDemo::setRDShow()
{
	if (!ui.checkBoxIR->isChecked() && !ui.checkBoxDepth->isChecked() && !ui.checkBoxRGB->isChecked() && !ui.checkBoxRD->isChecked())
		ui.checkBoxRD->setCheckState(Qt::CheckState::Checked);
	emit sendShowFlag(3, ui.checkBoxRD->isChecked());
}

void VisionDemo::flashCam()
{
	int cameraNum = CCameraDS::CameraCount();
	ui.comboBoxModule->clear();
	char camName[100];
	for (int i = 0; i < cameraNum; i++) {
		CCameraDS::CameraName(i, camName, 100);
		std::string cN(camName);
		if (cN.find("UVC") != std::string::npos) ui.comboBoxModule->addItem(QString::number(i));
	}
	if (ui.comboBoxModule->count() == 0) {
		QMessageBox::critical(NULL, "ERROR", "No Camera Found", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes); camIndex = -1; return;
	}
}

