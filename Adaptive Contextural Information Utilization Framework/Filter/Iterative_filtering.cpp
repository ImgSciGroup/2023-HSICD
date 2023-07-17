#define _CRT_SECURE_NO_WARNINGS
#include"head.h"



//int main()
//{
//	IplImage* image1 = cvLoadImage(R"(C:\data\Hermiston3\result\Filter_image-1.bmp)");//����ԭʼͼ��
//	IplImage* image2 = cvLoadImage(R"(C:\data\Hermiston3\result1\Filter_image-1.bmp)");//����ԭʼͼ��
//	Scalar pixel1, pixel2;
//	IplImage* res = cvCreateImage(cvGetSize(image1), IPL_DEPTH_8U, 1);
//	for (int i = 0; i < image1->height; i++) {
//		for (int j = 0; j < image1->width; j++) {
//			pixel1 = cvGet2D(image1, i, j);
//			pixel2 = cvGet2D(image2, i, j);
//			int  sum = sqrt(pow(pixel1.val[0] - pixel2.val[0], 2) + pow(pixel1.val[1] - pixel2.val[1], 2) + pow(pixel1.val[2] - pixel2.val[2], 2));
//			*cvPtr2D(res, i, j, NULL) = sum;
//		}
//	}
//	Mat dstImage2 = cvarrToMat(res);
//	imwrite(R"(C:\data\Hermiston3\result1\F.bmp)", dstImage2);

//}



int main(void) 
{
//////////////////////////////////////��������/////////////////////////////////////////////////////
	IplImage* Original_image = cvLoadImage("C:\\data\\Wetland\\data\\PostImg_2007_new_4_14_40.tif");//����ԭʼͼ��
	
	string Path = "C:\\data\\Wetland\\result1";//����ʵ���������õ���·��
	double Stability = 0.1;//�ϸ�Ҫ��
/////////////////////////////////////��������/////////////////////////////////////////////////////
	IplImage* out = cvCreateImage(cvGetSize(Original_image), Original_image->depth, 1);//����Ӧ�����ǩӰ��
	IplImage* Filter_img = cvCreateImage(cvGetSize(Original_image), Original_image->depth, 3);//�����˲�Ӱ��
	IplImage* Gray_image = NULL;//����ԭʼӰ��ת��Ϊ�Ҷ�Ӱ�����������Ӧ����
	IplImage* CGI = NULL;//��̬���»Ҷ�Ӱ��
	vector<double>STD;//��̬�������صı�׼��
	vector<CvPoint>Adaptive_region;//��������Ӧ��������
	CvScalar Pixel_filter_value;//�����˲�������ֵ
	double Std_pixel = 0;//����ÿ�����ؾ����Ҷ�Ӱ������Ӧ����������ñ�׼��
	CvPoint center;//�������ӵ�
	CvScalar SC;//Ϊ�Ѿ������˲�������ֵ��ֵ
	int Iter_num = 0;//��������
	int Num = 0;//�������ʱ�׼������
	int IterStop = 0;//����ֹͣ����
///////////////////////////////////////�����˲�//////////////////////////////////////////////////////
	if (Iter_num == 0)
	{
		cout << "������ʼ" << endl;
		cout << "��" << Iter_num << "�ε���" << endl;
		Iter_num++;
		Gray_image = Convert_gray_image(Original_image, Path, Iter_num);
		CGI = Convert_gray_image(Original_image, Path, Iter_num);
		for (int i = 0; i < Original_image->height; i++)
		{
			for (int j = 0; j < Original_image->width; j++)
			{
				center.x = i; center.y = j;
				Adaptive_region.clear(); Adaptive_region.shrink_to_fit();
				Adaptive_region = Unparam_Region(Gray_image, out, center);
				Std_pixel = Calculate_adaptive_std(CGI, Adaptive_region);
				STD.push_back(Std_pixel);
				Pixel_filter_value = Calculate_mean(Adaptive_region, Original_image);
				cvSet2D(Filter_img, center.x, center.y, Pixel_filter_value);
			}
		}
		char adr[256] = { 0 };//�˲�Ӱ��洢�ĵ�ַ
		sprintf(adr, "%s\\Filter_image-%d.bmp", Path.data(), Iter_num);//�Ҷ�Ӱ������
		Mat dstImage1 = cvarrToMat(Filter_img);
		imwrite(adr, dstImage1);
		//����Ӱ��
	}
	while (IterStop!= (Original_image->height * Original_image->width))
	{
		cout << "��" << Iter_num << "�ε���" << endl;
		cout << "������"<< IterStop <<"�����ص�"<<endl;
		Iter_num++;
		Num = 0;
		IterStop = 0;
		CGI = Convert_gray_image(Filter_img, Path, Iter_num);
		for (int i = 0; i < Original_image->height; i++)
		{
			for (int j = 0; j < Original_image->width; j++)
			{
				if (STD[Num] != -1)
				{
					center.x = i; center.y = j;
					Adaptive_region.clear(); Adaptive_region.shrink_to_fit();
					Adaptive_region = Unparam_Region(Gray_image, out, center);
					Std_pixel = Calculate_adaptive_std(CGI, Adaptive_region);
					if (abs(Std_pixel - STD[Num]) > Stability)//0.1
					{
						Pixel_filter_value = Calculate_mean(Adaptive_region, Filter_img);
						cvSet2D(Filter_img, center.x, center.y, Pixel_filter_value);
						STD[Num] = Std_pixel;
					}
					else
					{
						STD[Num] = -1;
						IterStop++;
					}
				}
				else
				{
					IterStop++;
				}

				Num++;
			}
		}
		char adr[256] = { 0 };//�˲�Ӱ��洢�ĵ�ַ
		sprintf(adr, "%s\\Filter_image-%d.bmp", Path.data(), Iter_num);//�Ҷ�Ӱ������
		Mat dstImage2 = cvarrToMat(Filter_img);
		imwrite(adr, dstImage2);
		//����Ӱ��
	}
	cvReleaseImage(&Filter_img);
	cvReleaseImage(&Original_image);
	return 0;
}


/*�ٲ�ɫӰ��ת��Ϊ�Ҷ�Ӱ��*/
IplImage* Convert_gray_image(IplImage* Originalimage, string path,int IterNum)
{
	IplImage* Grayscale_image = cvCreateImage(cvGetSize(Originalimage), IPL_DEPTH_8U, 1);//�Ҷ�ֵͼ��
	CvScalar C;//��ȡԭʼ����RGBֵ
	double Gray_value = 0;//���ػҶ�ֵ
	for (int i = 0; i < Originalimage->height; i++)
	{
		for (int  j = 0; j < Originalimage->width; j++)
		{
			C = cvGet2D(Originalimage, i, j);
			Gray_value = C.val[0] * 0.114 + C.val[1] * 0.587 + C.val[2] * 0.299;
			*cvPtr2D(Grayscale_image, i, j, NULL) = Gray_value;
		}
	}
	char adr[256] = { 0 };//�Ҷ�Ӱ��洢�ĵ�ַ
	sprintf(adr, "%s\\Grayscale_image-%d.bmp", path.data(), IterNum - 1);//�Ҷ�Ӱ������
	Mat dstImage = cvarrToMat(Grayscale_image);
	imwrite(adr, dstImage);
	
	return Grayscale_image;
}
/*����ٲ�ɫӰ��ÿ�����صľ�ֵ*/
 CvScalar Calculate_mean(vector<CvPoint>Region, IplImage* Originalimage)
{
	CvScalar S;//��ȡԭʼ����RGBֵ
	double Sum_B = 0;
	double Sum_G = 0;
	double Sum_R = 0;
	double Avg_B = 0;
	double Avg_G = 0;
	double Avg_R = 0;
	for (int i = 0; i < Region.size(); i++)
	{
		S = cvGet2D(Originalimage, Region[i].x,Region[i].y);
		Sum_B += S.val[0];
		Sum_G += S.val[1];
		Sum_R += S.val[2];
	}
	Avg_B = Sum_B / Region.size();
	Avg_G = Sum_G / Region.size(); 
	Avg_R = Sum_R / Region.size();
	S.val[0] = Avg_B;
	S.val[1] = Avg_G;
	S.val[2] = Avg_R;
	return S;
}
/*������ڻҶ�Ӱ������Ӧ�����ڵı�׼��*/
double Calculate_adaptive_std(IplImage* Grayimage, vector<CvPoint>Region)
{
	double Pixel_value = 0;//�����ػҶ�ֵ
	double Avg_adaptive = 0;//���վ�ֵ
	double Variance_sum_adaptive = 0;
	double Std_adaptive = 0;//���ձ�׼��
	for (int i = 0; i < Region.size(); i++)
	{
		Pixel_value += *cvPtr2D(Grayimage, Region[i].x, Region[i].y, NULL);
	}
	Avg_adaptive = Pixel_value / Region.size();
	for (int  i = 0; i < Region.size(); i++)
	{
		Pixel_value = *cvPtr2D(Grayimage, Region[i].x, Region[i].y, NULL);
		Variance_sum_adaptive += pow((Pixel_value- Avg_adaptive),2);
	}
	Std_adaptive = sqrt(Variance_sum_adaptive / Region.size());

	return Std_adaptive;
}


/*
*ʱ�䣺20201020
*�������ܣ���չ������pix->[���ķ�λ�����ķ�λ]
*����1������չӰ��(�Ҷ�)
*����2��������չ���Ӱ��(ԭʼΪ��ɫ����չ�����ر��Ϊ��ɫ)
*����3�����ӵ�(����Ӧ������������)
*����4��ѵ�������꼯(�仯��δ�仯ѵ�������꼯)
*/
/**/
vector<CvPoint> Unparam_Region(IplImage* img, IplImage* out, CvPoint Keypoints)
{
	int DIR[8][12] = { { -1,-1 },{ -1,0 },{ -1,1 },{ 0,-1 },{ 0,1 },{ 1,-1 },{ 1,0 },{ 1,1 } };
	cvZero(out);
	int x = Keypoints.x;//������
	int y = Keypoints.y;//������
	int center_point = *cvPtr2D(img, Keypoints.x, Keypoints.y, NULL);//���ӵ�Ҷ�ֵ
	vector<CvPoint> points;//�������չ�����
	vector<CvPoint> Connected_points;//��������չ�����
	points.clear(); points.shrink_to_fit();
	Connected_points.clear(); Connected_points.shrink_to_fit();
	CvPoint point_1, point_2;/////////
	Connected_points.push_back(Keypoints);
	//points.push_back(Keypoints);
	*(cvPtr2D(out, x, y, NULL)) = 255;
	int num = 0;//ͳ������չ���ظ���
	int pix = 0;//��ȡ����ֵ
	double Condition_Low = 0, Condition_Up = 0;
	vector<CvPoint> win_pts = Get_Window_xy(img, Keypoints, 3);
	vector<int> win_pix = get_Region_pixel(img, win_pts);
	Quartile condition = get_Quartile(img, win_pts);
	Condition_Low = condition.quartile_data[0][1];
	Condition_Up = condition.quartile_data[0][3];
	double win_mean = average_value(win_pix);//3���ھ�ֵ
	double win_std = std_value(win_pix);//3���ڷ���
	//cout << "init std=" << win_std << endl;
	for (int iNum = 0; iNum < 8; iNum++)
	{
		int iCurPosX = x + DIR[iNum][0];
		int iCurPosY = y + DIR[iNum][1];
		if (iCurPosX >= 0 && iCurPosX < (img->height) && iCurPosY >= 0 && iCurPosY < (img->width))
		{
			if (*(cvPtr2D(out, iCurPosX, iCurPosY, NULL)) != 255)
			{
				pix = *(cvPtr2D(img, iCurPosX, iCurPosY, NULL));
				if (Condition_Low <= pix && pix <= Condition_Up /*&& abs(center_point - pix) <= win_std*/)
				{//��������
					*(cvPtr2D(out, iCurPosX, iCurPosY, NULL)) = 255;
					point_1.x = iCurPosX;
					point_1.y = iCurPosY;
					Connected_points.push_back(point_1);
					points.push_back(point_1);
					num += 1;
				}
			}
		}
	}
	vector<int> pix_flag;
	bool grow_bool = true;
	for (int pos = 0; pos < points.size(); pos++)
	{
		if (num == 0)
			return Connected_points;
		else if (num > 1000)
		{
			return Connected_points;
		}
		else if (pos < points.size())
		{
			x = points[pos].x;
			y = points[pos].y;
		}
		else
		{
			return Connected_points;
		}
		for (int iNum = 0; iNum < 8; iNum++)
		{
			int iCurPosX = x + DIR[iNum][0];
			int iCurPosY = y + DIR[iNum][1];
			if (iCurPosX >= 0 && iCurPosX < (img->height) && iCurPosY >= 0 && iCurPosY < (img->width))
			{
				if (*(cvPtr2D(out, iCurPosX, iCurPosY, NULL)) != 255)
				{
					pix = *(cvPtr2D(img, iCurPosX, iCurPosY, NULL));
					if (Condition_Low <= pix && pix <= Condition_Up /*&& abs(center_point - pix) <= win_std*/)
					{//��������
						point_1.x = iCurPosX;
						point_1.y = iCurPosY;
						*(cvPtr2D(out, iCurPosX, iCurPosY, NULL)) = 255;
						Connected_points.push_back(point_1);
						points.push_back(point_1);
						num += 1;
						//grow_bool = true;
					}
				}
			}
		}
	}
	return Connected_points;
}
/*��ȡ��centerΪ���ģ���winsizeΪ��С�Ĵ�����img�ж�Ӧ������*/
vector<CvPoint> Get_Window_xy(IplImage* img, CvPoint center, int winsize)
{
	vector<CvPoint> points;
	CvPoint point;
	int minX = center.x - (int)floor(0.5 * winsize);
	int maxX = center.x + (int)floor(0.5 * winsize);
	int minY = center.y - (int)floor(0.5 * winsize);
	int maxY = center.y + (int)floor(0.5 * winsize);
	for (int i = minX; i <= maxX; i++)
	{
		for (int j = minY; j <= maxY; j++)
		{
			if (i >= 0 && i < img->height - 1 && j >= 0 && j < img->width)
			{
				point.x = i; point.y = j;
				//point = *cvPtr2D(img, i, j, NULL);
				points.push_back(point);
			}
		}
	}
	return points;
}
/*
*�������ܣ���������㼯��Ӧ��Ӱ��img�ϵ�����ֵ
*����1����Ҫ��ȡ����ֵӰ��
*����2������㼯
*/
vector<int> get_Region_pixel(IplImage* img, vector<CvPoint> region_points) {
	vector<int> region_pixel;
	region_pixel.clear();
	region_pixel.shrink_to_fit();
	int pixel = 0;
	for (int i = 0; i < region_points.size(); i++) {
		pixel = *cvPtr2D(img, region_points[i].x, region_points[i].y, NULL);
		region_pixel.push_back(pixel);
	}
	return region_pixel;
}
/*
*�������ܣ���������������ֵ�����ؽ��
*�������������ݼ�
*/
double average_value(vector<int> data) {
	double avg = 0;
	int sum = 0;
	for (int i = 0; i < data.size(); i++) {
		sum += data[i];
	}
	avg = sum / data.size();
	return avg;
}
/*
*�������ܣ�������������󷽲���ؽ��
*��������������
*/
double std_value(vector<int> data) {
	double avg = 0;
	double std = 0;
	double sum_variance = 0;
	avg = average_value(data);
	for (int i = 0; i < data.size(); i++) {
		sum_variance += ((data[i] - avg) * (data[i] - avg));
	}
	std = sqrt(sum_variance / data.size());

	return std;
}
/*����points�㼯��imgͼ���е��ķ�λ�㣨5���ؼ��㣩*/
Quartile get_Quartile(IplImage* img, vector<CvPoint> points)
{
	if (img->nChannels == 1)
	{//cout << "����Ҷ�ͼ����ķ�λ��" << endl;

		float ratio[5] = { 0, 0.25, 0.5, 0.75, 1 };
		Quartile quartile;
		int pix = 0;
		quartile.total_num = points.size();
		for (int i = 0; i < points.size(); i++)
		{
			pix = *cvPtr2D(img, points[i].x, points[i].y, NULL);
			quartile.data_position[0].push_back(pix);
			quartile.data_sort[0].push_back(pix);
		}
		sort(quartile.data_sort[0].begin(), quartile.data_sort[0].end());
		for (int j = 0; j < 5; j++)
		{
			if (j > 0)
			{
				quartile.quartile_data[0][j] = quartile.data_sort[0][(quartile.total_num * ratio[j]) - 1];
				quartile.quartile_data_vector[0].push_back(quartile.data_sort[0][(quartile.total_num * ratio[j]) - 1]);
			}
			else
			{
				quartile.quartile_data[0][j] = quartile.data_sort[0][quartile.total_num * ratio[j]];
				quartile.quartile_data_vector[0].push_back(quartile.data_sort[0][quartile.total_num * ratio[j]]);
			}
		}
		return quartile;
	}
	else if (img->nChannels == 3)
	{//cout << "����RGBͼ����ķ�λ��" << endl;
		float ratio[5] = { 0, 0.25, 0.5, 0.75, 1 };
		Quartile quartile;
		Scalar sc;
		quartile.total_num = points.size();
		for (int i = 0; i < points.size(); i++)
		{
			sc = cvGet2D(img, points[i].x, points[i].y);
			for (int j = 0; j < 3; j++)
			{
				quartile.data_position[j].push_back(sc.val[j]);
				quartile.data_sort[j].push_back(sc.val[j]);
			}

		}
		for (int i = 0; i < 3; i++)
			sort(quartile.data_sort[i].begin(), quartile.data_sort[i].end());
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				if (j > 0)
				{
					quartile.quartile_data[i][j] = quartile.data_sort[i][(quartile.total_num*ratio[j]) - 1];
					quartile.quartile_data_vector[i].push_back(quartile.data_sort[i][(quartile.total_num*ratio[j]) - 1]);
				}
				else
				{
					quartile.quartile_data[i][j] = quartile.data_sort[i][quartile.total_num*ratio[j]];
					quartile.quartile_data_vector[i].push_back(quartile.data_sort[i][quartile.total_num*ratio[j]]);
				}
			}
		}
		return quartile;
	}
	else
	{
		cout << "ͼ�񲨶�������1��3����" << endl;
	}
}

/*����Ӧ������չ����*/
/*vector<CvPoint> Connected_comp(IplImage* CMI, IplImage* out, CvPoint Keypoints, int pixel_diff, int Connected_num)
{
	int DIR[8][12] = { { -1,-1 },{ -1,0 },{ -1,1 },{ 0,-1 },{ 0,1 },{ 1,-1 },{ 1,0 },{ 1,1 } };
	cvZero(out);
	int x = Keypoints.x;//������
	int y = Keypoints.y;//������
	int center_point = *cvPtr2D(CMI, Keypoints.x, Keypoints.y, NULL);//���ӵ�Ҷ�ֵ
	int num = 0;//ͳ������չ�����ظ���
	vector<CvPoint> points;//�������չ�������
	vector<CvPoint> Connected_points;//�������չ�������
	int pixel = 0;
	vector<int> Connected_pixel;
	Connected_pixel.clear();
	Connected_pixel.shrink_to_fit();
	points.clear();
	Connected_points.clear();
	CvPoint point_1, point_2;
	Connected_points.push_back(Keypoints);
	Connected_pixel.push_back(center_point);
	*(cvPtr2D(out, x, y, NULL)) = 255;
	for (int pos = -1; Connected_points.size() < Connected_num; pos++)
	{//num
		if (pos != -1)
		{
			if (num == 0)
				return Connected_points;
			else if (pos < points.size())
			{
				x = points[pos].x;
				y = points[pos].y;
			}
			else
			{
				return Connected_points;
			}
		}
		for (int iNum = 0; iNum < 8; iNum++)
		{
			int iCurPosX = x + DIR[iNum][0];
			int iCurPosY = y + DIR[iNum][1];
			if (iCurPosX >= 0 && iCurPosX < (CMI->height) && iCurPosY >= 0 && iCurPosY < (CMI->width))
			{
				if (*(cvPtr2D(out, iCurPosX, iCurPosY, NULL)) != 255)
				{
					if (abs(center_point - *(cvPtr2D(CMI, iCurPosX, iCurPosY, NULL))) <= pixel_diff)
					{//��������
						*(cvPtr2D(out, iCurPosX, iCurPosY, NULL)) = 255;
						point_2.x = iCurPosX;
						point_2.y = iCurPosY;
						pixel = *(cvPtr2D(CMI, iCurPosX, iCurPosY, NULL));
						Connected_pixel.push_back(pixel);
						Connected_points.push_back(point_2);
						num += 1;
						point_1.x = iCurPosX;
						point_1.y = iCurPosY;
						points.push_back(point_1);
						if (Connected_points.size() >= Connected_num)
							return Connected_points;
					}
				}
			}
		}
	}
	return Connected_points;
}
*/