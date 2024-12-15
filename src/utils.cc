#include"utils.h"
#include<vector>
#include<cmath>


bool test_Parallel(std::vector<cv::Point> contour1,std::vector<cv::Point> contour2, float angleThreshold)
{
    // 1. 拟合两条轮廓的方向向量
    cv::Vec4f line1, line2;
    cv::fitLine(contour1, line1, cv::DIST_L2, 0, 0.01, 0.01);
    cv::fitLine(contour2, line2, cv::DIST_L2, 0, 0.01, 0.01);

    // 2. 提取方向向量
    cv::Point2f dir1(line1[0], line1[1]); // (vx, vy)
    cv::Point2f dir2(line2[0], line2[1]); // (vx, vy)

    // 3. 计算方向向量的夹角
    float dotProduct = dir1.x * dir2.x + dir1.y * dir2.y;
    float magnitude1 = std::sqrt(dir1.x * dir1.x + dir1.y * dir1.y);
    float magnitude2 = std::sqrt(dir2.x * dir2.x + dir2.y * dir2.y);
    float cosTheta = dotProduct / (magnitude1 * magnitude2);

    // 计算角度（转为弧度后转角度）
    float angle = std::acos(std::abs(cosTheta)) * 180.0 / CV_PI;

    // 4. 判断夹角是否小于阈值
    if (angle > angleThreshold||line1[0]>0.5||line2[0]>0.5||line1[0]<-0.5||line2[0]<-0.5) {
        return false; // 角度太大，轮廓不平行
    }

    // 5. 检查轮廓的相对位置
    cv::Point2f center1, center2;
    cv::Moments moments1 = cv::moments(contour1);
    cv::Moments moments2 = cv::moments(contour2);
    center1 = cv::Point2f(moments1.m10 / moments1.m00, moments1.m01 / moments1.m00);
    center2 = cv::Point2f(moments2.m10 / moments2.m00, moments2.m01 / moments2.m00);

    float distance = std::abs((center2.x - center1.x) * dir1.y - (center2.y - center1.y) * dir1.x) / magnitude1;

    // 自定义位置条件，确保轮廓中心间的距离合理
    float maxDistanceThreshold = 400.0; // 最大允许的平行间距
    if (distance > maxDistanceThreshold) {
        return false; // 距离太远，不认为平行
    }

    return true; // 平行
}


float compute_aspectRatio(const std::vector<cv::Point>& contour){   
    cv::RotatedRect minRect = cv::minAreaRect(contour);
    float length = std::max(minRect.size.width, minRect.size.height); 
    float width = std::min(minRect.size.width, minRect.size.height);  
    float aspectRatio = length / width;
    return aspectRatio;
}

bool test_pair(std::vector<cv::Point> contour_1,std::vector<cv::Point> contour_2)
{
    float aspectRatio_1=compute_aspectRatio(contour_1);
    float aspectRatio_2=compute_aspectRatio(contour_2);
    float area_1=cv::contourArea(contour_1);
    float area_2=cv::contourArea(contour_2);

    if(aspectRatio_1-aspectRatio_2<=10&&aspectRatio_2-aspectRatio_1<=10&&
    area_1-area_2<=1500&&area_2-area_1<=1500&&area_1>10&&area_2>10
    &&test_Parallel(contour_1,contour_2,5.0))
    {
        return 1;
    }
    else return 0;
}

std::vector<std::pair<std::vector<cv::Point>, std::vector<cv::Point>>> pair_light(std::vector<std::vector<cv::Point>> contours)
{
    std::vector<std::pair<std::vector<cv::Point>, std::vector<cv::Point>>> contourpairs;
    for(int i=0;i<contours.size();i++)
    {
        for(int j=i+1;j<contours.size();j++)
        {
            if(test_pair(contours[i],contours[j]))
            {
                contourpairs.push_back(std::make_pair(contours[i],contours[j]));
            }
        }
    }
    return contourpairs;
}

    std::tuple<cv::Mat, std::vector<std::vector<cv::Point2f>>> LINE_Rect(std::vector<std::pair<std::vector<cv::Point>, std::vector<cv::Point>>>contourPairs,cv::Mat img)
    {
        std::vector<std::vector<cv::Point2f>> pointsArray;
        for(int i=0;i<contourPairs.size();i++) 
        {
            std::pair<std::vector<cv::Point>, std::vector<cv::Point>> pair = contourPairs[i];
            std::vector<cv::Point> contour1=pair.first;
            std::vector<cv::Point> contour2=pair.second;

            cv::RotatedRect minRect1=cv::minAreaRect(contour1);
            cv::RotatedRect minRect2=cv::minAreaRect(contour2);
            cv::Point2f rect_points_1[4];
            cv::Point2f rect_points_2[4];
            minRect1.points(rect_points_1);
            minRect2.points(rect_points_2);

            // 将点和对应的y值存入一个vector
            std::vector<std::pair<cv::Point2f, float>> points_with_y1;
            std::vector<std::pair<cv::Point2f, float>> points_with_y2;
            for (int i = 0; i < 4; i++) {
            points_with_y1.push_back(std::make_pair(rect_points_1[i], rect_points_1[i].y));
            points_with_y2.push_back(std::make_pair(rect_points_2[i], rect_points_2[i].y));
            }

            std::sort(rect_points_1, rect_points_1 + 4, 
            [](const cv::Point2f &a, const cv::Point2f &b) 
            {
                return a.y < b.y || (a.y == b.y && a.x < b.x);
            });
            std::sort(rect_points_2, rect_points_2 + 4, 
            [](const cv::Point2f &a, const cv::Point2f &b) 
            {
                return a.y < b.y || (a.y == b.y && a.x < b.x);
            });

            cv::Point2f rect_points[4];
            rect_points[0].x=(rect_points_1[0].x+rect_points_1[1].x)/2;
            rect_points[0].y=(rect_points_1[0].y+rect_points_1[1].y)/2;

            rect_points[1].x=(rect_points_1[2].x+rect_points_1[3].x)/2;
            rect_points[1].y=(rect_points_1[2].y+rect_points_1[3].y)/2;

            rect_points[2].x=(rect_points_2[0].x+rect_points_2[1].x)/2;
            rect_points[2].y=(rect_points_2[0].y+rect_points_2[1].y)/2;

            rect_points[3].x=(rect_points_2[2].x+rect_points_2[3].x)/2;
            rect_points[3].y=(rect_points_2[2].y+rect_points_2[3].y)/2;

            double min=rect_points[0].x+rect_points[0].y;
            double max=rect_points[0].x+rect_points[0].y;
            for(int i=0;i<4;i++)
            {
                if(rect_points[i].x+rect_points[i].y>max)
                {
                    max=rect_points[i].x+rect_points[i].y;
                }
                if(rect_points[i].x+rect_points[i].y<min)
                {
                    min=rect_points[i].x+rect_points[i].y;
                }
            }

            std::vector<cv::Point2f> points(4);
            for(int i=0;i<4;i++)
            {
                if(rect_points[i].x+rect_points[i].y==min)
                {
                    points[0] = rect_points[i];
                    rect_points[i] = cv::Point2f(0, 0); 
                }
                if(rect_points[i].x+rect_points[i].y==max)
                {
                    points[2] = rect_points[i];
                    rect_points[i] = cv::Point2f(0, 0);
                }
            }
            for (int i = 0; i < 4; i++) {
            if (rect_points[i] != cv::Point2f(0, 0)) 
            {
                if (points[1] == cv::Point2f(0, 0)) 
                {
                    points[1] = rect_points[i];
                } 
                else 
                {
                    points[3] = rect_points[i];
                }
            }
        }
            pointsArray.push_back(points);
            cv::line(img,points[0],points[1],cv::Scalar(0, 255, 0), 2); 
            cv::line(img,points[1],points[2],cv::Scalar(0, 255, 0), 2); 
            cv::line(img,points[2],points[3],cv::Scalar(0, 255, 0), 2); 
            cv::line(img,points[3],points[0],cv::Scalar(0, 255, 0), 2);
        }
        return std::make_tuple(img, pointsArray);
    }

