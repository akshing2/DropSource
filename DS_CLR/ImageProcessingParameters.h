#pragma once
#include "opencv2/opencv.hpp"
/*
	This header file contains the default values used to perform image processing
	functions in the app.
*/

// Canny edge detection params
#define DEF_HI_THRESH		255
#define DEF_LO_THRESH		127
#define DEF_EDGE_THRESH		1
#define DEF_KERNAL_SIZE		3

// findContours default params
#define DEF_MODE_FC			cv::RETR_TREE
#define DEF_METHOD_FC		cv::CHAIN_APPROX_NONE

// Sub Pixel Edge Detection params
#define DEF_ALPHA_SP		0.5
#define DEF_LO_SP			0
#define DEF_HI_SP			127
#define DEF_MODE_SP			cv::RETR_TREE
