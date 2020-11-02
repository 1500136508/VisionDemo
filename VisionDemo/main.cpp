#include "VisionDemo.h"
#include "CameraDS.h"
//#include "CvvImage.h"
#include <highgui.hpp>
#include <QtWidgets/QApplication>
#include <thread>

int thread_camera()
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
	if (!camera.OpenCamera(0, false, 640, 480)) //不弹出属性选择窗口，用代码制定图像宽和高
	{
		fprintf(stderr, "Can not open camera.\n");
		return -1;
	}


	cvNamedWindow("camera");

	while (1)
	{
		//获取一帧
		IplImage *pFrame = camera.QueryFrame();

		//显示
		cvShowImage("camera", pFrame);

		if (cvWaitKey(20) == 'q')
			break;
	}
	camera.CloseCamera(); //可不调用此函数，CCameraDS析构时会自动关闭摄像头

	cvDestroyWindow("camera");
	return 0;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VisionDemo w;
    w.show();

	std::thread t1(thread_camera);
	t1.detach();

    return a.exec();
}
