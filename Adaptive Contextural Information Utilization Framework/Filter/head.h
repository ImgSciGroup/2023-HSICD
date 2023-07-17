#pragma once
#include<fstream>  //ifstream
#include<iostream>
#include "string"   //包含getline()
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


// 保存区域BGR值：0 - B, 1 - G, 2 - R
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

/*定义4分位数*/
struct Quartile {
	int total_num = 0;//保存该区域内含像素的总个数
	vector<int> data_position[3];//保存原始数据数组;0/1/2表示B/G/R(下同)
	vector<int> data_sort[3];//保存排序后数据数组
	int quartile_position[3][5] = { 0 };//保存4分位点在原始数据数组下的位置
	vector<int> quartile_data_vector[3];//保存4分位点在原始数据数组下位置的像素值到vector
	int quartile_data[3][5] = { 0 };//保存4分位点在原始数据数组下的位置的像素值
};
/*RGB直方图hist[i][j]：i=0/1/2(表示B/G/R三波段)；j表示像素值；hist[i][j]表示i波段像素值为j的频率*/
struct Histogram
{
	int bin;					 //直方图参数bin
	int Hist[3][80] = { 0 };	 //局部直方图在0-255(即：参数bin个)分布：//0--B波段////1--G波段////2--R波段////
	int hist[3][80] = { 0 };
	float Sort_Hist[3][80] = { 0 };	 //BGR排序直方图
	float Gray_Hist[33] = { -1 };	 //灰度直方图
	int noNULLnum[3] = { 0 };		//统计每个波段直方图非空bin的个数
	int hist_avg[3] = { 0 };	 //直方图的均值（0/1/2分别代表：B/G/R）
	int hist_fre[3][80] = { 0 };	 //直方图的排序像素值（0/1/2分别代表：B/G/R）
	void clear()
	{
		Gray_Hist[33] = { -1 };		//灰度直方图
		Sort_Hist[3][80] = { 0 };	//灰度排序直方图
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