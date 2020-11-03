#pragma once

#include <QLabel>
#include <QMouseEvent>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QImage>
#include <QPoint>
#include <QRect>
#include <QMenu>
#include <QDebug>
#include <QAction>
#include <QString>
#include <QCursor>

class QLabel_Mouse : public QLabel
{
	Q_OBJECT

private:
	cv::Point startPoint;
	cv::Point endPoint;
    cv::Point startPoint2;
    cv::Point endPoint2;
    bool getFirstArea = false;
    bool getSecondArea = false;

	cv::Point pos;
	QPoint m_ptPress;
	QRect m_areaMovable;

	bool showValue = false;
	int boundaryLength = 0;
	float ratio = 0.0;
	bool m_bPressed;
    bool m_drawArea;
	bool y_boundary = false;

	QMenu *saveMenu;
	QImage Mat2QImage(cv::Mat &mat);
	void createMeau();
	bool actived = false;
    void calcXY(int &x, int &y);

signals:
	//void sendRange(cv::Point sp, cv::Point ep);
	void sendPos(int x,int y);
	void moveWindow(QPoint loc);
	void maxWindow();
	void signal_savedata(QString at);
    void sendArea(int x1,int y1, int x2, int y2,int flag);

private slots:
	void setImage(cv::Mat mat);
	void loadDefualtPNG();
	void actionsSlot();

protected:
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void mouseDoubleClickEvent(QMouseEvent * event);
	virtual void contextMenuEvent(QContextMenuEvent *event);
public:
	QLabel_Mouse(QWidget *parent = nullptr);
	~QLabel_Mouse();
};
