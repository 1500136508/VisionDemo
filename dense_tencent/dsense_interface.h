/********************************
author:huangruopu
date:2020/05/13
function: dsense algorithm
version:v1.0_190805
mark: first release version
1.depth estimation
2.temperature shift warp
3.depth to rgb register
4.filling hole and denoise

********************************/ 
#pragma once

#define MAX(a,b) (a>b?a:b)
#define MIN(a,b) (a<b?a:b)
typedef unsigned char uchar;//8bit
typedef unsigned short ushort;//16bit
typedef unsigned __int64 uint64;//64bit
typedef struct faceRectMy
{
    int x;
    int y;
    int width;
    int height;
}faceRectIn;//�������λ�ô�С
typedef struct adaptiveParameter
{
    int SADWindowSize;                     //SAD���ڴ�С
    int minDisparity;                      //��С�Ӳһ��Ϊ����
    int numberOfDisparities;               //�ӲΧ = ����Ӳ�+��С�Ӳ�-1
    int textureThreshold;                  //����check ��ֵ
    int uniquenessRatio;                   //uni check ��ֵ
    short disp12MaxDiff;                   //LR check ��ֵ
    short lambdaAD;                        //SADϵ��
    short lambdaCensus;                    //censusϵ��
    int focalLength;                       //����
    float b;                               //���߿��
    float Z;                               //�ο�ͼ����
    bool isLeftCam;                        //����ͷ�Ƿ�����ߣ�Ĭ����false
    bool isfilled;                         //�Ƿ񲹶�
    bool isDenoise;                        //�Ƿ�ȥ��
    int speckleWindowSize;                 //ȥ��̶ȣ�Խ��ȥ��Խ��
    int holeSize;                          //�����̶ȣ�Խ�󲹶�Խ��
    int lines;                             //��������
}adaptiveState;//��������㷨�ɵ����Ĳ���
typedef struct ir_rgb_Parameter
{
    //ir�Ͳ�ɫ����ͷ�ڲ�
    float fxir;
    float fyir;
    float cxir;
    float cyir;
    float fxrgb;
    float fyrgb;
    float cxrgb;
    float cyrgb;

    //ir�Ͳ�ɫ����ͷ���
    float R00;
    float R01;
    float R02;
    float R10;
    float R11;
    float R12;
    float R20;
    float R21;
    float R22;
    float T1;
    float T2;
    float T3;

}ir_rgb_State;//����ͷ�����
//������Ƚӿ�
void __declspec(dllexport) dsense_interface
(
    unsigned char* imgRight,               //����Ĳο�ͼ
    unsigned char* imgLeft,                //ʵʱͼƬ
    float* depth,                          //������ͼ
    adaptiveState& adaptivePara,           //�ɵ����Ĳ���
    faceRectIn& rectIn,                    //������λ�ô�С
    int width,                             //����ͼ����
    int height,                             //����ͼ��߶�
    short* disparity              //����Ӳ�ͼ
);
//�¶�Ư�Ƶ����ӿ�
void __declspec(dllexport) temper_warp_interface
(
    unsigned char* input,                  //����ɢ��ͼ
    unsigned char* output,                 //��Ư�������ɢ��ͼ
    unsigned char* mapx,                   //�¶�x����Ư�ƵĲ�������Ҫ�ڴ��Сwidth*height�ֽ�
    unsigned char* mapy,                   //�¶�x����Ư�ƵĲ�������Ҫ�ڴ��Сwidth*height�ֽ�
    int width,                             //����ͼ����
    int height,                            //����ͼ��߶�
    float temperature,                     //ʵʱ��ȡ���ĵ�ǰ�¶�
    float temp_stand,                      //�궨��������ĳ���
    float temp_max,                        //�궨�������������¶�
    float temp_min                         //�궨�����������С�¶�
);

//���ͼ���ɫͼ����ӿ�
void __declspec(dllexport) depth2RGB
(
    float* inputdepth,                     //�������ͼ
    float* outdepth,                       //������������ͼ
    float* tmpdepth,                       //�м�buffer����Ҫ�ڴ��С4*width*height�ֽ�
    int width_rgb,                         //��ɫͼ���
    int height_rgb,                        //��ɫͼ�߶�
    int width_ir,                          //����ͼ���
    int height_ir,                         //����ͼ�߶�
    ir_rgb_State& para                     //����ͷ�����
);

//IRͼ���ɫͼ����ӿ�
int __declspec(dllexport) ir2RGB
(
    unsigned char* input_ir,               //����IRͼ
    float* inputdepth,                     //���ԭʼ���ͼ
    int width_ir,                          //IRͼ���
    int height_ir,                         //IRͼ�߶�
    ir_rgb_State& para,                    //����ͷ�����
    faceRectIn& rectIn,                    //IRͼ������λ��
    int& result_x,                         //���:x������Ҫƽ�Ƶ����أ�������ʾ����ƽ��
    int& result_y                          //���:y������Ҫƽ�Ƶ����أ�������ʾ����ƽ��
);

//ȥ�����ͼ���Ľӿ�
void __declspec(dllexport) denoise(
    float* img,                //ori depth map change to clean depth map
    short newVal,              //error depth,if more than it ,not a bad depth
    int maxSpeckleSize,        //the maximum of speckle  , if less than it, is a speckle
    short maxDiff,             //the maximum of difference , if the pixel difference of two adjacent point is less than it,two point is the same speckle region
    unsigned char* _buf,       //buffer size = width*height*(3*sizeof(int)+sizeof(uchar)) <= 20M
    int max_th,                //remove out depth 
    int width,                 //image width
    int height                 //image height
);

//���ͼ�����Ľӿ�
void __declspec(dllexport) filling(
    unsigned char* guide,         // gray edge map
    float* img,                   //ori depth map change to filled depth map
    short newVal,                 //error depth,if more than it ,not a bad depth
    int maxSpeckleSize,           //the maximum of speckle  , if less than it, is a speckle
    int windows,                  //searching window for fill average value
    unsigned char* _buf,          //buffer size = width*height*(3*sizeof(int)+sizeof(uchar)) <= 13M
    int width,                    //image width
    int height                    //image height
);
