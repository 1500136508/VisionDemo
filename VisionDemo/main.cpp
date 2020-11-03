#include "VisionDemo.h"
#include "CameraDS.h"
//#include "CvvImage.h"
#include <highgui.hpp>
#include <QtWidgets/QApplication>
#include <thread>

int thread_camera()
{
	int cam_count;

	//������ȡ����ͷ��Ŀ
	cam_count = CCameraDS::CameraCount();
	printf("There are %d cameras.\n", cam_count);


	//��ȡ��������ͷ������
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

	//�򿪵�һ������ͷ
	//if(! camera.OpenCamera(0, true)) //��������ѡ�񴰿�
	if (!camera.OpenCamera(0, false, 640, 480)) //����������ѡ�񴰿ڣ��ô����ƶ�ͼ���͸�
	{
		fprintf(stderr, "Can not open camera.\n");
		return -1;
	}


	cvNamedWindow("camera");

	while (1)
	{
		//��ȡһ֡
		IplImage *pFrame = camera.QueryFrame();

		//��ʾ
		cvShowImage("camera", pFrame);

		if (cvWaitKey(20) == 'q')
			break;
	}
	camera.CloseCamera(); //�ɲ����ô˺�����CCameraDS����ʱ���Զ��ر�����ͷ

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
