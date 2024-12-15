#include <opencv2/opencv.hpp>
#include"detect_light.h"
#include"utils.h"

int main(){
   
    cv::VideoCapture cap("../images/demo.avi");
    cv::Mat frame;
   
    
cv::Mat cameraMatrix = (cv::Mat_<double>(3, 3) << 
    894.9123667432059, 0, 361.4960421332104, 
    0, 895.1927371682655, 275.636326189742, 
    0, 0, 1);
cv::Mat distCoeffs = (cv::Mat_<double>(1, 5) << 
    -0.211610906204210, 0.184925401993397, -0.0008448907659021592, 
    0.0006430996918483068, 0.003766512014423);

    while (true) {
        cap >> frame;
        if (frame.empty()) 
        {
            break;
        }
    std::vector<std::vector<cv::Point>> contours=detect_light(frame);
    std::vector<std::pair<std::vector<cv::Point>, std::vector<cv::Point>>> contourpairs=pair_light(contours);
    
    std::vector<std::vector<cv::Point2f>> pointsArray;
    std::tie(frame,pointsArray)=LINE_Rect(contourpairs,frame);

    std::vector<cv::Point2f> imagePoints;
    std::vector<cv::Point3f> objectPoints;

    for (const auto& points : pointsArray) {
        for (const auto& pt : points) {
            imagePoints.push_back(pt); 
            objectPoints.push_back(cv::Point3f(pt.x, pt.y, 0.0f)); 
    }
    }
     // 进行 PnP 解算
    cv::Mat rvec, tvec;
    bool test = cv::solvePnP(objectPoints, imagePoints, cameraMatrix, distCoeffs, rvec, tvec);

    if (test) {
        std::cout << "旋转向量: " << rvec << std::endl;
        std::cout << "平移向量: " << tvec << std::endl;
        std::cout<<std::endl;
        std::cout<<std::endl;
    } 

    if (cv::waitKey(30)==27) 
        {
            break;
        }
    imshow("Video", frame);
    }
    cap.release();
    cv::destroyAllWindows();
    return 0;
}