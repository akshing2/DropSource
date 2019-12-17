#pragma once
#include <opencv2/opencv.hpp>

// simple ocv test
void test_ocv(void);

std::vector<std::string> ListOfFiles(std::string Input_Directory);

// Image Processing Functions
namespace ImageProcessing
{
	// the functions used for every image
	// Preprocessing
	void BinaryThresh(cv::Mat image);
	void DrawContours(cv::Mat bin_img, cv::Mat colr_img);

	// Contour Image List


};