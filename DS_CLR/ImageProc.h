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

#define MIN_SATELLITE_SIZE	10
#define MAX_SATELLITE_SIZE	15
#define MIN_MAIN_DROP_SIZE	20

#define AXIAL_DEV_ALLOWED	0.50	// 10%
#define MAX_AXIAL_DEV		(1 + AXIAL_DEV_ALLOWED)
#define MIN_AXIAL_DEV		(1 - AXIAL_DEV_ALLOWED)

#define HEIGHT_VAL			0.5
#define HEIGHT_WIDTH_CMP	(1 + HEIGHT_VAL)

// Functions to navigate file systems
namespace file_system
{
	std::vector<std::string> ListOfFiles(std::string Input_Directory);
};


// Image Processing Functions
namespace ImageProcessing
{
	// PRE-PROCESSING #########################################################################################
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

	// MAIN DROP POSITIONS ####################################################################################
	// returns a vector of mass centers for a single image
	// note that return values are in pixels and not actual units
	// @param:
	//	binary_image	->	image that has already undergone thresholding
	std::vector<cv::Point2f> ImageCentroids(cv::Mat binary_image);
	void DrawCentroids(cv::Mat source, std::vector<cv::Point2f> Centers);
	// returns maximum y displacement found in vector of centroids
	cv::Point2f MaxImageCentroid(std::vector<cv::Point2f> centroids);
	cv::Point2f FindMainDropPos(cv::Mat grayscale_img);
	// Draw Contours
	// Returns processed image, which is a frame of the high speed video
	cv::Mat DrawContours(cv::Mat bin_img, cv::Mat colr_img, bool IncludeSatellites, bool NoiseReduction);

	// LIGAMENT LENGTH ########################################################################################
	// Find Bounding Rectangle
	std::vector<cv::Rect2f> FindBoundingRects(cv::Mat grayscale_img);
	// find bounding rectangle with largest area
	cv::Rect2f FindMaxAreaBoundingRect(std::vector<cv::Rect2f> BoundRects);
	cv::Rect2f FindMainDropRect(cv::Mat grayscale_img);
	// Draw Bounding Rectangle
	cv::Mat DrawBoundingRects(cv::Mat color_img, std::vector<cv::Rect2f> bound_rects);
	// Find bottom most point of contour
	cv::Point2f FindBottomMostPoint(cv::Mat grayscale_img);
	// radius of main drop head
	float Distance2Points(cv::Point2f p1, cv::Point2f p2);
	// Calculate length of ligament in picture
	float LengthOfLigament(cv::Mat grayscale_img);

	// DROPLET VOLUME #########################################################################################
	// Find the maximum area of a contour
	double FindMaxContourArea(cv::Mat grayscale_img);
	int FindMaxCCAreaLabel(cv::Mat Stats, int NumLabels);
	// Find label with highest position centroid (in y direction)
	int FindHighestLabel(cv::Mat Centroids, int NumLabels);
	// Apply a mask that makes the main drop appear only
	cv::Mat MainDropMask(cv::Mat grayscale_img);
	// Calculate diameter of each level
	int CalculateDiameter(cv::Mat main_drop_row);
	// Calculate the droplet volume
	float MainDropVolume(cv::Mat main_drop_img, float img_width, float img_height);
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