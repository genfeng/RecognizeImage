// RecognizeImage.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <windows.h>
#include <string>
#include <map>
#include "CImageProcess.h"

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

int Get_gray(Mat* im, int i, int j)
{
    return *im->ptr<uchar>(j, i);
}

//将 DEPTH_8U型二值图像进行细化  经典的Zhang并行快速细化算法
void Thin(const Mat &src, Mat &dst, const int iterations)
{
    const int height = src.rows - 1;
    const int width = src.cols - 1;

    //拷贝一个数组给另一个数组
    if (src.data != dst.data)
    {
        src.copyTo(dst);
    }


    int n = 0, i = 0, j = 0;
    Mat tmpImg;
    uchar *pU, *pC, *pD;
    BOOL isFinished = FALSE;

    for (n = 0; n < iterations; n++)
    {
        dst.copyTo(tmpImg);
        isFinished = FALSE;   //一次 先行后列扫描 开始
        //扫描过程一 开始
        for (i = 1; i < height; i++)
        {
            pU = tmpImg.ptr<uchar>(i - 1);
            pC = tmpImg.ptr<uchar>(i);
            pD = tmpImg.ptr<uchar>(i + 1);
            for (int j = 1; j < width; j++)
            {
                if (pC[j] > 0)
                {
                    int ap = 0;
                    int p2 = (pU[j] > 0);
                    int p3 = (pU[j + 1] > 0);
                    if (p2 == 0 && p3 == 1)
                    {
                        ap++;
                    }
                    int p4 = (pC[j + 1] > 0);
                    if (p3 == 0 && p4 == 1)
                    {
                        ap++;
                    }
                    int p5 = (pD[j + 1] > 0);
                    if (p4 == 0 && p5 == 1)
                    {
                        ap++;
                    }
                    int p6 = (pD[j] > 0);
                    if (p5 == 0 && p6 == 1)
                    {
                        ap++;
                    }
                    int p7 = (pD[j - 1] > 0);
                    if (p6 == 0 && p7 == 1)
                    {
                        ap++;
                    }
                    int p8 = (pC[j - 1] > 0);
                    if (p7 == 0 && p8 == 1)
                    {
                        ap++;
                    }
                    int p9 = (pU[j - 1] > 0);
                    if (p8 == 0 && p9 == 1)
                    {
                        ap++;
                    }
                    if (p9 == 0 && p2 == 1)
                    {
                        ap++;
                    }
                    if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) > 1 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) < 7)
                    {
                        if (ap == 1)
                        {
                            if ((p2*p4*p6 == 0) && (p4*p6*p8 == 0))
                            {
                                dst.ptr<uchar>(i)[j] = 0;
                                isFinished = TRUE;
                            }

                            //   if((p2*p4*p8==0)&&(p2*p6*p8==0))
                           //    {                           
                           //         dst.ptr<uchar>(i)[j]=0;
                           //         isFinished =TRUE;                            
                           //    }

                        }
                    }
                }

            } //扫描过程一 结束


            dst.copyTo(tmpImg);
            //扫描过程二 开始
            for (i = 1; i < height; i++)  //一次 先行后列扫描 开始
            {
                pU = tmpImg.ptr<uchar>(i - 1);
                pC = tmpImg.ptr<uchar>(i);
                pD = tmpImg.ptr<uchar>(i + 1);
                for (int j = 1; j < width; j++)
                {
                    if (pC[j] > 0)
                    {
                        int ap = 0;
                        int p2 = (pU[j] > 0);
                        int p3 = (pU[j + 1] > 0);
                        if (p2 == 0 && p3 == 1)
                        {
                            ap++;
                        }
                        int p4 = (pC[j + 1] > 0);
                        if (p3 == 0 && p4 == 1)
                        {
                            ap++;
                        }
                        int p5 = (pD[j + 1] > 0);
                        if (p4 == 0 && p5 == 1)
                        {
                            ap++;
                        }
                        int p6 = (pD[j] > 0);
                        if (p5 == 0 && p6 == 1)
                        {
                            ap++;
                        }
                        int p7 = (pD[j - 1] > 0);
                        if (p6 == 0 && p7 == 1)
                        {
                            ap++;
                        }
                        int p8 = (pC[j - 1] > 0);
                        if (p7 == 0 && p8 == 1)
                        {
                            ap++;
                        }
                        int p9 = (pU[j - 1] > 0);
                        if (p8 == 0 && p9 == 1)
                        {
                            ap++;
                        }
                        if (p9 == 0 && p2 == 1)
                        {
                            ap++;
                        }
                        if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) > 1 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) < 7)
                        {
                            if (ap == 1)
                            {
                                //   if((p2*p4*p6==0)&&(p4*p6*p8==0))
                                //   {                           
                               //         dst.ptr<uchar>(i)[j]=0;
                               //         isFinished =TRUE;                            
                               //    }

                                if ((p2*p4*p8 == 0) && (p2*p6*p8 == 0))
                                {
                                    dst.ptr<uchar>(i)[j] = 0;
                                    isFinished = TRUE;
                                }

                            }
                        }
                    }

                }

            } //一次 先行后列扫描完成
            //如果在扫描过程中没有删除点，则提前退出
            if (isFinished == FALSE)
            {
                break;
            }
        }

    }
}

//四周细化算法
void Refine(Mat& image)
{
    int p[8];
    int top = 1, down = 1, right = 1, left = 1;
    vector<Point> del;
    int grayvalue = 0;
    int height = image.rows;   //获取图像高度
    int width = image.cols;       //获取图像宽度
    Mat *im = reinterpret_cast<Mat*>((void*)&image);    //获取像素点信息
    //上下收缩
    for (int i = 1; i < height - 1; i++)
    {
        for (int j = 1; j < width - 1; j++)
        {
            grayvalue = Get_gray(im, j, i);  //获取指定点灰度值
            if (grayvalue != 0)   //判断中心点是否为前景
            {
                p[0] = (Get_gray(im, j + 1, i) == 0) ? 0 : 1;
                p[1] = (Get_gray(im, j + 1, i - 1) == 0) ? 0 : 1;
                p[2] = (Get_gray(im, j, i - 1) == 0) ? 0 : 1;
                p[3] = (Get_gray(im, j - 1, i - 1) == 0) ? 0 : 1;
                p[4] = (Get_gray(im, j - 1, i) == 0) ? 0 : 1;
                p[5] = (Get_gray(im, j - 1, i + 1) == 0) ? 0 : 1;
                p[6] = (Get_gray(im, j, i + 1) == 0) ? 0 : 1;
                p[7] = (Get_gray(im, j + 1, i + 1) == 0) ? 0 : 1;
                if (i < height - 2)
                    down = (Get_gray(im, j, i + 2) == 0) ? 0 : 1;
                else
                    down = 1;
                //  横向直线
                if (p[6] && (p[5] || p[7] || p[0] || p[4]) && !(p[1] || p[3]) && p[2] == 0 && down)
                {
                    del.push_back(Point(j, i));
                }
                if (p[2] && (p[1] || p[3] || p[0] || p[4]) && !(p[5] || p[7]) && p[6] == 0)
                {
                    del.push_back(Point(j, i));
                }
            }
        }
    }

    for (int i = 1; i < height - 2; i++)
    {
        grayvalue = Get_gray(im, 0, i);
        if (grayvalue != 0)
        {
            if (Get_gray(im, 0, i - 1) && Get_gray(im, 1, i - 1) && Get_gray(im, 0, i + 1) == 0 && Get_gray(im, 1, i) == 0) //上2，上1，右上1，下1=0，右1=0
            {
                del.push_back(Point(0, i));
            }
            if (Get_gray(im, 0, i - 1) == 0 && Get_gray(im, 1, i + 1) && Get_gray(im, 1, i) == 0 && Get_gray(im, 0, i + 2))//上1=0，下1，右下1，右1=0，下2
            {
                del.push_back(Point(0, i));
            }
        }
        if (grayvalue != 0)
        {
            if (Get_gray(im, width - 1, i - 1) && Get_gray(im, width - 2, i - 1) && Get_gray(im, width - 1, i + 1) == 0 && Get_gray(im, width - 2, i) == 0) //上2，上1，左上1，下1=0，左1=0
            {
                del.push_back(Point(width - 1, i));
            }
            if (Get_gray(im, width - 1, i - 1) == 0 && Get_gray(im, width - 2, i + 1) && Get_gray(im, width - 2, i) == 0 && Get_gray(im, width - 1, i + 2))//上1=0，下1，左下1，左1=0，下2
            {
                del.push_back(Point(width - 1, i));
            }
        }
    }
    for (int i = 0; i < del.size(); i++)
    {
        uchar* data = image.ptr<uchar>(del[i].y);
        data[del[i].x] = 0;
    }

    //左右收缩
    for (int i = 1; i < height - 1; i++)
    {
        for (int j = 1; j < width - 1; j++)
        {
            grayvalue = Get_gray(im, j, i);  //获取指定点灰度值
            if (grayvalue != 0)   //判断中心点是否为前景
            {
                p[0] = (Get_gray(im, j + 1, i) == 0) ? 0 : 1;
                p[1] = (Get_gray(im, j + 1, i - 1) == 0) ? 0 : 1;
                p[2] = (Get_gray(im, j, i - 1) == 0) ? 0 : 1;
                p[3] = (Get_gray(im, j - 1, i - 1) == 0) ? 0 : 1;
                p[4] = (Get_gray(im, j - 1, i) == 0) ? 0 : 1;
                p[5] = (Get_gray(im, j - 1, i + 1) == 0) ? 0 : 1;
                p[6] = (Get_gray(im, j, i + 1) == 0) ? 0 : 1;
                p[7] = (Get_gray(im, j + 1, i + 1) == 0) ? 0 : 1;
                if (j < width - 2)
                    right = (Get_gray(im, j + 2, i) == 0) ? 0 : 1;
                else
                    right = 1;


                //竖直线
                if (p[0] && (p[1] || p[7] || p[2] || p[6]) && !(p[3] || p[5]) && p[4] == 0 && right)
                {
                    del.push_back(Point(j, i));
                }
                if (p[4] && (p[3] || p[5] || p[2] || p[6]) && !(p[1] || p[7]) && p[0] == 0)
                {
                    del.push_back(Point(j, i));
                }

            }
        }
    }

    for (int j = 1; j < width - 2; j++)
    {
        grayvalue = Get_gray(im, j, 0);
        if (grayvalue != 0)
        {
            if (Get_gray(im, j - 1, 0) == 0 && Get_gray(im, j + 1, 0) && Get_gray(im, j + 2, 0) && Get_gray(im, j, 1) == 0 && Get_gray(im, j + 1, 1)) //左1=0，右1，右2，下1=0，右下1
            {
                del.push_back(Point(j, 0));
            }
            if (Get_gray(im, j - 1, 0) && Get_gray(im, j + 1, 0) == 0 && Get_gray(im, j, 1) == 0 && Get_gray(im, j - 1, 1))//左1，右1=0，下1=0，左下1
            {
                del.push_back(Point(j, 0));
            }
        }
    }
    for (int j = 1; j < width - 2; j++)
    {
        grayvalue = Get_gray(im, j, height - 1);
        if (grayvalue != 0)
        {
            if (Get_gray(im, j - 1, height - 1) == 0 && Get_gray(im, j + 1, height - 1) && Get_gray(im, j + 2, height - 1) && Get_gray(im, j, height - 2) == 0 && Get_gray(im, j + 1, height - 2)) //左1=0，右1，右2，下1=0，右下1
            {
                del.push_back(Point(j, height - 1));
            }
            if (Get_gray(im, j - 1, height - 1) && Get_gray(im, j + 1, height - 1) == 0 && Get_gray(im, j, height - 2) == 0 && Get_gray(im, j - 1, height - 2))//左1，右1=0，下1=0，左下1
            {
                del.push_back(Point(j, height - 1));
            }
        }
    }

    for (int i = 0; i < del.size(); i++)
    {
        uchar* data = image.ptr<uchar>(del[i].y);
        data[del[i].x] = 0;
    }
}

// int main()
// {
//     String s_img_path = GetExePath() + "\\..\\ImgTest\\";
// 
//     Mat src = imread(s_img_path + "2.png");
//     if (src.empty())
//     {
//         src = imread(s_img_path + "1.png");
//         if (src.empty())
//         {
//             printf("Could not load image...");
//             return -1;
//         }
//     }
//     Mat src_gray, binary;
//     Mat Triangles(src.rows, src.cols, CV_8UC3, Scalar(0, 0, 0)), Rects = Triangles.clone();
//     Mat BigCircles = Triangles.clone(), Lines = Triangles.clone(), Plines = Triangles.clone();
//     Mat Alls = src.clone();
// 
//     //二值化
//     cvtColor(src, src_gray, COLOR_BGR2GRAY);
//     Mat src_bit;
//     bilateralFilter(src_gray, src_bit, 10, 20, 20);
//     //     Mat src_dilate;
//     //     Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
//     //     //高级形态学处理，调用这个函数就可以了，具体要选择哪种操作，就修改第三个参数就可以了
//     //     morphologyEx(src_gray, src_dilate, MORPH_DILATE, element);
//     //     imwrite(s_img_path + "src_dilate.jpg", src_dilate);
//     threshold(src_bit, binary, 0, 255, THRESH_BINARY | THRESH_OTSU);
//     binary = ~binary;
//     Refine(binary);
//     imwrite(s_img_path + "2_binary.jpg", binary);
// 
//     //发现轮廓
//     vector<vector<Point>> contours;
//     vector<Point> point;
//     vector<Vec4i> hireachy;
//     findContours(binary, contours, hireachy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());
// 
//     Point test1(450, 2630);
//     Point test2(1194, 2933);
//     Point test3(1080, 1552);
//     int min_test1(INT_MAX), min_test2(INT_MAX), min_test3(INT_MAX);
//     Point p_test1(450, 2630);
//     Point p_test2(1194, 2933);
//     Point p_test3(1080, 1552);
//     vector<vector<Point>> min_contours(3);
//     // 绘制出所有轮廓
//     static int s_thickness = 1;
//     for (int t = 0; t < (int)contours.size(); t++)
//     {
//         vector<Point>& contour = contours[t];
//         Scalar color;
// 
//         cv::Moments moment = cv::moments(cv::Mat(contour));
//         if (moment.m00 > 0.000001) {
//             Point temp(moment.m10 / moment.m00, moment.m01 / moment.m00);
//             int dis1 = pow(temp.x - test1.x, 2) + pow(temp.y - test1.y, 2);
//             if (dis1 < min_test1) {
//                 min_contours[0] = contour;
//                 min_test1 = dis1;
//                 p_test1 = temp;
//             }
//             int dis2 = pow(temp.x - test2.x, 2) + pow(temp.y - test2.y, 2);
//             if (dis2 < min_test2) {
//                 min_contours[1] = contour;
//                 min_test2 = dis2;
//                 p_test2 = temp;
//             }
//             int dis3 = pow(temp.x - test3.x, 2) + pow(temp.y - test3.y, 2);
//             if (dis3 < min_test3) {
//                 min_contours[2] = contour;
//                 min_test3 = dis3;
//                 p_test3 = temp;
//             }
//         }
//         continue;
// 
//         double epsilon = 0.01*arcLength(contour, true);
//         approxPolyDP(contour, point, epsilon, true);
//         if (point.size() == 3)
//         {
//             drawContours(Triangles, contours, t, Scalar(0, 0, 255), s_thickness, 8, Mat(), 0, Point());
// 
//             color = Scalar(255, 0, 0);
//             drawContours(Alls, contours, t, color, s_thickness, 8, Mat(), 0, Point());
//         }
//         else if (point.size() == 4)
//         {
//             drawContours(Rects, contours, t, Scalar(0, 0, 255), s_thickness, 8, Mat(), 0, Point());
// 
//             color = Scalar(0, 255, 0);
//             drawContours(Alls, contours, t, color, s_thickness, 8, Mat(), 0, Point());
//         }
//         else if (point.size() == 2)
//         {
//             drawContours(Lines, contours, t, Scalar(0, 0, 255), s_thickness, 8, Mat(), 0, Point());
// 
//             color = Scalar(0, 0, 255);
//             drawContours(Alls, contours, t, color, s_thickness, 8, Mat(), 0, Point());
//         }
//         else
//         {
//             double area = contourArea(contour);
//             drawContours(Plines, contours, t, Scalar(0, 0, 255), s_thickness, 8, Mat(), 0, Point());
// 
//             color = Scalar(255, 0, 255);
//             drawContours(Alls, contours, t, color, s_thickness, 8, Mat(), 0, Point());
//         }
//         cout << "边的数目：" << point.size() << endl;
//     }
//     drawContours(Alls, min_contours, 0, Scalar(0, 0, 255), s_thickness, 8, Mat(), 0, Point());
//     drawContours(Alls, min_contours, 1, Scalar(0, 0, 255), s_thickness, 8, Mat(), 0, Point());
//     drawContours(Alls, min_contours, 2, Scalar(0, 0, 255), s_thickness, 8, Mat(), 0, Point());
// 
//     int font_face = cv::FONT_HERSHEY_COMPLEX;
//     double font_scale = 1;
//     int thickness = 1;
//     putText(Alls, "1", test1, font_face, font_scale, cv::Scalar(0, 0, 255), thickness, 8, 0);
//     putText(Alls, "2", test2, font_face, font_scale, cv::Scalar(255, 0, 0), thickness, 8, 0);
//     putText(Alls, "3", test3, font_face, font_scale, cv::Scalar(0, 255, 0), thickness, 8, 0);
// 
//     putText(Alls, "1", p_test1, font_face, font_scale, cv::Scalar(0, 0, 255), thickness, 8, 0);
//     putText(Alls, "2", p_test2, font_face, font_scale, cv::Scalar(255, 0, 0), thickness, 8, 0);
//     putText(Alls, "3", p_test3, font_face, font_scale, cv::Scalar(0, 255, 0), thickness, 8, 0);
// 
//     imwrite(s_img_path + "1_Triangles.jpg", Triangles);
//     imwrite(s_img_path + "1_Rects.jpg", Rects);
//     imwrite(s_img_path + "1_Lines.jpg", Lines);
//     imwrite(s_img_path + "1_Plines.jpg", Plines);
//     imwrite(s_img_path + "1_Alls.jpg", Alls);
// 
//     waitKey(0);
// 
//     return 0;
// }

int main()
{
    String s_img_path = GetExePath() + "\\..\\ImgTest\\";
    CImageProcess image_process(s_img_path + "1.png");
    Point test1(450, 2630);
    Point test2(1194, 2933);
    Point test3(1080, 1552);
    vector<vector<Point>> min_contours(3);
    min_contours[0] = image_process.GetNearestContourByPoint(test1);
    min_contours[1] = image_process.GetNearestContourByPoint(test2);
    min_contours[2] = image_process.GetNearestContourByPoint(test3);

    Mat Alls(image_process.GetClone());
    int s_thickness = 1;
    drawContours(Alls, min_contours, 0, Scalar(0, 0, 255), s_thickness, 8, Mat(), 0, Point());
    drawContours(Alls, min_contours, 1, Scalar(0, 0, 255), s_thickness, 8, Mat(), 0, Point());
    drawContours(Alls, min_contours, 2, Scalar(0, 0, 255), s_thickness, 8, Mat(), 0, Point());

    int font_face = cv::FONT_HERSHEY_COMPLEX;
    double font_scale = 1;
    int thickness = 1;
    putText(Alls, "1", test1, font_face, font_scale, cv::Scalar(0, 0, 255), thickness, 8, 0);
    putText(Alls, "2", test2, font_face, font_scale, cv::Scalar(255, 0, 0), thickness, 8, 0);
    putText(Alls, "3", test3, font_face, font_scale, cv::Scalar(0, 255, 0), thickness, 8, 0);
    imwrite(s_img_path + "1_Alls.jpg", Alls);
    waitKey(0);

    return 0;
}