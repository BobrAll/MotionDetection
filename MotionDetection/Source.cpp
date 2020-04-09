#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

VideoCapture cap(1);
Mat cam_img;
Mat templ_img(480, 640, CV_8UC3, Scalar(0, 0, 0));
bool mode = 0;
unsigned short fps = 60;

void open_settings();
void copy_img(Mat &img1, Mat &img2);
void do_detection();

int main()
{
	if (cap.isOpened())
	{
		while (true)
		{
			cap.read(cam_img);
			imshow("t", templ_img);
			do_detection();
			imshow("Cam", cam_img);
			char c = waitKey(round(1000/fps));
			if (c == 's')
			{
				open_settings();
				continue;
			}
			if (c > 0)
				break;
		}
	}
	else
	{
		cout << "Cam is not connected";
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
		Mat current_img(cam_img.rows, cam_img.cols, CV_8UC3);
		copy_img(cam_img, current_img);
		int gap = 25;
		for (size_t i = 0; i < templ_img.rows; i++)
		{
			for (size_t j = 0; j < templ_img.cols; j++)
			{
				if (abs(cam_img.at<Vec3b>(i, j)[0] - templ_img.at<Vec3b>(i, j)[0]) > gap ||
					abs(cam_img.at<Vec3b>(i, j)[1] - templ_img.at<Vec3b>(i, j)[1]) > gap ||
					abs(cam_img.at<Vec3b>(i, j)[2] - templ_img.at<Vec3b>(i, j)[2]) > gap)
				{
					cam_img.at<Vec3b>(i, j)[0] = 0;
					cam_img.at<Vec3b>(i, j)[1] = 200;
					cam_img.at<Vec3b>(i, j)[2] = 0;
				}
			}
		}
		copy_img(current_img, templ_img);
	}
}