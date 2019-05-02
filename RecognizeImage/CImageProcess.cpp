#include "pch.h"
#include "CImageProcess.h"

using namespace cv;
using namespace std;

CImageProcess::CImageProcess(const std::string& file_path)
{
    path_ = file_path;
    CalculateContours();
}

CImageProcess::~CImageProcess()
{
}

const std::vector<std::vector<cv::Point>>& CImageProcess::GetAllContours() const
{
    return contours_;
}

const std::vector<cv::Point> CImageProcess::GetNearestContourByPoint(const cv::Point& point) const
{
    int min_distance(INT_MAX);
    vector<cv::Point> nearest_contour;
    // ���Ƴ���������
    for (unsigned t = 0; t < contours_.size(); t++)
    {
        const vector<Point>& contour = contours_[t];
        cv::Moments moment = cv::moments(cv::Mat(contour));
        if (moment.m00 > 0.000001) {
            Point temp(moment.m10 / moment.m00, moment.m01 / moment.m00);
            int distance = pow(temp.x - point.x, 2) + pow(temp.y - point.y, 2);
            if (distance < min_distance) {
                nearest_contour = contour;
                min_distance = distance;
            }
        }
    }
    return nearest_contour;
}

int CImageProcess::GetImageWidth() const
{
    return src_.cols;
}

int CImageProcess::GetImageHeight() const
{
    return src_.rows;
}

cv::Mat CImageProcess::GetClone() const
{
    return src_.clone();
}

void CImageProcess::CalculateContours()
{
    src_ = imread(path_);
    if (src_.empty())
    {
        cout << "path:" << path_ << " Could not load image...";
        assert(false);
        return;
    }
    Mat src_gray, binary;
    //�Ҷ�ת��
    cvtColor(src_, src_gray, COLOR_BGR2GRAY);
    Mat src_bilateral;
    bilateralFilter(src_gray, src_bilateral, 10, 20, 20);
    //     Mat src_dilate;
    //     Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
    //     //�߼���̬ѧ����������������Ϳ����ˣ�����Ҫѡ�����ֲ��������޸ĵ����������Ϳ�����
    //     morphologyEx(src_gray, src_dilate, MORPH_DILATE, element);
    //     imwrite(s_img_path + "src_dilate.jpg", src_dilate);
    threshold(src_bilateral, binary, 0, 255, THRESH_BINARY | THRESH_OTSU);
    binary = ~binary;
    ThinBinaryImage(binary);

    //��������
    vector<Vec4i> hireachy;
    findContours(binary, contours_, hireachy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());
}

//����ϸ���㷨
void CImageProcess::ThinBinaryImage(cv::Mat& image)
{
    int p[8];
    int top = 1, down = 1, right = 1, left = 1;
    vector<Point> del;
    int grayvalue = 0;
    int height = image.rows;   //��ȡͼ��߶�
    int width = image.cols;       //��ȡͼ����
    Mat *im = reinterpret_cast<Mat*>((void*)&image);    //��ȡ���ص���Ϣ
    //��������
    for (int i = 1; i < height - 1; i++)
    {
        for (int j = 1; j < width - 1; j++)
        {
            grayvalue = GetGray(im, j, i);  //��ȡָ����Ҷ�ֵ
            if (grayvalue != 0)   //�ж����ĵ��Ƿ�Ϊǰ��
            {
                p[0] = (GetGray(im, j + 1, i) == 0) ? 0 : 1;
                p[1] = (GetGray(im, j + 1, i - 1) == 0) ? 0 : 1;
                p[2] = (GetGray(im, j, i - 1) == 0) ? 0 : 1;
                p[3] = (GetGray(im, j - 1, i - 1) == 0) ? 0 : 1;
                p[4] = (GetGray(im, j - 1, i) == 0) ? 0 : 1;
                p[5] = (GetGray(im, j - 1, i + 1) == 0) ? 0 : 1;
                p[6] = (GetGray(im, j, i + 1) == 0) ? 0 : 1;
                p[7] = (GetGray(im, j + 1, i + 1) == 0) ? 0 : 1;
                if (i < height - 2)
                    down = (GetGray(im, j, i + 2) == 0) ? 0 : 1;
                else
                    down = 1;
                //  ����ֱ��
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
        grayvalue = GetGray(im, 0, i);
        if (grayvalue != 0)
        {
            if (GetGray(im, 0, i - 1) && GetGray(im, 1, i - 1) && GetGray(im, 0, i + 1) == 0 && GetGray(im, 1, i) == 0) //��2����1������1����1=0����1=0
            {
                del.push_back(Point(0, i));
            }
            if (GetGray(im, 0, i - 1) == 0 && GetGray(im, 1, i + 1) && GetGray(im, 1, i) == 0 && GetGray(im, 0, i + 2))//��1=0����1������1����1=0����2
            {
                del.push_back(Point(0, i));
            }
        }
        if (grayvalue != 0)
        {
            if (GetGray(im, width - 1, i - 1) && GetGray(im, width - 2, i - 1) && GetGray(im, width - 1, i + 1) == 0 && GetGray(im, width - 2, i) == 0) //��2����1������1����1=0����1=0
            {
                del.push_back(Point(width - 1, i));
            }
            if (GetGray(im, width - 1, i - 1) == 0 && GetGray(im, width - 2, i + 1) && GetGray(im, width - 2, i) == 0 && GetGray(im, width - 1, i + 2))//��1=0����1������1����1=0����2
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

    //��������
    for (int i = 1; i < height - 1; i++)
    {
        for (int j = 1; j < width - 1; j++)
        {
            grayvalue = GetGray(im, j, i);  //��ȡָ����Ҷ�ֵ
            if (grayvalue != 0)   //�ж����ĵ��Ƿ�Ϊǰ��
            {
                p[0] = (GetGray(im, j + 1, i) == 0) ? 0 : 1;
                p[1] = (GetGray(im, j + 1, i - 1) == 0) ? 0 : 1;
                p[2] = (GetGray(im, j, i - 1) == 0) ? 0 : 1;
                p[3] = (GetGray(im, j - 1, i - 1) == 0) ? 0 : 1;
                p[4] = (GetGray(im, j - 1, i) == 0) ? 0 : 1;
                p[5] = (GetGray(im, j - 1, i + 1) == 0) ? 0 : 1;
                p[6] = (GetGray(im, j, i + 1) == 0) ? 0 : 1;
                p[7] = (GetGray(im, j + 1, i + 1) == 0) ? 0 : 1;
                if (j < width - 2)
                    right = (GetGray(im, j + 2, i) == 0) ? 0 : 1;
                else
                    right = 1;


                //��ֱ��
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
        grayvalue = GetGray(im, j, 0);
        if (grayvalue != 0)
        {
            if (GetGray(im, j - 1, 0) == 0 && GetGray(im, j + 1, 0) && GetGray(im, j + 2, 0) && GetGray(im, j, 1) == 0 && GetGray(im, j + 1, 1)) //��1=0����1����2����1=0������1
            {
                del.push_back(Point(j, 0));
            }
            if (GetGray(im, j - 1, 0) && GetGray(im, j + 1, 0) == 0 && GetGray(im, j, 1) == 0 && GetGray(im, j - 1, 1))//��1����1=0����1=0������1
            {
                del.push_back(Point(j, 0));
            }
        }
    }
    for (int j = 1; j < width - 2; j++)
    {
        grayvalue = GetGray(im, j, height - 1);
        if (grayvalue != 0)
        {
            if (GetGray(im, j - 1, height - 1) == 0 && GetGray(im, j + 1, height - 1) && GetGray(im, j + 2, height - 1) && GetGray(im, j, height - 2) == 0 && GetGray(im, j + 1, height - 2)) //��1=0����1����2����1=0������1
            {
                del.push_back(Point(j, height - 1));
            }
            if (GetGray(im, j - 1, height - 1) && GetGray(im, j + 1, height - 1) == 0 && GetGray(im, j, height - 2) == 0 && GetGray(im, j - 1, height - 2))//��1����1=0����1=0������1
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

//�� DEPTH_8U�Ͷ�ֵͼ�����ϸ��  �����Zhang���п���ϸ���㷨
void CImageProcess::ThinBinaryImage2(const cv::Mat& src, cv::Mat& dst, const int iterations)
{
    const int height = src.rows - 1;
    const int width = src.cols - 1;

    //����һ���������һ������
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
        isFinished = FALSE;   //һ�� ���к���ɨ�� ��ʼ
        //ɨ�����һ ��ʼ
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

            } //ɨ�����һ ����


            dst.copyTo(tmpImg);
            //ɨ����̶� ��ʼ
            for (i = 1; i < height; i++)  //һ�� ���к���ɨ�� ��ʼ
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

            } //һ�� ���к���ɨ�����
            //�����ɨ�������û��ɾ���㣬����ǰ�˳�
            if (isFinished == FALSE)
            {
                break;
            }
        }

    }
}
