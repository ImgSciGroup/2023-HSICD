#pragma once
#include<fstream>  //ifstream
#include<iostream>
#include "string"   //����getline()
#include<cmath>
#include <vector>
#include <stdio.h>
#include <tchar.h>
#include<opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include "cv.h"
#include "highgui.h"
#include "Math.h"
#include <iostream>
#include <fstream> 
#include <iomanip>
#include <windows.h>
#include "io.h"
using namespace std;
using namespace cv;


// ��������BGRֵ��0 - B, 1 - G, 2 - R
struct Region_BGR {
	vector<int> BGR[3];//0-B,1-G,2-R
	void clear()
	{
		for (int i = 0; i < 3; i++)
		{
			BGR[i].clear(); BGR[i].shrink_to_fit();
		}
	}
};

/*����4��λ��*/
struct Quartile {
	int total_num = 0;//����������ں����ص��ܸ���
	vector<int> data_position[3];//����ԭʼ��������;0/1/2��ʾB/G/R(��ͬ)
	vector<int> data_sort[3];//�����������������
	int quartile_position[3][5] = { 0 };//����4��λ����ԭʼ���������µ�λ��
	vector<int> quartile_data_vector[3];//����4��λ����ԭʼ����������λ�õ�����ֵ��vector
	int quartile_data[3][5] = { 0 };//����4��λ����ԭʼ���������µ�λ�õ�����ֵ
};
/*RGBֱ��ͼhist[i][j]��i=0/1/2(��ʾB/G/R������)��j��ʾ����ֵ��hist[i][j]��ʾi��������ֵΪj��Ƶ��*/
struct Histogram
{
	int bin;					 //ֱ��ͼ����bin
	int Hist[3][80] = { 0 };	 //�ֲ�ֱ��ͼ��0-255(��������bin��)�ֲ���//0--B����////1--G����////2--R����////
	int hist[3][80] = { 0 };
	float Sort_Hist[3][80] = { 0 };	 //BGR����ֱ��ͼ
	float Gray_Hist[33] = { -1 };	 //�Ҷ�ֱ��ͼ
	int noNULLnum[3] = { 0 };		//ͳ��ÿ������ֱ��ͼ�ǿ�bin�ĸ���
	int hist_avg[3] = { 0 };	 //ֱ��ͼ�ľ�ֵ��0/1/2�ֱ����B/G/R��
	int hist_fre[3][80] = { 0 };	 //ֱ��ͼ����������ֵ��0/1/2�ֱ����B/G/R��
	void clear()
	{
		Gray_Hist[33] = { -1 };		//�Ҷ�ֱ��ͼ
		Sort_Hist[3][80] = { 0 };	//�Ҷ�����ֱ��ͼ
		hist_avg[3] = { 0 };
		noNULLnum[3] = { 0 };
	}
};


IplImage* Convert_gray_image(IplImage* Originalimage, string path, int IterNum);
CvScalar Calculate_mean(vector<CvPoint>Region, IplImage* Originalimage);
double Calculate_adaptive_std(IplImage* Grayimage, vector<CvPoint>Region);
//vector<CvPoint> Connected_comp(IplImage* CMI, IplImage* out, CvPoint Keypoints, int pixel_diff, int Connected_num);
vector<CvPoint> Unparam_Region(IplImage* img, IplImage* out, CvPoint Keypoints);
vector<CvPoint> Get_Window_xy(IplImage* img, CvPoint center, int winsize);
vector<int> get_Region_pixel(IplImage* img, vector<CvPoint> region_points);
Quartile get_Quartile(IplImage* img, vector<CvPoint> points);
double average_value(vector<int> data);
double std_value(vector<int> data);