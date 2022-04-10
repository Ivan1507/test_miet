//@file demo_Threshold.h
//@brief Contains demonstration of Threshold function in comparing with OpenCV
//@author Roman Golovanov
//@date 23 March 2016

#include "../stdafx.h"

#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <vector>

extern "C"
{
#include <Kernels/ref.h>
#include <types.h>
}

#include "../DemoEngine.h"
///////////////////////////////////////////////////////////////////////////////
//@brief Demonstration of Threshold function

class demo_ConvexHull : public IDemoCase
{
public:
    ///@brief default ctor

    demo_ConvexHull() :m_threshold(100) {

    }
    ///@see IDemoCase::ReplyName
    virtual std::string ReplyName() const override
    {
        return "ConvexHull";
    }

private:
    ///@see IDemoCase::execute
    virtual void execute() override;

    ///@brief provide interactive demo

    static void applyParameters(int pos, void* data);

private:
    int m_threshold;
    cv::Mat m_srcImage;

};

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////


namespace
{
    const std::string m_openVXWindow = "openVX";
    const std::string m_openCVWindow = "openCV";
    const std::string m_originalWindow = "original";
    const std::string m_diffWindow = "Diff of " + m_openVXWindow + " and " + m_openCVWindow;
}


void demo_ConvexHull::execute()
{
   
    cv::namedWindow(m_originalWindow, CV_WINDOW_NORMAL);
    cv::namedWindow(m_openVXWindow, CV_WINDOW_NORMAL);
    cv::namedWindow(m_openCVWindow, CV_WINDOW_NORMAL);
    cv::namedWindow(m_diffWindow, CV_WINDOW_NORMAL);

    const std::string imgPath = "C:\\dev\\test_miet\\Image\\apple.png";
    m_srcImage = cv::imread(imgPath, CV_LOAD_IMAGE_GRAYSCALE);
    cv::imshow(m_originalWindow, m_srcImage);
   

    cv::createTrackbar("ConvexHull:", m_originalWindow, &m_threshold, 255, applyParameters, static_cast<void*>(this));

    applyParameters(m_threshold,this);

    cv::waitKey(0);
}

void demo_ConvexHull::applyParameters(int, void* data)
{
    auto demo = static_cast<demo_ConvexHull*>(data);
    const cv::Size imgSize(demo->m_srcImage.cols, demo->m_srcImage.rows);

    cv::RNG rng(12345);
    ///@{ OPENCV
 
    cv::Mat canny_output;
    cv::Canny(demo->m_srcImage, canny_output, demo->m_threshold, demo->m_threshold * 2);
    std::vector<std::vector<cv::Point>> contours;

   findContours(canny_output, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
  
   std::vector<std::vector<cv::Point>> hull(contours.size());
   for (size_t i = 0; i < contours.size(); ++i)
   {
        cv::convexHull(contours[i], hull[i]);
   }
   /*std::cout << "opencv hull size: "<<hull[0].size() << '\n';
   for (auto&& v : hull[0])
       std::cout << v.x << " " << v.y << '\n';*/
   cv::Mat drawing = cv::Mat::zeros(canny_output.size(), CV_8UC3);//result of openCv Convex Hull
   for (size_t i = 0; i < contours.size(); ++i)
   {
       cv::Scalar color=cv::Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
       drawContours(drawing, contours, (int)i, color);
       drawContours(drawing, hull, (int)i, color);
   }
   imshow(m_openCVWindow, drawing);
   


   ///@{ OPENVX
   cv::Mat my_canny;
   cv::Canny(demo->m_srcImage, my_canny, demo->m_threshold, demo->m_threshold * 2);
   std::vector<std::vector<cv::Point>> my_contours;

   findContours(my_canny, my_contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

   std::vector<std::vector<cv::Point>> myhull;
   myhull.reserve(my_contours.size());
   for (size_t i = 0; i < my_contours.size(); ++i) {
       void* p = (void*)&(my_contours[i][0]);
       size_t sz = my_contours[i].size();
       void* p1=new vx_coordinates2d_t[sz];
       _vx_array src{
           p,
           sz,
           VX_TYPE_COORDINATES2D
       };
       vx_array res = ref_ConvexHull(&src,p1);
       cv::Point* beg = (cv::Point*)(res->data);
       std::vector<cv::Point> res_vec(beg, beg + res->size);
       myhull.push_back(std::move(res_vec));
       delete[] p1;
   }
  /* std::cout << "myhuul size: " << myhull[0].size() << '\n';
   for (auto&& v : myhull[0])
       std::cout << v.x << " " << v.y << "\n";*/

  

   cv::Mat openvX = cv::Mat::zeros(my_canny.size(), CV_8UC3);//result of my Convex Hull
   for (size_t i = 0; i < my_contours.size(); ++i)
   {
       cv::Scalar color = cv::Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
       drawContours(openvX, my_contours, (int)i, color);
       drawContours(openvX, myhull, (int)i, color);
   }
   imshow(m_openVXWindow, openvX);
  
    cv::Mat diffImage(imgSize, CV_8UC1);
    cv::absdiff(openvX,drawing, diffImage);
    cv::imshow(m_diffWindow, diffImage);

   
}



///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
IDemoCasePtr CreateConvexHullDemo()
{
    return std::make_unique<demo_ConvexHull>();
}
