#include "qlabel_mouse.h"
#include <QPixmap>
#include <QSize>
#include <QDebug>
#include <time.h>

static int INPUTWIDTH = 480;
static int INPUTHEIGHT = 848;


void QLabel_Mouse::loadDefualtPNG()
{
	actived = false;
	QString filename = "styles\\disconnect.png";
	QImage img;
	img.load(filename);
	this->setScaledContents(false);
	this->setPixmap(QPixmap::fromImage(img));

}

void QLabel_Mouse::mouseMoveEvent(QMouseEvent * event)
{
    if (m_bPressed)
    {
        emit moveWindow(event->pos() - m_ptPress);
    }
    if (!this->hasMouseTracking())
        return;
    int x = event->localPos().x();
    int y = event->localPos().y();
    calcXY(x,y);
    emit sendPos(x,y);
    if (startPoint.x!=-1 && m_drawArea && !getFirstArea){
        endPoint.x = x;
        endPoint.y = y;
        if (startPoint.x < INPUTWIDTH && endPoint.x > INPUTWIDTH) {
            endPoint.x = INPUTWIDTH;
        }
        else if (startPoint.x < INPUTWIDTH * 2 && endPoint.x > INPUTWIDTH * 2){
            endPoint.x = INPUTWIDTH * 2;
        }
        else if (startPoint.x < INPUTWIDTH * 3 && endPoint.x > INPUTWIDTH * 3){
            endPoint.x = INPUTWIDTH * 3;
        }
        else if (startPoint.x > INPUTWIDTH && endPoint.x < INPUTWIDTH){
            endPoint.x  = INPUTWIDTH;
        }
        else if (startPoint.x > INPUTWIDTH * 2 && endPoint.x < INPUTWIDTH * 2){
            endPoint.x = INPUTWIDTH * 2;
        }
    }

    if (startPoint2.x!=-1 && m_drawArea && getFirstArea && !getSecondArea){
        endPoint2.x = x;
        endPoint2.y = y;
        if (startPoint2.x < INPUTWIDTH && endPoint2.x > INPUTWIDTH) {
            endPoint2.x = INPUTWIDTH;
        }
        else if (startPoint2.x < INPUTWIDTH * 2 && endPoint2.x > INPUTWIDTH * 2){
            endPoint2.x = INPUTWIDTH * 2;
        }
        else if (startPoint.x < INPUTWIDTH * 3 && endPoint2.x > INPUTWIDTH * 3){
            endPoint2.x = INPUTWIDTH * 3;
        }
        else if (startPoint.x > INPUTWIDTH && endPoint2.x < INPUTWIDTH){
            endPoint2.x  = INPUTWIDTH;
        }
        else if (startPoint.x > INPUTWIDTH * 2 && endPoint2.x < INPUTWIDTH * 2){
            endPoint2.x = INPUTWIDTH * 2;
        }
    }
}

void QLabel_Mouse::mousePressEvent(QMouseEvent * event)
{
	//startPoint.x = MIN(event->localPos().x() * 2, 800);
	//startPoint.y = MIN(event->localPos().y() * 2, 1280);
	//printf("start point %d-%d \n", startPoint.x, startPoint.y);
    if (event->button() == Qt::LeftButton)
    {
        m_ptPress = event->pos();
        m_areaMovable = QRect(0, 0, this->size().width(), 30);
        m_bPressed = m_areaMovable.contains(m_ptPress);
        m_drawArea = !m_bPressed;
        if (!getFirstArea){
            startPoint.x = event->localPos().x();
            startPoint.y = event->localPos().y();
            if (startPoint.y < 30){
                startPoint.x = startPoint.y = -1;
                return;
            }
            calcXY(startPoint.x, startPoint.y);
            //if (startPoint.x < 400 || startPoint.x > 800) return;
            endPoint.x = startPoint.x;
            endPoint.y = startPoint.y;
        }
        else if (!getSecondArea) {
            startPoint2.x = event->localPos().x();
            startPoint2.y = event->localPos().y();
            calcXY(startPoint2.x, startPoint2.y);
            //if (startPoint2.x < 400 || startPoint2.x > 800) return;
            endPoint2.x = startPoint2.x;
            endPoint2.y = startPoint2.y;
        }
    }
    else if (event->button() == Qt::RightButton){
        startPoint.x = startPoint.y = endPoint.x = endPoint.y = -1;
        startPoint2.x = startPoint2.y = endPoint2.x = endPoint2.y = -1;
        emit sendArea(startPoint2.x,startPoint2.y,endPoint2.x,endPoint2.y, 0);
    }
}

void QLabel_Mouse::mouseReleaseEvent(QMouseEvent * event)
{
	//endPoint.x = MIN(event->localPos().x() * 2, 800);
	//endPoint.y = MIN(event->localPos().y() * 2, 1280);
	//emit sendRange(startPoint, endPoint);
	//printf("end point %d-%d \n", endPoint.x, endPoint.y);
    m_bPressed = false;
    m_drawArea = false;
    if (!getFirstArea){
        emit sendArea(startPoint.x,startPoint.y,endPoint.x,endPoint.y, 0);
        getFirstArea=true;
    }
    else if (!getSecondArea){
        emit sendArea(startPoint2.x,startPoint2.y,endPoint2.x,endPoint2.y, 1);
        getSecondArea=true;
    }
    else{
        startPoint.x = startPoint.y = endPoint.x = endPoint.y = -1;
        startPoint2.x = startPoint2.y = endPoint2.x = endPoint2.y = -1;
        getFirstArea = getSecondArea = false;
        emit sendArea(startPoint2.x,startPoint2.y,endPoint2.x,endPoint2.y, 0);
    }
}

void QLabel_Mouse::mouseDoubleClickEvent(QMouseEvent * event)
{
    m_areaMovable = QRect(0, 0, this->size().width(), 30);
    if (event->button() == Qt::LeftButton && m_areaMovable.contains(event->pos())) {
        emit maxWindow();
    }
}

QLabel_Mouse::QLabel_Mouse(QWidget *parent)
	: QLabel(parent)
{
	loadDefualtPNG();
	m_areaMovable = QRect(0, 0, this->size().width(), 30);
	m_bPressed = false;
    m_drawArea = false;
    startPoint.x = startPoint.y = endPoint.x = endPoint.y = -1;
    startPoint2.x = startPoint2.y = endPoint2.x = endPoint2.y = -1;
    //createMeau();
}



QLabel_Mouse::~QLabel_Mouse()
{

}

QImage QLabel_Mouse::Mat2QImage(cv::Mat &mat)
{
	if (mat.type() == CV_8UC1 || mat.type() == CV_32FC1)
	{
		QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
		image.setColorCount(256);
		for (int i = 0; i < 256; i++)
		{
			image.setColor(i, qRgb(i, i, i));
		}
		uchar *pSrc = mat.data;
		for (int row = 0; row < mat.rows; row++)
		{
			uchar *pDest = image.scanLine(row);
			memcpy(pDest, pSrc, mat.cols);
			pSrc += mat.step;
		}
		return image;
	}
	else if (mat.type() == CV_8UC3)
	{
		const uchar *pSrc = (const uchar*)mat.data;
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
		return image.rgbSwapped();
	}
	else if (mat.type() == CV_8UC4)
	{
		const uchar *pSrc = (const uchar*)mat.data;
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
		return image.copy();
	}
	else
	{
		return QImage();
	}
}


void QLabel_Mouse::setImage(cv::Mat mat) {
    actived = false;
    clock_t t = clock();
    QSize labelSize = this->size();
	int labelHeight = labelSize.height();
	int labelWidth = labelSize.width();

    if (startPoint.x != -1){
        cv::rectangle(mat,startPoint,endPoint,cv::Scalar(0,128,255),2);
    }
    if (startPoint2.x != -1){
        cv::rectangle(mat,startPoint2,endPoint2,cv::Scalar(128,255,255),2);
    }

    cv::Mat biggerMat = cv::Mat::zeros(labelHeight, labelWidth, CV_8UC3);

	int matWidth = mat.cols;
	int matHeight = mat.rows;

	float ratio1 = labelWidth * 1.0 / matWidth;
	float ratio2 = labelHeight * 1.0 / matHeight;

	if (matHeight * ratio1 < labelHeight) {
		ratio = ratio1;
		boundaryLength = (labelHeight - int(mat.rows * ratio)) / 2;
		y_boundary = true;
		cv::resize(mat, mat, cv::Size(0, 0), ratio, ratio);

        mat.copyTo(biggerMat.rowRange(boundaryLength,boundaryLength+mat.rows));
	}
	else {
		ratio = ratio2;

		boundaryLength = (labelWidth - int(mat.cols * ratio)) / 2;
		cv::resize(mat, mat, cv::Size(0, 0), ratio, ratio);
		y_boundary = false;

        mat.copyTo(biggerMat.colRange(boundaryLength,boundaryLength+mat.cols));
	}
	QPixmap pixMap = QPixmap::fromImage(this->Mat2QImage(biggerMat));
    clock_t t2 = clock();
    this->setPixmap(pixMap);
    clock_t t3 = clock();

    //qDebug() << t3 - t2 << t2 - t;
}


void QLabel_Mouse::createMeau()
{
//	saveMenu = new QMenu();
//	saveMenu->addAction(QString::fromLocal8Bit("保存红外图"), this, SLOT(actionsSlot()));
//    //saveMenu->addAction(QString::fromLocal8Bit("保存RGB图"), this, SLOT(actionsSlot()));
//    //saveMenu->addAction(QString::fromLocal8Bit("保存RGB图(YUY2)"), this, SLOT(actionsSlot()));
//	saveMenu->addAction(QString::fromLocal8Bit("保存深度图"), this, SLOT(actionsSlot()));
//    //saveMenu->addAction(QString::fromLocal8Bit("保存点云"), this, SLOT(actionsSlot()));
//    //saveMenu->addAction(QString::fromLocal8Bit("全部保存"), this, SLOT(actionsSlot()));
//    //saveMenu->addAction(QString::fromLocal8Bit("连续保存"), this, SLOT(actionsSlot()));
}

void QLabel_Mouse::calcXY(int &x, int &y)
{
    QSize labelSize = this->size();
    int labelHeight = labelSize.height();
    int labelWidth = labelSize.width();
    if (y_boundary) {
        if (y < boundaryLength || y >= labelHeight - boundaryLength) {
            x = y = 0;
        }
        else {
            y = (y - boundaryLength) / ratio;
            x = x / ratio;
            x = x;
        }
    }
    else {

        if (x < boundaryLength || x >= labelWidth - boundaryLength) {
            x = y = 0;
        }
        else {
            x = (x - boundaryLength) / ratio;
            y = y / ratio;
            x = x;
        }
    }
}



void QLabel_Mouse::contextMenuEvent(QContextMenuEvent *event)
{
//	if (actived) {
//		saveMenu->exec(QCursor::pos());
//		event->accept();
//	}
}


void QLabel_Mouse::actionsSlot()
{
	QAction *action = (QAction*)sender();
	QString actionText = action->text();
	qDebug() << actionText << " triggerred!";
	emit signal_savedata(actionText);
}
