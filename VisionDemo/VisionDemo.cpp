#include <thread>
#include <opencv.hpp>
#include <QDebug>
#include "VisionDemo.h"
#include "CameraDS.h"
#include "../dense_tencent/dsense_interface.h"

cv::Mat K_rgb = cv::Mat::zeros(3, 3, CV_32FC1);
cv::Mat R = cv::Mat::zeros(3, 3, CV_32FC1);
cv::Mat K_ir = cv::Mat::zeros(3, 3, CV_32FC1);
cv::Mat T = cv::Mat::zeros(3, 3, CV_32FC1);
cv::Mat tmpM = cv::Mat::zeros(3, 3, CV_32FC1);

ir_rgb_State rgb_param{ 0 };

unsigned char PD[1024];
float realpd[30];

VisionDemo::VisionDemo(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

	spLabImage = std::make_shared<QLabel>(new QLabel());

	RGBFrame = cv::Mat(cv::Size(frameHeightR, frameWidthR), CV_8UC3);
	datagroup = new uchar[frameWidth*frameHeight * 4 + frameWidthR * frameHeightR * 2];

	buildConnect();
}

VisionDemo::~VisionDemo()
{
}

void VisionDemo::buildConnect()
{
	connect(ui.m_btn_ShowImg, SIGNAL(clicked(bool)), this, SLOT(on_ShowImage_clicked()));
}

int VisionDemo::thread_camera()
{
	int cam_count;

	//仅仅获取摄像头数目
	cam_count = CCameraDS::CameraCount();
	printf("There are %d cameras.\n", cam_count);


	//获取所有摄像头的名称
	for (int i = 0; i < cam_count; i++)
	{
		char camera_name[1024];
		int retval = CCameraDS::CameraName(i, camera_name, sizeof(camera_name));

		if (retval > 0)
			printf("Camera #%d's Name is '%s'.\n", i, camera_name);
		else
			printf("Can not get Camera #%d's name.\n", i);
	}


	if (cam_count == 0)
		return -1;

	CCameraDS camera;

	//打开第一个摄像头
	//if(! camera.OpenCamera(0, true)) //弹出属性选择窗口
	if (!camera.OpenCamera(0, 640, 480)) //不弹出属性选择窗口，用代码制定图像宽和高
	{
		fprintf(stderr, "Can not open camera.\n");
		return -1;
	}


	//cvNamedWindow("camera");
	while (1)
	{
		try
		{
			//获取一帧
			//IplImage *pFrame = camera.QueryFrame();
			//cv::Mat m = cv::cvarrToMat(pFrame);
			//imwrite("test1.tiff", m);

			//显示
			//cvShowImage("camera", pFrame);

			bool getParam = false;
			long long irT = 0;
			long long rgbT = 0;
			long long depthT = 0;
			long long lastRgbT = 0;

			memset(datagroup, 0, 640 * 480 * 2);
			camera.readRawData(datagroup);
			if (!getParam)
			{
				m_mutex.lock();
				memcpy(PD, datagroup + 640 * 480 * 2 - 1032, 1024);
				read_pd_data();
				getParam = true;
				m_mutex.unlock();
			}

			bool thisRoundIR = true;
			uchar* ptr = datagroup + frameHeight * frameWidth * 2;
			memcpy(&rgbT, datagroup + 640 * 480 * 2 - 8, sizeof(long long));
			if (rgbT == lastRgbT) continue;
			else lastRgbT = rgbT;
			cv::Mat rgbyuv(cv::Size(frameWidthR, frameHeightR), CV_8U, ptr);
			cv::imwrite("test.tiff", rgbyuv);
			RGBFrame = cv::imdecode(rgbyuv, CV_LOAD_IMAGE_COLOR);
			cv::transpose(RGBFrame, RGBFrame);
			cv::flip(RGBFrame, RGBFrame, 0);
			cv::flip(RGBFrame, RGBFrame, -1);
			//RGBFrame.copyTo(container[2]);
			cv::imwrite("test.tiff", RGBFrame);

			if (cvWaitKey(20) == 'q')
				break;
		}
		catch (cv::Exception &e)
		{
			const char *err_msg = e.what();
			break;
		}
		catch (...)
		{
			break;
		}
	}
	camera.CloseCamera(); //可不调用此函数，CCameraDS析构时会自动关闭摄像头

	//cvDestroyWindow("camera");
	return 0;
}

void VisionDemo::read_pd_data()
{
	unsigned char *ptr = PD + 16;
	int datalen = 30;
	int i = 0;
	while (i < datalen) {
		memcpy(&realpd[i], ptr, sizeof(float)); ptr += sizeof(float);
		qDebug() << "param " << i << " " << realpd[i];
		i++;
	}
#ifdef EFE_FORMAT
	float ratio = 1080.f / 480.0f;
#else
	float ratio = 2;
#endif
	rgb_param.fxir = realpd[0] / 2;
	rgb_param.fyir = realpd[1] / 2;
	rgb_param.cxir = realpd[2] / 2;
	rgb_param.cyir = realpd[3] / 2;

	rgb_param.fxrgb = realpd[9] / ratio;
	rgb_param.fyrgb = realpd[10] / ratio;
	rgb_param.cxrgb = realpd[11] / ratio;
	rgb_param.cyrgb = realpd[12] / ratio;

	K_rgb.at<float>(0, 0) = rgb_param.fxrgb;
	K_rgb.at<float>(0, 2) = rgb_param.cxrgb;
	K_rgb.at<float>(1, 1) = rgb_param.fyrgb;
	K_rgb.at<float>(1, 2) = rgb_param.cyrgb;
	K_rgb.at<float>(2, 2) = 1;

	K_ir.at<float>(0, 0) = rgb_param.fxir;
	K_ir.at<float>(0, 2) = rgb_param.cxir;
	K_ir.at<float>(1, 1) = rgb_param.fyir;
	K_ir.at<float>(1, 2) = rgb_param.cyir;
	K_ir.at<float>(2, 2) = 1;

	rgb_param.R00 = realpd[18];
	rgb_param.R01 = realpd[19];
	rgb_param.R02 = realpd[20];

	rgb_param.R10 = realpd[21];
	rgb_param.R11 = realpd[22];
	rgb_param.R12 = realpd[23];

	rgb_param.R20 = realpd[24];
	rgb_param.R21 = realpd[25];
	rgb_param.R22 = realpd[26];

	R.at<float>(0, 0) = rgb_param.R00;
	R.at<float>(0, 1) = rgb_param.R01;
	R.at<float>(0, 2) = rgb_param.R02;
	R.at<float>(1, 0) = rgb_param.R10;
	R.at<float>(1, 1) = rgb_param.R11;
	R.at<float>(1, 2) = rgb_param.R12;
	R.at<float>(2, 0) = rgb_param.R20;
	R.at<float>(2, 1) = rgb_param.R21;
	R.at<float>(2, 2) = rgb_param.R22;

	rgb_param.T1 = realpd[27];
	rgb_param.T2 = realpd[28];
	rgb_param.T3 = realpd[29];

	T.at<float>(0, 2) = rgb_param.T1;
	T.at<float>(1, 2) = rgb_param.T2;
	T.at<float>(2, 2) = rgb_param.T3;
	tmpM = K_rgb * R*K_ir.inv() + K_rgb * T*K_ir.inv() / 600;
	dsaver->setParam(rgb_param.fxrgb, rgb_param.fyrgb, rgb_param.cxrgb, rgb_param.cyrgb);
	std::ofstream logFile; logFile.open("moudule_param.yaml");
	logFile << "fx: " << rgb_param.fxrgb << std::endl;
	logFile << "fy: " << rgb_param.fyrgb << std::endl;
	logFile << "cx: " << rgb_param.cxrgb << std::endl;
	logFile << "cy: " << rgb_param.cyrgb << std::endl;
	logFile << "tx: " << 40 << std::endl;
	logFile << "mind: " << 300 << std::endl;
	logFile << "maxd: " << 2000 << std::endl;
	logFile << "f0: " << rgb_param.fxrgb << std::endl;
	logFile.close();
}

void VisionDemo::on_ShowImage_clicked()
{
	std::thread t1(&VisionDemo::thread_camera, this);
	t1.detach();
}