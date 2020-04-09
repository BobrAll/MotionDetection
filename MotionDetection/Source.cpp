#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

VideoCapture cap(1);
Mat cam_img, mask_img, filled_mask_img;
Mat templ_img(480, 640, CV_8UC3, Scalar(0, 0, 0));
Mat prev_templ_img(480, 640, CV_8UC3, Scalar(0, 0, 0));
bool mode = 0;
unsigned short fps = 60;

void open_settings();
void copy_img(Mat &img1, Mat &img2);
void do_detection();
void do_filled_mask();
void work_with_contours();
int main()
{
	if (cap.isOpened())
		while (true)
		{
			cap.read(cam_img);
			do_detection();
			do_filled_mask();
			work_with_contours();

			imshow("Cam", cam_img);
			imshow("Mask", mask_img);
			imshow("Filled mask", filled_mask_img);

			char c = waitKey(round(1000/fps));
			if (c == 's')
			{
				open_settings();
				continue;
			}
			if (c > 0)
				break;
		}
	else
	{
		cout << "Cam is not connected\n";
		system("pause");
	}
}
void open_settings()
{
	short num;
	cout << "[Settings menu]\n";
	cout << "1 >> Change detection mode.\n";
	cout << "2 >> Change fps.\n";
	cout << "And other key to exit.\n";
	cout << "Enter number of required option >>";
	cin >> num;

	switch (num)
	{
	case 1: std::cout << "Set value of detection mode\n";
		cout << "0 >> Each previous frame set as template for next frame\n";
		cout << "1 >> Current frame set as template\n";
		cin >> mode; break;

	case 2: cout << "Set value of fps for camera(1-60) >>";
		cin >> fps;
		if (fps > 60)
			fps = 60; break;

	default: break;
	}
	system("cls");
}
void copy_img(Mat &img1, Mat &img2)
{
	//1-е изображение копируется во 2-е
	for (size_t i = 0; i < img1.rows; i++)
	{
		for (size_t j = 0; j < img1.cols; j++)
		{
			img2.at<Vec3b>(i, j)[0] = img1.at<Vec3b>(i, j)[0];
			img2.at<Vec3b>(i, j)[1] = img1.at<Vec3b>(i, j)[1];
			img2.at<Vec3b>(i, j)[2] = img1.at<Vec3b>(i, j)[2];
		}
	}
}
void do_detection()
{
	if (mode == 0)
	{
		unsigned short gap = 30;
		Mat current_img(cam_img.rows, cam_img.cols, CV_8UC3);
		Mat local_mask_img(cam_img.rows, cam_img.cols, CV_8UC3, Scalar(0, 0, 0));
		copy_img(cam_img, current_img);
		
		for (size_t i = 0; i < templ_img.rows; i++)
		{
			for (size_t j = 0; j < templ_img.cols; j++)
			{
				if (abs(cam_img.at<Vec3b>(i, j)[0] - templ_img.at<Vec3b>(i, j)[0]) > gap ||
					abs(cam_img.at<Vec3b>(i, j)[1] - templ_img.at<Vec3b>(i, j)[1]) > gap ||
					abs(cam_img.at<Vec3b>(i, j)[2] - templ_img.at<Vec3b>(i, j)[2]) > gap ||
					abs(cam_img.at<Vec3b>(i, j)[0] - prev_templ_img.at<Vec3b>(i, j)[0]) > gap ||
					abs(cam_img.at<Vec3b>(i, j)[1] - prev_templ_img.at<Vec3b>(i, j)[1]) > gap ||
					abs(cam_img.at<Vec3b>(i, j)[2] - prev_templ_img.at<Vec3b>(i, j)[2]) > gap)
				{
					local_mask_img.at<Vec3b>(i, j)[0] = 0;
					local_mask_img.at<Vec3b>(i, j)[1] = 200;
					local_mask_img.at<Vec3b>(i, j)[2] = 0;
				}
			}
		}
		copy_img(templ_img, prev_templ_img);
		copy_img(current_img, templ_img);
		mask_img = local_mask_img;
	}
}
void do_filled_mask()
{
	Mat local_filled_mask_img(mask_img.rows, mask_img.cols, CV_8UC3, Scalar(0, 0, 0));
	//отсеивание шумов
	for (size_t i = 2; i < mask_img.rows - 2; i++)
	{
		for (size_t j = 2; j < mask_img.cols - 2; j++)
		{
			if (mask_img.at<Vec3b>(i, j - 2)[1] != 200 && mask_img.at<Vec3b>(i, j + 2)[1] != 200)
				mask_img.at<Vec3b>(i, j)[1] = 0;
			if (mask_img.at<Vec3b>(i - 2, j)[1] != 200 && mask_img.at<Vec3b>(i + 2, j)[1] != 200)
				mask_img.at<Vec3b>(i, j)[1] = 0;
		}
	}
	//заливка маски
	for (size_t i = 0; i < mask_img.rows; i++)
	{
		vector<size_t> pos;
		for (size_t j = 0; j < mask_img.cols; j++)
		{
			if (mask_img.at<Vec3b>(i, j)[1] == 200)
				pos.push_back(j);
		}
		if(pos.size() > 0)
			for (size_t k = *min_element(pos.begin(), pos.end()); k < *max_element(pos.begin(), pos.end()); k++)
			{
				local_filled_mask_img.at<Vec3b>(i, k)[1] = 200;
			}
	}
	//отсеивание шумов
	for (size_t i = 2; i < local_filled_mask_img.rows - 2; i++)
	{
		for (size_t j = 2; j < local_filled_mask_img.cols - 2; j++)
		{
			if (local_filled_mask_img.at<Vec3b>(i, j - 2)[1] != 200 && local_filled_mask_img.at<Vec3b>(i, j + 2)[1] != 200)
				local_filled_mask_img.at<Vec3b>(i, j)[1] = 0;
			if (local_filled_mask_img.at<Vec3b>(i - 2, j)[1] != 200 && local_filled_mask_img.at<Vec3b>(i + 2, j)[1] != 200)
				local_filled_mask_img.at<Vec3b>(i, j)[1] = 0;
		}
	}
	filled_mask_img = local_filled_mask_img;
}
void work_with_contours()
{
	vector<vector<Point>> contours;
	Mat canny_img(filled_mask_img.rows, filled_mask_img.cols, CV_8UC1);
	Canny(filled_mask_img, canny_img, 100, 200);
	findContours(canny_img, contours, 1, 2);
		drawContours(cam_img, contours, -1, Scalar(0, 0, 200), 5);
}