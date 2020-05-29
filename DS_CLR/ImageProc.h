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
*/

#pragma once
#include <opencv2/opencv.hpp>
#include <stdlib.h>
#include "ThresholdingMethodsDefs.h"
#include "UncertaintyAnalysis.h"
#include "EdgesSubPix.h"

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
	cv::Mat BinaryThresh(cv::Mat image, int thresh_type);
	// Image subtraction method
	//	@param:
	//		ref_img		->	reference image opened in grayscale, should be background only
	//		src_img		->	image source opened in grayscale, should contain object(s)
	cv::Mat GrayImageSubtraction(cv::Mat ref_img, cv::Mat src_img);
	// Canny edge detection operation. Provides a binary image of detected edges
	cv::Mat CannyEdgeDetect(cv::Mat grayscale_img, int HighThresh, int LowThresh, int edgeThresh, int kernel_size);
	// function to find the top most and bottom most extreme points on the main drop using subpixel edge detection method
	// 0-> Top Point, 1-> Bottom Point
	std::tuple<cv::Point2f, cv::Point2f> TopAndBotOfMainDrop(cv::Mat grayscale_img);
	// function to get the left most and right most extreme points on main drop using sub pixel edge detection
	// 0-> Left, 1-> Right
	std::tuple<cv::Point2f, cv::Point2f> LeftAndRighOfMainDrop(cv::Mat grayscale_img);
	// function to check for existance of ligament 
	// determined using sub pixel edge detection
	bool isMainDropLong(cv::Mat grayscale_img);

	// MAIN DROP POSITIONS ####################################################################################
	// returns a vector of mass centers for a single image
	// note that return values are in pixels and not actual units
	// @param:
	//	binary_image	->	image that has already undergone thresholding
	std::vector<cv::Point2f> ImageCentroids(cv::Mat binary_image);
	void DrawCentroids(cv::Mat source, std::vector<cv::Point2f> Centers);
	// returns maximum y displacement found in vector of centroids
	cv::Point2f MaxImageCentroid(std::vector<cv::Point2f> centroids);
	cv::Point2f FindMainDropPos(cv::Mat grayscale_img, int thresh_type);
	// Draw Contours
	// Returns processed image, which is a frame of the high speed video
	cv::Mat DrawContours(cv::Mat bin_img, cv::Mat colr_img, bool IncludeSatellites, bool NoiseReduction);
	// Function to shift the centroid if a ligament is detected
	cv::Point2f CorrectCentroid(cv::Mat grayscale_img, int thresh_type, cv::Point2f detected_cent);

	// LIGAMENT LENGTH ########################################################################################
	// Find Bounding Rectangle
	std::vector<cv::Rect2f> FindBoundingRects(cv::Mat grayscale_img, int thresh_type);
	// find bounding rectangle with largest area
	cv::Rect2f FindMaxAreaBoundingRect(std::vector<cv::Rect2f> BoundRects);
	cv::Rect2f FindMainDropRect(cv::Mat grayscale_img, int thresh_type);
	// Draw Bounding Rectangle
	cv::Mat DrawBoundingRects(cv::Mat color_img, std::vector<cv::Rect2f> bound_rects);
	// Find bottom most point of contour
	cv::Point2f FindBottomMostPoint(cv::Mat grayscale_img, int thresh_type);
	// radius of main drop head
	float Distance2Points(cv::Point2f p1, cv::Point2f p2);
	// Determine the two points (start and end) of a ligament
	// LigPoints[0] -> Start, LigPoints[1] -> End
	std::vector<cv::Point2f> LigPoints(cv::Mat grayscale_img, int thresh_type);
	// Return cv points needed to calculate length of ligament as a tuple
	// 0->MainHeadPos, 1->bottommost position, 2->p1, 3->p2
	std::tuple<cv::Point2f, cv::Point2f, cv::Point2f, cv::Point2f> LigLenPoints(cv::Mat grayscale_img, int thresh_type);
	// Calculate length of ligament in picture
	float LengthOfLigament(cv::Mat grayscale_img, int thresh_type);
	
	// DROPLET VOLUME #########################################################################################
	// Find the maximum area of a contour
	double FindMaxContourArea(cv::Mat grayscale_img, int thresh_type);
	int FindMaxCCAreaLabel(cv::Mat Stats, int NumLabels);
	// Find label with highest position centroid (in y direction)
	int FindHighestLabel(cv::Mat Centroids, int NumLabels);
	// Apply a mask that makes the main drop appear only
	cv::Mat MainDropMask(cv::Mat grayscale_img, int thresh_type);
	// Calculate diameter of each level
	int CalculateDiameter(cv::Mat main_drop_row);
	// Calculate the droplet volume
	// addition of UA_info to conduct uncertainty analysis
	// 0->UA_flag, 1->del_dx, 2->del_dy, 3->del_ocv_rel
	float MainDropVolume(cv::Mat grayscale_img, float img_width, float img_height, int thresh_type, std::tuple<bool, float, float, float> UA_info, float* ret_del_v);
	// Updated method to calculate volume of main droplet
	// Use of subpixel edge detection method and volume of revolution using polar coordinates
	// img_width and omg_height are mm ROI measurements
	// May need to implement some form of uncertainty analysis later
	float SubPixelVolume(cv::Mat grayscale_img, cv::Mat color_img, float img_width, float img_height);
	// function to draw results of sub pixel edge detection
	cv::Mat DrawSubPixEdge(cv::Mat gray_img, std::vector<Contour> EdgeContours);
	// function to convert response from px to mm in subpixel edge detection
	float ResponsePX2MM(float R_px, float theta, float dx, float dy);

	// DEBUG IMAGES ##############################################################################################
	// NOTE: functions are only called if parameter is selected
	// Draw Main Drop centroid
	cv::Mat DrawMainDropCent(cv::Mat GrayscaleImg, cv::Mat ColorImg, int thresh_type, cv::Point2f MainDropPoint);
	// Draw All Satellites
	cv::Mat DrawAllSatellites(cv::Mat GrayscaleImg, cv::Mat ColorImg, int thresh_type, float MainDropPos_mm);
	// Draw Ligament Length
	cv::Mat DrawLigamentLength(cv::Mat GrayscaleImg, cv::Mat ColorImg, int thresh_type);
	// Draw Volume of Main Drop
	cv::Mat DrawMainDropVolume(cv::Mat GrayscaleImg, cv::Mat ColorImg, int thresh_type);

	// IMAGE DRAWING ###############################################################################################
	// To draw only selected centroids.Allows selection of drawing params
	cv::Mat DrawSelectedCentroids(cv::Mat ColorImg, std::vector<cv::Point2f> Centers, cv::Scalar clr, int radius);
	// To draw a line between two selected points
	cv::Mat DrawLine2Points(cv::Mat ColorImg, std::vector<cv::Point2f> Points, cv::Scalar clr, int thickness);
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
	bool test_preprocessing(std::string input_dir, std::string output_dir, int thresh_type);

	void test_DrawContours(std::string input_dir, std::string output_dir, int thresh_type);

	void test_image_subtraction(cv::Mat gry_ref, cv::Mat gry_i, cv::Mat clr_i, int thresh_type);
};