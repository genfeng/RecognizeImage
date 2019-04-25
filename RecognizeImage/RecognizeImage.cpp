// RecognizeImage.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <windows.h>
#include <string>

#define MATCHMETHOD TM_SQDIFF_NORMED//宏定义匹配模式
using namespace cv;
using namespace std;

void ReplaceAll(std::string& source, const std::string& from, const std::string& to)
{
	std::string newString;
	newString.reserve(source.length());  // avoids a few memory allocations

	std::string::size_type lastPos = 0;
	std::string::size_type findPos;

	while (std::string::npos != (findPos = source.find(from, lastPos)))
	{
		newString.append(source, lastPos, findPos - lastPos);
		newString += to;
		lastPos = findPos + from.length();
	}

	// Care for the rest after last occurrence
	newString += source.substr(lastPos);

	source.swap(newString);
}

string GetExePath()
{
	TCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buffer, MAX_PATH);
	wstring::size_type pos = wstring(buffer).find_last_of(L"\\/");
	wstring w_ret = wstring(buffer).substr(0, pos);
	std::string ret(w_ret.begin(), w_ret.end());
	ReplaceAll(ret, "\\\\", "\\");
	return ret;
}

int main()
{
	static int s_thickness = 1;

	String s_img_path = GetExePath() + "\\..\\ImgTest\\";

	Mat src = imread(s_img_path + "1.jpg");
	if (src.empty())
	{
		src = imread(s_img_path + "1.png");
		if (src.empty())
		{
			printf("Could not load image...");
			return -1;
		}
	}

	Mat src_gray, binary;
	Mat Triangles = src.clone(), Rects = src.clone(), BigCircles = src.clone(), Lines = src.clone(), Plines = src.clone();
	Mat Alls = src.clone();

	imshow("Input Image", src);

	//二值化
	cvtColor(src, src_gray, COLOR_BGR2GRAY);
	threshold(src_gray, binary, 0, 255, THRESH_BINARY | THRESH_OTSU);
	binary = ~binary;
	imshow("binary", binary);

	//发现轮廓
	vector<vector<Point>> contours;
	vector<Point> point;
	vector<Vec4i> hireachy;
	findContours(binary, contours, hireachy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());

	// 绘制出所有轮廓
	for (int t = 0; t < (int)contours.size(); t++)
	{
		double epsilon = 0.01*arcLength(contours[t], true);
		approxPolyDP(contours[t], point, epsilon, true);
		if (point.size() == 3)
		{
			drawContours(Triangles, contours, t, Scalar(0, 0, 255), s_thickness, 8, Mat(), 0, Point());
			drawContours(Alls, contours, t, Scalar(255, 0, 0), s_thickness, 8, Mat(), 0, Point());
		}
		else if (point.size() == 4)
		{
			drawContours(Rects, contours, t, Scalar(0, 0, 255), s_thickness, 8, Mat(), 0, Point());
			drawContours(Alls, contours, t, Scalar(0, 255, 0), s_thickness, 8, Mat(), 0, Point());
		}
		else if (point.size() == 2)
		{
			drawContours(Lines, contours, t, Scalar(0, 0, 255), s_thickness, 8, Mat(), 0, Point());
			drawContours(Alls, contours, t, Scalar(0, 0, 255), s_thickness, 8, Mat(), 0, Point());
		}
		else
		{
			double area = contourArea(contours[t]);
			drawContours(Plines, contours, t, Scalar(0, 0, 255), s_thickness, 8, Mat(), 0, Point());
			drawContours(Alls, contours, t, Scalar(255, 0, 255), s_thickness, 8, Mat(), 0, Point());
		}
		cout << "边的数目：" << point.size() << endl;
	}

	Point origin(20, 25);
	int font_face = cv::FONT_HERSHEY_COMPLEX;
	double font_scale = 1;
	int thickness = 1;
	putText(Alls, "2", origin, font_face, font_scale, cv::Scalar(0, 0, 255), thickness, 8, 0);

	origin.x += 35;
	putText(Alls, "3", origin, font_face, font_scale, cv::Scalar(255, 0, 0), thickness, 8, 0);

	origin.x += 35;
	putText(Alls, "4", origin, font_face, font_scale, cv::Scalar(0, 255, 0), thickness, 8, 0);

	origin.x += 35;
	putText(Alls, ">4", origin, font_face, font_scale, cv::Scalar(255, 0, 255), thickness, 8, 0);

	imwrite(s_img_path + "1_Triangles.jpg", Triangles);
	imwrite(s_img_path + "1_Rects.jpg", Rects);
	imwrite(s_img_path + "1_Lines.jpg", Lines);
	imwrite(s_img_path + "1_Plines.jpg", Plines);
	imwrite(s_img_path + "1_Alls.jpg", Alls);

	waitKey(0);

	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
