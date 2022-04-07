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
    cv::Mat src_grey;

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
   /* vx_coordinates2d_t vec[11]{ {1,2},{6,1}, {3,5}, { 5,3 }, {8,2},{6,3} ,{10,4},{5,3},{9,3},{7,6}, {12,1} };
    std::cout << "All points:\n";
    for (int i = 0; i < 11; ++i) {
        std::cout << "("<<vec[i].x << ", " << vec[i].y <<")" << '\n';
    }*/
 
  
    cv::namedWindow(m_originalWindow, CV_WINDOW_NORMAL);
   /* cv::namedWindow(m_openVXWindow, CV_WINDOW_NORMAL);
    cv::namedWindow(m_openCVWindow, CV_WINDOW_NORMAL);
    cv::namedWindow(m_diffWindow, CV_WINDOW_NORMAL);*/



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
    int thresh = 100;
    cv::RNG rng(12345);
    ///@{ OPENCV
 
    cv::Mat canny_output;
    cv::Canny(demo->m_srcImage, canny_output, thresh, thresh * 2);
    std::vector<std::vector<cv::Point>> contours;

   findContours(canny_output, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
  
   std::vector<std::vector<cv::Point>>hull (contours.size());
   std::cout<<contours.size()<<'\n';
   for (size_t i = 0; i < contours.size(); ++i)
   {
        cv::convexHull(contours[i], hull[i]);
   }
   for (int i = 0; i < hull[0].size(); ++i) {
       std::cout << hull[0][i].x << " " << hull[0][i].y << '\n';
   }
   cv::Mat drawing = cv::Mat::zeros(canny_output.size(), CV_8UC3);//result of openCv Convex Hull
   for (size_t i = 0; i < contours.size(); ++i)
   {
       cv::Scalar color=cv::Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
       drawContours(drawing, contours, (int)i, color);
       drawContours(drawing, hull, (int)i, color);
   }
   imshow("opencv", drawing);
   


   //openvx
   void* p = (void*)&(contours[0][0]);
   size_t sz = contours[0].size();
   _vx_array src{
       p,
       sz,
       VX_TYPE_COORDINATES2D
   };
   void* p1 = new char[sz * sizeof(vx_coordinates2d_t)];
   size_t res_sz;
    ref_ConvexHull(&src,p1,res_sz);
    _vx_array res{
        p1,
        res_sz,
        VX_TYPE_COORDINATES2D
    };
    std::cout << '\n' << res_sz << '\n';
   cv::Point* beg = (cv::Point*)(src.data);
   cv::Point* beg_res = (cv::Point*)(res.data);
   std::cout << beg_res->x << " " << beg_res->y << "\n";
   for (int i = 0; i < res.size; ++i) {
       std::cout << (beg_res+i)->x << " " << (beg_res+i)->y << "\n";
   }
   std::vector<cv::Point> mycontours(beg, beg + sz);
   std::vector<cv::Point> myhull(beg_res,beg_res+res_sz);
   for (int i = 0; i < myhull.size(); ++i) {
       std::cout << myhull[i].x << " " << myhull[i].y << "\n";
   }
  /* for (size_t i = 0; i < src.size; ++i) {
       std::cout << (beg + i)->x << " " << (beg + i)->y << '\n';
   }*/

   cv::Mat openvX = cv::Mat::zeros(canny_output.size(), CV_8UC3);//result of openCv Convex Hull
   for (size_t i = 0; i < mycontours.size(); ++i)
   {

       cv::Scalar color = cv::Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
       drawContours(openvX, mycontours, (int)i, color);
       drawContours(openvX, myhull, (int)i, color);
   }
   imshow("opencv", openvX);
  


   
}



///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
IDemoCasePtr CreateConvexHullDemo()
{
    return std::make_unique<demo_ConvexHull>();
}
