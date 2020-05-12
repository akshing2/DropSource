#pragma once
#include <stdlib.h>
#include "opencv2/opencv.hpp"
#include <math.h>
/*
	@brief:
	These are a set of functions to calculate the absolute unertainties in the measurements of specified parameters to be analysed.
	These functions should be called in the main function file, DropSourceForm.cpp. Note that the Uncertainty Analysis of the 
	TimeVector is done externally due to its simplicity.

	Author:		Akshin Goswami
	Date:		11/05/2020

*/

// Functions to determine uncertainies in image properties (ie image width and height)
namespace UA_Images
{
	// a function that returns a tuple of values that represent the error in width and height of images
	// 0 -> del_img_w
	// 1 -> del_img_h
	std::tuple<float, float> get_del_img_w_h(std::vector<cv::Mat> grayscale_imgs);

	// A function that returns a tuple of values representing the error in vertical and horiztonal conversion factors
	// 0 -> del_dx
	// 1 -> del_dy
	std::tuple<float, float> get_del_dx_dy(cv::Mat gray_img, double ROI_W, double ROI_H, float del_imgw, float del_imgh, float del_ROIw, float del_ROIh);
};

// Functions to determine uncertanty in droplet position
namespace UA_Position
{
	float get_del_rmm(float rpx, float del_rpx_rel, float dy, float del_dy);
};

// Functions to determine uncertainties in droplet velocity
namespace UA_Velocity
{
	float get_del_v(float v0_i, float delta_rmm_i, float delta_t_ms, float del_rmm_i, float del_t_i);
};

// functions to determine uncertainties in ligament length of droplet
namespace UA_LigamentLength
{

};

// functions to determine uncertainties in main droplet volume
namespace UA_Volume
{

};