#include "detect_light.h"
#include<vector>

bool test_aspectRatio(const std::vector<cv::Point>& contour){       //检验长宽比
cv::RotatedRect minRect = cv::minAreaRect(contour);
float length = std::max(minRect.size.width, minRect.size.height); 
float width = std::min(minRect.size.width, minRect.size.height);  
float aspectRatio = length / width;
    if(aspectRatio>=3&&aspectRatio<=20)  return true;
    else return false;
}



float compute_area_ratio(const std::vector<cv::Point>& contour) {   //计算与最小外接矩形的相似程度
    cv::RotatedRect minAreaRect;

    minAreaRect = cv::minAreaRect(contour);

    float area = cv::contourArea(contour);

    cv::Size rectSize = minAreaRect.size;
    double area_2 = rectSize.width * rectSize.height;
    if(area_2==0)
    {
    return 0.f;
    }
    else  return static_cast<float>(area/area_2);
}




std::vector<std::vector<cv::Point>> detect_light(cv::Mat src)
{
    cv::Mat gray, dst;
    

    cv::cvtColor(src, gray,cv::COLOR_BGR2GRAY);
    cv::threshold(gray,dst,100,255,cv::THRESH_BINARY);


    std::vector<std::vector<cv::Point>> contours_1;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(dst,contours_1,hierarchy,cv::RETR_EXTERNAL,cv::CHAIN_APPROX_SIMPLE);

    std::vector<std::vector<cv::Point>> contours_2;
    for(auto& contour:contours_1)
		{
            float area_ratio=compute_area_ratio(contour);
            float area = cv::contourArea(contour);
            
            if(area>=10&&area_ratio>0.4&&test_aspectRatio(contour))
            {
                contours_2.push_back(contour);
            }
        }



  
    return contours_2;
}
