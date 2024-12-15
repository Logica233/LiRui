#ifndef DEMO_INCLUDE_DETECT_LIGHT_H_
#define DEMO_INCLUDE_DETECT_LIGHT_H_

#include <opencv2/opencv.hpp>
#include <vector>

std::vector<std::vector<cv::Point>> detect_light(cv::Mat src);

float compute_area_ratio(const std::vector<cv::Point>& contour);

bool test_aspectRatio(const std::vector<cv::Point>& contour);
#endif