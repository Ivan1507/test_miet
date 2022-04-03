//@file demo_Threshold.h
//@brief Contains demonstration of Threshold function in comparing with OpenCV
//@author Roman Golovanov
//@date 23 March 2016

#include "../stdafx.h"

#include <opencv2/opencv.hpp>
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



};

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
void demo_ConvexHull::execute()
{
    vx_coordinates2d_t vec[11]{ {1,2},{6,1}, {3,5}, { 5,3 }, {8,2},{6,3} ,{10,4},{5,3},{9,3},{7,6}, {12,1} };
    std::cout << "All points:\n";
    for (int i = 0; i < 11; ++i) {
        std::cout << "("<<vec[i].x << ", " << vec[i].y <<")" << '\n';
    }
    void* p = (void*)(vec);
    _vx_array src{
        p,
        11,
        VX_TYPE_COORDINATES2D
    }; 
    std::cout << "Convex Hull:\n";
    _vx_array *res=ref_ConvexHull(&src);
   
    vx_coordinates2d_t* beg = (vx_coordinates2d_t*)(p);
  



    cv::waitKey(0);
}

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
IDemoCasePtr CreateConvexHullDemo()
{
    return std::make_unique<demo_ConvexHull>();
}
