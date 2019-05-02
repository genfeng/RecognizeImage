#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
#include <windows.h>
#include <string>
#include <map>
#include <vector>

class CImageProcess
{
public:
    CImageProcess(const std::string& file_path);
    ~CImageProcess();

public:
    const std::vector<std::vector<cv::Point>>& GetAllContours() const;
    const std::vector<cv::Point> GetNearestContourByPoint(const cv::Point& point) const;
    int GetImageWidth() const;
    int GetImageHeight() const;
    cv::Mat GetClone() const;

private:
    void CalculateContours();
    void ThinBinaryImage(cv::Mat& image);
    void ThinBinaryImage2(const cv::Mat& src, cv::Mat& dst, const int iterations);
    int GetGray(cv::Mat* im, int i, int j)
    {
        return *im->ptr<uchar>(j, i);
    }

private:
    std::string path_;
    cv::Mat src_;
    std::vector<std::vector<cv::Point>> contours_;
};

