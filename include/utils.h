#ifndef DEMO_INCLUDE_UTILS_H_
#define DEMO_INCLUDE_UTILS_H_

#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>

float compute_aspectRatio(const std::vector<cv::Point>& contour);

bool test_pair(std::vector<cv::Point> contour_1,std::vector<cv::Point> contour_2);

std::vector<std::pair<std::vector<cv::Point>, std::vector<cv::Point>>> pair_light(std::vector<std::vector<cv::Point>> contours);

bool test_Parallel(std::vector<cv::Point> contour1,std::vector<cv::Point> contour2, float angleThreshold);

std::tuple<cv::Mat, std::vector<std::vector<cv::Point2f>>> LINE_Rect(std::vector<std::pair<std::vector<cv::Point>, std::vector<cv::Point>>>,cv::Mat img);
#endif