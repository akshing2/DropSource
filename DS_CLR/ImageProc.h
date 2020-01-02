/*
*	@brief
*	The following is a set of functions used for image processing and susbequent droplet analysis.
*	These are external functions to be used in the windows form, DropSourceForm.h. The aim here is
*	to process each individual frame of the high speed video, extract information about the parameters
*	we want, and then save that information into data structures to be published as a CSV
*	or some other appropriate format.
*
*
*	AUTHOR:		Akshin Goswami
*	DATE:		17/12/2019
*	VERSION:	0.1.0
*
*	Future Goals:
*		1. Develop algorithm to determine position and velocity of main droplet.
*		2. Develop algorithm to determine number of satellites.
*		3. Develop algorithm to determine length of ligament.
*		4. Develop algorithm to determine volume of main droplet.
*/

#pragma once
#include <opencv2/opencv.hpp>
#include <stdlib.h>

#define MIN_SATELLITE_SIZE 10
#define MAX_SATELLITE_SIZE 15
#define MIN_MAIN_DROP_SIZE 20

// Functions to navigate file systems
namespace file_system
{
	std::vector<std::string> ListOfFiles(std::string Input_Directory);
};


// Image Processing Functions
namespace ImageProcessing
{
	// Read in a single image from specified filepath
	// @param:
	//		input_dir	->	Input directory from form
	//		IMREAD_TYPE	->	Determines how image should be read in (eg colour or grayscale)
	cv::Mat GetInputImage(std::string input_file, int IMREAD_TYPE);

	// Save a vector of images from input directory
	// @param:
	//		input_dir	->	Input directory from form
	//		IMREAD_TYPE	->	Determines how image should be read in (eg colour or grayscale)
	std::vector<cv::Mat> get_images(std::string input_dir, int IMREAD_TYPE);
	// Preprocessing
	cv::Mat BinaryThresh(cv::Mat image);
	// returns a vector of mass centers for a single image
	// note that return values are in pixels and not actual units
	// @param:
	//	binary_image	->	image that has already undergone thresholding
	std::vector<cv::Point2f> ImageCentroids(cv::Mat binary_image);
	void DrawCentroids(cv::Mat source, std::vector<cv::Point2f> Centers);
	// returns maximum y displacement found in vector of centroids
	cv::Point2f MaxImageCentroid(std::vector<cv::Point2f> centroids);
	// Draw Contours
	// Returns processed image, which is a frame of the high speed video
	cv::Mat DrawContours(cv::Mat bin_img, cv::Mat colr_img, bool IncludeSatellites, bool NoiseReduction);

	// Contour Image List


};

// Numerical Methods
namespace NumericalMethods
{
	cv::Point2f ForwardDifference(cv::Point2f rt0, cv::Point2f rt1);
};

// Set of functions for testing image processing functions
namespace ImProcTest
{
	// simple ocv test
	void test_ocv(void);

	// Test preprocessing
	// Read in all images in input folder
	// And output preprocessed images in output directory
	bool test_preprocessing(std::string input_dir, std::string output_dir);

	void test_DrawContours(std::string input_dir, std::string output_dir);
};