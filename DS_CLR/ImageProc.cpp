#include "ImageProc.h"
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <iostream>
//#include "DropSourceFrom.h"
#include  <filesystem>
#define PI	3.14159265358979323846

namespace fs = std::filesystem;
using namespace cv;
using recurv_dir_it = std::filesystem::recursive_directory_iterator;

std::vector<std::string> file_system::ListOfFiles(std::string path)
{
	std::vector<std::string> fpList;

	for (auto& dirEntry : std::filesystem::recursive_directory_iterator(path))
	{
		std::filesystem::path file = dirEntry.path();
		fpList.push_back(file.string());
	}
		

	return fpList;
}

cv::Mat ImageProcessing::GetInputImage(std::string input_file, int IMREAD_TYPE)
{
	cv::Mat img = cv::imread(input_file, IMREAD_TYPE);

	return img;
}

std::vector<cv::Mat> ImageProcessing::get_images(std::string input_dir, int IMREAD_TYPE)
{
	std::vector<cv::Mat> images;
	cv::Mat img;
	// Get the filepaths of all the images as a string vector
	std::vector<std::string> fp_input = file_system::ListOfFiles(input_dir);

	// iterate through fp_input and save into grayscale images
	for (std::vector<std::string>::iterator it = fp_input.begin(); it != fp_input.end(); ++it)
	{
		img = GetInputImage(*it, IMREAD_TYPE);
		images.push_back(img);
	}

	return images;
}

cv::Mat ImageProcessing::BinaryThresh(cv::Mat image, int thresh_type)
{
	// initialise pre-processed image to return
	cv::Mat pre_proc;

	// Apply a Gausian linear filter on the image,
	// This is to reduce the noise of the image.
	// Parameters may need to be changed.
	cv::GaussianBlur(image, pre_proc, cv::Size(5, 5), 0);

	// Conduct an adaptive threshold on the droplet.
	// Parameters may need to be changed.
	// https://docs.opencv.org/2.4/modules/imgproc/doc/miscellaneous_transformations.html?highlight=adaptivethreshold
	if (thresh_type == THRESH_ADAPTIVE)
	{
		cv::adaptiveThreshold(pre_proc, pre_proc, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 9, 2);
	}
	else if (thresh_type == THRESH_GLOBAL)
	{
		cv::threshold(pre_proc, pre_proc, 0, 255, cv::THRESH_BINARY);
	}
	

	return pre_proc;
}

cv::Mat ImageProcessing::GrayImageSubtraction(cv::Mat ref_img, cv::Mat src_img)
{
	cv::Mat ret_img;
	
	// temporary arrays
	cv::Mat tmp1;
	cv::Mat tmp2;

	// conversions
	cv::convertScaleAbs(ref_img, tmp1, 0.5, 128);
	cv::convertScaleAbs(src_img, tmp2, 0.5, 0);

	// now subtract
	//cv::subtract(tmp1, tmp2, ret_img);
	cv::subtract(ref_img, src_img, ret_img);

	return ret_img;
}

cv::Mat ImageProcessing::CannyEdgeDetect(cv::Mat grayscale_img, int HighThresh, int LowThresh, int edgeThresh, int kernel_size)
{	
	cv::Mat dest, detected_edges;

	// reduce noise with specified kernel size
	cv::GaussianBlur(grayscale_img, detected_edges, cv::Size(kernel_size, kernel_size), 0);

	// canny detector
	cv::Canny(detected_edges, detected_edges, LowThresh, HighThresh, kernel_size);

	// use canny's output as mask to show edges
	dest = Scalar::all(0);

	grayscale_img.copyTo(dest, detected_edges);

	return dest;
}

std::tuple<cv::Point2f, cv::Point2f> ImageProcessing::TopAndBotOfMainDrop(cv::Mat grayscale_img)
{
	std::tuple<cv::Point2f, cv::Point2f> ExtPoints;
	cv::Point2f Bottom;
	cv::Point2f Top;

	// parameters for sub pixel edge detection
	double alpha = 0.5;
	int low = 0;
	int high = 127;
	std::vector<Contour> contours;		// vector of Contour structs from EdgesSubPix.h
	std::vector<cv::Vec4i> heirarchy;
	int mode = cv::RETR_TREE;			// same mode used in finding main drop points

	// perform sub pixel edge detection
	EdgesSubPix(grayscale_img, alpha, low, high, contours, heirarchy, mode);

	// let's see if the points we get are what we want
	cv::Mat subpixedge = DrawSubPixEdge(grayscale_img, contours);
	cv::imshow("Gray", grayscale_img);
	cv::imshow("subpixedge", subpixedge);
	cv::waitKey(0);
	cv::destroyAllWindows();

	// now loop through the first contour (main drop) to find extreme points
	// for top most point, it is the point with min Y value
	// for bottom most point, it is the point with max Y value
	float Ytop_cmp = grayscale_img.size().height;	// start with max value to find min
	float Ybot_cmp = 0;								// start with min value to find max
	float MaxY = Ybot_cmp;
	float MinY = Ytop_cmp;
	if (contours.size() > 0)
	{
		for (int i = 0; i < contours.at(0).points.size(); i++)
		{
			Ytop_cmp = contours.at(0).points.at(i).y;
			Ybot_cmp = Ytop_cmp;						// we're comparing the same Y values
			// check for top most point
			if (Ytop_cmp < MinY)
			{
				MinY = Ytop_cmp;
				Top = contours.at(0).points.at(i);
			}
			// check for bottom most point
			if (Ybot_cmp > MaxY)
			{
				MaxY = Ybot_cmp;
				Bottom = contours.at(0).points.at(i);
			}
		}
	}
	else {
		Top.x = -1;
		Top.y = -1;
		Bottom = Top;
	}
	
	// add points to return tuple
	ExtPoints = std::make_tuple(Top, Bottom);
	return ExtPoints;
}

std::tuple<cv::Point2f, cv::Point2f> ImageProcessing::LeftAndRighOfMainDrop(cv::Mat grayscale_img)
{
	std::tuple<cv::Point2f, cv::Point2f> ExtPoints;
	cv::Point2f Right;
	cv::Point2f Left;

	// parameters for sub pixel edge detection
	double alpha = 0.5;
	int low = 0;
	int high = 127;
	std::vector<Contour> contours;		// vector of Contour structs from EdgesSubPix.h
	std::vector<cv::Vec4i> heirarchy;
	int mode = cv::RETR_TREE;			// same mode used in finding main drop points

	// perform sub pixel edge detection
	EdgesSubPix(grayscale_img, alpha, low, high, contours, heirarchy, mode);

	// now loop through the first contour (main drop) to find extreme points
	// for Left most point, it is the point with min X value
	// for Right most point, it is the point with max X value
	float Xleft_cmp = grayscale_img.size().height;		// start with max value to find min
	float Xright_cmp = 0;								// start with min value to find max
	float MaxX = Xright_cmp;
	float MinX = Xleft_cmp;
	if (contours.size() > 0)
	{
		for (int i = 0; i < contours.at(0).points.size(); i++)
		{
			Xright_cmp = contours.at(0).points.at(i).x;
			Xleft_cmp = Xright_cmp;						// we're comparing the same X values
			// check for left most point
			if (Xleft_cmp < MinX)
			{
				MinX = Xleft_cmp;
				Left = contours.at(0).points.at(i);
			}
			// check for right most point
			if (Xright_cmp > MaxX)
			{
				MaxX = Xright_cmp;
				Right = contours.at(0).points.at(i);
			}
		}
	}
	else {
		Left.x = -1;
		Left.y = -1;
		Right = Left;
	}

	// add points to return tuple
	ExtPoints = std::make_tuple(Left, Right);
	return ExtPoints;
}

bool ImageProcessing::isMainDropLong(cv::Mat grayscale_img)
{
	bool isLong = false;	// assume false until proven true
	
	// get the extreme points of the main drop
	std::tuple<cv::Point2f, cv::Point2f> TopAndBot = TopAndBotOfMainDrop(grayscale_img);
	cv::Point2f Top = std::get<0>(TopAndBot);
	cv::Point2f Bot = std::get<1>(TopAndBot);
	std::tuple<cv::Point2f, cv::Point2f> LftAndRht = LeftAndRighOfMainDrop(grayscale_img);
	cv::Point2f Left = std::get<0>(LftAndRht);
	cv::Point2f Right = std::get<1>(LftAndRht);

	// get vertical distance
	float vert_dist = Distance2Points(Top, Bot);
	// get horizontal distance
	float horz_dist = Distance2Points(Left, Right);

	// if Horizontal distance is less than some max threshold, 
	float dist_cmp = 0.5;
	if ((horz_dist <= (1.0 + dist_cmp) * vert_dist))
	{
		// roughly circular
		isLong = true;
	}

	return isLong;
}

std::vector<cv::Point2f> ImageProcessing::ImageCentroids(cv::Mat binary_image)
{
	//std::vector<cv::Point2f> centers;
	std::vector<std::vector<cv::Point>> Contours;
	std::vector<cv::Vec4i> Heirarchy;
	// Get Mass centers
	std::vector<cv::Point2f> centers;
	// width of image
	float half_width = (binary_image.size().width/2);
	cv::Point2f center;

	// find contours
	// Parameters found in python script
	cv::findContours(binary_image, Contours, Heirarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

	// Get moments
	std::vector<cv::Moments> mu(Contours.size());
	for (int i = 0; i < Contours.size(); i++)
	{
		mu[i] = cv::moments(Contours[i], true);
	}

	
	for (int i = 0; i < Contours.size(); i++)
	{
		center = cv::Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
		if (center.x <= (float)(MAX_AXIAL_DEV * half_width) && center.x >= (float)(MIN_AXIAL_DEV * half_width))
		{
			// In range so include in returned centers
			centers.push_back(center);
		}

	}

	return centers;
}

void ImageProcessing::DrawCentroids(cv::Mat source, std::vector<cv::Point2f> Centers)
{
	cv::Scalar red(0, 0, 255);
	cv::Scalar blue(255, 0, 0);
	cv::Scalar green(0, 255, 0);
	int radius = 3;

	for (int i = 0; i < Centers.size(); i++)
	{
		cv::circle(source, Centers[i], radius, green, -1);
		//cv::putText(source, std::to_string(i), Centers[i], cv::FONT_HERSHEY_SCRIPT_COMPLEX, 2, green);
	}
}

cv::Mat ImageProcessing::DrawSelectedCentroids(cv::Mat ColorImg, std::vector<cv::Point2f> Centers, cv::Scalar clr, int radius)
{
	cv::Mat drawing = ColorImg.clone();

	for (int i = 0; i < Centers.size(); i++)
	{
		cv::circle(drawing, Centers[i], radius, clr, -1);
	}

	return drawing;
}

cv::Mat ImageProcessing::DrawLine2Points(cv::Mat ColorImg, std::vector<cv::Point2f> Points, cv::Scalar clr, int thickness)
{
	cv::Mat drawing = ColorImg.clone();

	cv::line(drawing, Points[0], Points[1], clr, thickness);
	
	return drawing;
}


cv::Point2f ImageProcessing::MaxImageCentroid(std::vector<cv::Point2f> centroids)
{
	cv::Point2f MaxCentroid(0, 0);

	for (int i = 0; i < centroids.size(); i++)
	{
		if (centroids[i].y > MaxCentroid.y)
		{
			MaxCentroid.y = centroids[i].y;
			MaxCentroid.x = centroids[i].x;
		}
	}

	return MaxCentroid;
}

cv::Point2f ImageProcessing::FindMainDropPos(cv::Mat grayscale_img, int thresh_type)
{
	cv:Point2f pos;

	cv::Mat bin_img = BinaryThresh(grayscale_img, thresh_type);

	std::vector<cv::Point2f> centers = ImageCentroids(bin_img);

	pos = MaxImageCentroid(centers);

	return pos;
}

cv::Mat ImageProcessing::DrawContours(cv::Mat bin_img, cv::Mat colr_img, bool IncludeSatellites, bool NoiseReduction)
{
	// initialise return processed frame
	cv::Mat cont_img = colr_img.clone();
	// initialise contours and heriarchies
	// May need to revisit
	std::vector<std::vector<cv::Point>> Contours;
	std::vector<cv::Vec4i> Heirarchy;
	// Set colour of drawing
	cv::Scalar colour = Scalar(0, 0, 255);
	cv::Scalar colour2 = Scalar(0, 255, 0);
	// radius of center of circle
	int radius = 2;

	// find contours
	// Parameters found in python script
	cv::findContours(bin_img, Contours, Heirarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

	// Get moments
	std::vector<cv::Moments> mu(Contours.size());
	for (int i = 0; i < Contours.size(); i++)
	{
		mu[i] = cv::moments(Contours[i], true);
	}

	// Get Mass centers
	std::vector<cv::Point2f> mc(Contours.size());
	for (int i = 0; i < Contours.size(); i++)
	{
		mc[i] = cv::Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
	}

	// Draw contours with centroids in them
	for (int i = 0; i < Contours.size(); i++)
	{
		// Remove noise
		if (NoiseReduction)
		{
			if (cv::contourArea(Contours[i]) < MIN_SATELLITE_SIZE)
			{
				// remove contour if smaller than satellite
				Contours.erase(Contours.begin() + i);
				continue;
			}
		}
		

		// check if you want to include satellites or not
		if (!IncludeSatellites)
		{
			// Less than max satellite size, consider as satellite
			if (cv::contourArea(Contours[i]) < MAX_SATELLITE_SIZE) {
				Contours.erase(Contours.begin() + i);
				continue;
			}
		}
		// Draw contours
		cv::drawContours(cont_img, Contours, i, colour, -1);
		// Put center of mass on
		cv::circle(cont_img, mc[i], radius, colour2, -1);
		// Label centers
		/*std::string cent_str = std::to_string(mc[i].y);
		cv::putText(cont_img, cent_str, mc[i], cv::FONT_HERSHEY_COMPLEX, 1, colour2);*/
	}

	return cont_img;
}

cv::Point2f ImageProcessing::CorrectCentroid(cv::Mat grayscale_img, int thresh_type, cv::Point2f detected_cent)
{
	cv::Point2f Corrected = detected_cent;
	// TODO: will give wrong correction when only ligament present on screen
	// (worth fixing? Usually ligament breaks up before drop leave ROI)
	// see if a ligament is present. Usually height is much bigger than the width
	if (isMainDropLong(grayscale_img))
	{
		// ligament exists, so correct it
		// first find the bottom most point
		std::tuple<cv::Point2f, cv::Point2f> TopAndBot = TopAndBotOfMainDrop(grayscale_img);
		cv::Point2f bot = std::get<1>(TopAndBot);		// done through sub pixel edge detection

		// next get the distance between the supposed center and bottom most point
		float dist = Distance2Points(detected_cent, bot);

		// now shift the corrected center to 0.5*dist downward
		Corrected.y = Corrected.y + 0.5 * dist;
	}

	return Corrected;
}

std::vector<cv::Rect2f> ImageProcessing::FindBoundingRects(cv::Mat grayscale_img, int thresh_type)
{
	std::vector<cv::Rect2f> BoundRects;
	std::vector<std::vector<cv::Point>> Contours;
	std::vector<cv::Vec4i> Heirarchy;
	// Set colour of drawing
	cv::Scalar colour = Scalar(0, 0, 255);
	cv::Scalar colour2 = Scalar(0, 255, 0);
	
	// binary threshold
	cv::Mat bin_img = BinaryThresh(grayscale_img, thresh_type);

	// find contours
	// Parameters found in python script
	cv::findContours(bin_img, Contours, Heirarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

	// polys for bounding rect 
	std::vector<std::vector<cv::Point2f>> contours_poly(Contours.size());

	// get all bounding rectangles
	for (int i = 0; i < Contours.size(); i++)
	{
		cv::approxPolyDP(Contours[i], contours_poly[i], 3, true);
		BoundRects.push_back(cv::boundingRect(contours_poly[i]));
	}

	return BoundRects;
}

cv::Rect2f ImageProcessing::FindMaxAreaBoundingRect(std::vector<cv::Rect2f> BoundRects)
{
	cv::Rect2f MaxRect;
	float max_area = 0;
	for (int i = 0; i < BoundRects.size(); i++)
	{
		if (BoundRects[i].area() > max_area)
		{
			max_area = BoundRects[i].area();
			MaxRect = BoundRects[i];
		}
	}

	return MaxRect;
}

cv::Rect2f ImageProcessing::FindMainDropRect(cv::Mat grayscale_img, int thresh_type)
{
	cv::Rect2f MainDropRect;
	std::vector<cv::Rect2f> Rects;

	Rects = FindBoundingRects(grayscale_img, thresh_type);
	MainDropRect = FindMaxAreaBoundingRect(Rects);

	return MainDropRect;
}

cv::Mat ImageProcessing::DrawBoundingRects(cv::Mat color_img, std::vector<cv::Rect2f> bound_rects)
{
	cv::Scalar red = cv::Scalar(0, 0, 255);
	cv::Mat drawing = color_img.clone();
	std::string length_txt = "Length: ";
	std::string width_txt = "Width: ";
	cv::Point2f center(10, 500);
	cv::Point2f center2(10, 700);
	cv::Rect2f MainRect;
	
	// find bounding box with biggest area and assume this is the main droplet
	MainRect = FindMaxAreaBoundingRect(bound_rects);
	cv::rectangle(drawing,MainRect.tl(), MainRect.br(), red, 2);
	cv::putText(drawing, length_txt + std::to_string(MainRect.height), center, cv::FONT_HERSHEY_COMPLEX_SMALL, 2, red);
	cv::putText(drawing, width_txt + std::to_string(MainRect.width), center, cv::FONT_HERSHEY_COMPLEX_SMALL, 2, red);

	return drawing;
}

cv::Point2f ImageProcessing::FindBottomMostPoint(cv::Mat grayscale_img, int thresh_type)
{
	cv::Point2f bottom;
	std::vector<std::vector<cv::Point>> Contours;
	std::vector<cv::Point> cnts;
	std::vector<cv::Vec4i> Heirarchy;
	cv::Point extBot(-1,-1);

	// binarise image
	cv::Mat bin_img = BinaryThresh(grayscale_img, thresh_type);

	// find contours
	cv::findContours(bin_img, Contours, Heirarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);
	if (Contours.size() > 0)
	{
		// to get largest of contours
		cnts = *std::max_element(Contours.begin(), Contours.end(), [](const std::vector<cv::Point>& lhs, const std::vector<cv::Point>& rhs)
			{
				return lhs.size() < rhs.size();
			});
		// find extreme bottom
		extBot = *std::max_element(cnts.begin(), cnts.end(), [](const cv::Point& lhs, const cv::Point& rhs)
			{
				return lhs.y < rhs.y;
			});
	}

	bottom.x = float(extBot.x);
	bottom.y = float(extBot.y);

	return bottom;
}

float ImageProcessing::Distance2Points(cv::Point2f p1, cv::Point2f p2)
{
	float dist;
	
	dist = sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));

	return dist;
}

std::vector<cv::Point2f> ImageProcessing::LigPoints(cv::Mat grayscale_img, int thresh_type)
{
	std::vector<cv::Point2f>LigamentPoints;

	// 1. find main drop position
	cv::Point2f pos = FindMainDropPos(grayscale_img, thresh_type);

	// 2. find extreme bottom
	cv::Point2f ExtBot = FindBottomMostPoint(grayscale_img, thresh_type);

	// 3. find radius of head
	float HeadRadius = Distance2Points(pos, ExtBot);

	// 4. find bounding rectangle
	cv::Rect2f Rect = FindMainDropRect(grayscale_img, thresh_type);

	// 5. find edge of head and ligament, p1
	cv::Point2f p1 = pos;
	p1.y = p1.y - HeadRadius;

	// 6. find tail end of ligament, p2
	cv::Point2f p2 = pos;
	p2.y = p2.y + HeadRadius - Rect.height;

	if (Rect.height > HEIGHT_WIDTH_CMP* Rect.width)
	{
		// if height much greater than width, save points
		LigamentPoints.push_back(p1);
		LigamentPoints.push_back(p2);
	}

	return LigamentPoints;
}

std::tuple<cv::Point2f, cv::Point2f, cv::Point2f, cv::Point2f> ImageProcessing::LigLenPoints(cv::Mat grayscale_img, int thresh_type)
{
	std::tuple<cv::Point2f, cv::Point2f, cv::Point2f, cv::Point2f> LigLenPts;

	// using sub pixel edge detection to get top and bottom of main drop
	std::tuple<cv::Point2f, cv::Point2f> TopAndBot = TopAndBotOfMainDrop(grayscale_img);

	// 1. find main drop position
	cv::Point2f MainDropPos = FindMainDropPos(grayscale_img, thresh_type);
	// 1 a) correct main drop position
	MainDropPos = CorrectCentroid(grayscale_img, thresh_type, MainDropPos);

	// 2. find extreme bottom
	//cv::Point2f bxy = FindBottomMostPoint(grayscale_img, thresh_type);
	// trying with new method
	cv::Point2f bxy = std::get<1>(TopAndBot);

	// 3. find radius of head
	float HeadRadius = Distance2Points(MainDropPos, bxy);

	// 4. find bounding rectangle
	cv::Rect2f Rect = FindMainDropRect(grayscale_img, thresh_type);

	// 5. find edge of head and ligament, p1
	cv::Point2f p1 = MainDropPos;
	p1.y = p1.y - HeadRadius;

	// 6. find tail end of ligament, p2
	/*cv::Point2f p2 = MainDropPos;
	p2.y = p2.y + HeadRadius - Rect.height;*/
	// let's find the tail end of the ligament using the new method
	cv::Point2f p2 = std::get<0>(TopAndBot);

	LigLenPts = std::make_tuple(MainDropPos, bxy, p1, p2);

	return LigLenPts;
}

float ImageProcessing::LengthOfLigament(cv::Mat grayscale_img, int thresh_type)
{
	float LigLength = -1;
	std::tuple<cv::Point2f, cv::Point2f, cv::Point2f, cv::Point2f> LigLenPts = LigLenPoints(grayscale_img, thresh_type);

	// 1. find main drop position
	cv::Point2f pos = std::get<0>(LigLenPts);

	// 2. find extreme bottom
	cv::Point2f ExtBot = std::get<1>(LigLenPts);

	// 3. find radius of head
	float HeadRadius = Distance2Points(pos, ExtBot);

	// 4. find bounding rectangle
	cv::Rect2f Rect = FindMainDropRect(grayscale_img, thresh_type);

	// 5. find edge of head and ligament, p1
	cv::Point2f p1 = std::get<2>(LigLenPts);

	// 6. find tail end of ligament, p2
	cv::Point2f p2 = std::get<3>(LigLenPts);

	if (Rect.height > HEIGHT_WIDTH_CMP* Rect.width)
	{
		// if height much greater than width, calculate ligament
		LigLength = Distance2Points(p2, p1);
	}

	return LigLength;
}

double ImageProcessing::FindMaxContourArea(cv::Mat grayscale_img, int thresh_type)
{
	std::vector<std::vector<cv::Point>> Contours;
	std::vector<cv::Vec4i> Heirarchy;
	double MaxContourArea = 0;

	// image processing
	cv::Mat bin_img = BinaryThresh(grayscale_img, thresh_type);

	// find contours
	// Parameters found in python script
	cv::findContours(bin_img, Contours, Heirarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

	// itterate through contours and find the max area contour
	for (int i = 0; i < Contours.size(); i++)
	{
		if (cv::contourArea(Contours[i]) > MaxContourArea)
		{
			MaxContourArea = cv::contourArea(Contours[i]);
		}
	}

	return MaxContourArea;
}

int ImageProcessing::FindMaxCCAreaLabel(cv::Mat Stats, int NumLabels)
{
	/*double MaxCCArea;
	double MinCCArea;
	cv::Point minloc;
	cv::Point maxloc;

	cv::Mat inspect = Stats.col(4);
	
	cv::minMaxLoc(inspect, &MinCCArea, &MaxCCArea, &minloc, &maxloc);*/

	int MaxCCArea = 0;
	int MaxCCAreaLabel = -1;
	cv::Mat inspect = Stats.col(4);

	for (int i = 1; i < NumLabels; i++)
	{
		if (inspect.at<int>(i,0) > MaxCCArea)
		{
			MaxCCArea = inspect.at<int>(i, 0);
			MaxCCAreaLabel = i;
		}
	}

	return MaxCCAreaLabel;
}

int ImageProcessing::FindHighestLabel(cv::Mat Centroids, int NumLabels)
{
	int HighestLabel = -1;
	double HighestPos = 0;

	for (int i = 1; i < NumLabels; i++)
	{
		if (Centroids.at<double>(i, 1) > HighestPos)
		{
			HighestPos = Centroids.at<double>(i, 1);
			HighestLabel = i;
		}
	}

	return HighestLabel;
}


cv::Mat ImageProcessing::MainDropMask(cv::Mat grayscale_img, int thresh_type)
{
	// for connected components
	cv::Mat stats, centroids, labelImage;

	// binarize image
	cv::Mat bin_img = BinaryThresh(grayscale_img, thresh_type);

	// connected components
	int nLabels = cv::connectedComponentsWithStats(bin_img, labelImage, stats, centroids, 4);

	// make mask
	cv::Mat mask(labelImage.size(), bin_img.type(), cv::Scalar(255));
	//cv::Mat surfSup = stats.col(4) > FindMaxCCArea(stats, nLabels);

	for (int i = 1; i < nLabels; i++)
	{
		/*if (surfSup.at<uchar>(i, 0))
		{
			mask = mask & (labelImage == i);
		}*/

		if (i == FindMaxCCAreaLabel(stats,nLabels))
		{
			mask = mask & (labelImage == i);
		}
	}

	cv::Mat r(bin_img.size(), bin_img.type(), cv::Scalar(0));
	bin_img.copyTo(r, mask);

	//cv::Mat r = labelImage;

	return r;
}

int ImageProcessing::CalculateDiameter(cv::Mat main_drop_row)
{
	int Dia = 0;
	int LeftMost = 0;
	int RightMost = 0;

	// Ideally, noise has been taken care of so just count non zeros in row
	
	//Dia = cv::countNonZero(main_drop_row);

	//for (int col = 0; col < main_drop_row.cols; col++)
	//{
	//	if (main_drop_row.at<uchar>(0, col) > 0)
	//	{
	//		// there is a 1 in this binary image
	//		if (LeftMost == 0)
	//		{
	//			// new leftmost pixel
	//			LeftMost = col;
	//		}

	//		if (col > RightMost)
	//		{
	//			// keep updating rightmost
	//			RightMost = col;
	//		}

	//	}
	//}


	// first find left most pixel in row
	for (int col = 0; col < main_drop_row.cols; col++)
	{
		if (main_drop_row.at<uchar>(0, col) > 0)
		{
			LeftMost = col;
			break;
		}
	}

	// now find right most pixel
	for (int col = main_drop_row.cols-1; col > 0; col--)
	{
		if (main_drop_row.at<uchar>(0, col) > 0)
		{
			RightMost = col;
			break;
		}
	}

	Dia = abs(RightMost - LeftMost);

	return Dia;
}

float ImageProcessing::MainDropVolume(cv::Mat grayscale_img, float img_width, float img_height, int thresh_type, std::tuple<bool, float, float, float> UA_info, float* ret_del_v)
{
	float TotalVol = 0;	// total volume of droplet returned
	float Vi = 0;			// volume of i'th thin disk
	float Area = 0;			// Area of circular face of i'th thin disk
	float X = 0;
	float di = 0;			// diameter of i'th disk

	// first get dx and dy
	float dx = img_width / grayscale_img.size().width;		// mm/px
	float dy = img_height / grayscale_img.size().height;	// mm/px

	// for uncertainty analysis
	bool UA_flag = std::get<0>(UA_info);
	float del_dx = std::get<1>(UA_info);
	float del_H = std::get<2>(UA_info);
	float del_ocv_rel = std::get<3>(UA_info);
	float del_X;
	float del_di;
	float del_Vi = 0;

	// sum of upper and lower bounds of volume
	float V_hi = 0;
	float Vi_hi = 0;
	float V_lo = 0;
	float Vi_lo = 0;
	float del_V = -1;

	// canny edge detector params
	int LowThresh = 127;
	int HighThresh = 255;
	int edgeThresh = 1;
	int kernal_size = 3;

	bool init_reset_flag = true;

	//cv::Mat main_drop_img = MainDropMask(grayscale_img, thresh_type);
	cv::Mat main_drop_img = CannyEdgeDetect(grayscale_img, HighThresh, LowThresh, edgeThresh, kernal_size);

	// let's check if we get the canny image we want
	/*cv::imshow("Gray Image", grayscale_img);
	cv::imshow("Canny Image", main_drop_img);
	cv::waitKey(0);
	cv::destroyAllWindows();*/

	// itterate row by row
	for (int row = 0; row < main_drop_img.rows; row++)
	{
		// Get area of each row
		X = CalculateDiameter(main_drop_img.row(row)); // done thru open cv
		di = X * dx;
		Area = PI * pow((di/2), 2.0);
		Vi = Area * dy;

		TotalVol += Vi;

		// do uncertainty analysis if cbox checked
		if (UA_flag)
		{
			del_X = del_ocv_rel * X;
			// uncertainty in diamater of i'th disk
			del_di = UA_Volume::get_del_di(X, dx, del_X, del_dx);
			// uncertainty in volume of i'th disk
			del_Vi = UA_Volume::get_del_Vi(di, dy, del_di, del_H);
			// sum up to get upper bound of volume
			Vi_hi = Vi + del_Vi;
			V_hi += Vi_hi;
			// sum up to get lower bound of volume
			Vi_lo = (Vi - del_Vi);
			V_lo += Vi_lo;
		}
	}

	if (UA_flag)
	{
		// comparison values
		float Cv_hi = abs(TotalVol - V_hi);
		float Cv_lo = abs(TotalVol - V_lo);
		// finally, the error in volume
		del_V = (Cv_hi + Cv_lo)/2;
	}


	*ret_del_v = del_V;
	return TotalVol;
}

// assume that this function is only called when the main droplet is present
float ImageProcessing::SubPixelVolume(cv::Mat grayscale_img, cv::Mat color_img, float img_width, float img_height)
{
	// variables for volume calculation
	float R_px;												// R(theta) in subpixel edge detection
	float R_mm;												// R(theta) in subpixel edge detection converted to mm
	float integral_sum = 0;									// where sum of the integral is stored
	float dx = img_width / grayscale_img.size().width;		// mm/px
	float dy = img_height / grayscale_img.size().height;	// mm/px
	float theta_i;											// direction of i'th response
	float MainDropVol = 0;									// returned value (mm^3)

	// parameters for sub pixel edge detection
	double alpha = 0.5;
	int low = 0;
	int high = 127;
	std::vector<Contour> contours;		// vector of Contour structs from EdgesSubPix.h
	std::vector<cv::Vec4i> heirarchy;
	int mode = cv::RETR_TREE;			// same mode used in finding main drop points

	// perform sub pixel edge detection
	EdgesSubPix(grayscale_img, alpha, low, high, contours, heirarchy, mode);

	// need to see if it works like I want
	// let's see if the first contour is always the main drop (it is)
	/*cv::Mat draw = DrawSubPixEdge(color_img, contours);
	cv::imshow("Grayscale image", grayscale_img);
	cv::imshow("Sub pixel edge detection", draw);
	cv::waitKey(0);
	cv::destroyAllWindows();*/

	// Need to perform the integration
	Contour MainDropCnt = contours.at(0);		// looking at the main drop only
	
	for (int i = 0; i < MainDropCnt.response.size(); i++)
	{
		// loop through every response and measure integral sum
		// first, convert units of response, R, from px to mm
		R_px = MainDropCnt.response.at(i);
		theta_i = MainDropCnt.direction.at(i);
		R_mm = ResponsePX2MM(R_px, theta_i, dx, dy);	// response now in mm instead of px
		// only add integral sum if theta is between 0 and pi
		if (theta_i <= PI) integral_sum += pow(R_mm, 3.0) * sin(theta_i);	// might need to check if this is valid
		
	}

	// now we try main drop volume
	MainDropVol = 2.0 / 3.0 * PI * integral_sum;

	return MainDropVol;
}

cv::Mat ImageProcessing::DrawSubPixEdge(cv::Mat gray_img, std::vector<Contour> EdgeContours)
{
	cv::Mat drawing;
	cv::cvtColor(gray_img, drawing, cv::COLOR_GRAY2RGB);

	int radius = 1;
	cv::Scalar green(0, 255, 0);

	// draw the points in cartesian coordinates
	for (int i = 0; i < 1; i++)
	{
		// looping through each contour (represents a body)

		for (int j = 0; j < EdgeContours.at(i).points.size(); j++)
		{
			// loop through and draw every point
			cv::circle(drawing, EdgeContours.at(i).points.at(j), radius, green);
		}

	}

	return drawing;
}

float ImageProcessing::ResponsePX2MM(float R_px, float theta, float dx, float dy)
{
	float R_mm = 0;

	float Rx_mm = R_px * sin(theta) * dx;	// X component of response in mm
	float Ry_mm = R_px * cos(theta) * dy;	// Y component of response in mm

	R_mm = sqrt(pow(Rx_mm, 2.0) + pow(Ry_mm, 2.0));

	return R_mm;
}

cv::Mat ImageProcessing::DrawMainDropCent(cv::Mat GrayscaleImg, cv::Mat ColorImg, int thresh_type, cv::Point2f MainDropPoint)
{
	cv::Mat drawing = ColorImg.clone();
	// Centroid drawin in red colour
	cv::Scalar red = Scalar(0, 0, 255);
	// draw in radius of 2
	int radius = 2;

	//cv::Point2f MainDropPos = FindMainDropPos(GrayscaleImg, thresh_type);
	std::vector<cv::Point2f>Centers2Draw;
	Centers2Draw.push_back(MainDropPoint);

	// now draw main drop positions
	// draw only if position is valid (ie >= 0)
	if (MainDropPoint.y >= 0) drawing = DrawSelectedCentroids(ColorImg, Centers2Draw, red, radius);

	return drawing;
}

cv::Mat ImageProcessing::DrawAllSatellites(cv::Mat GrayscaleImg, cv::Mat ColorImg, int thresh_type, float MainDropPos_mm)
{
	cv::Mat drawing;
	// Satellites drawn in yellow
	cv::Scalar yellow = cv::Scalar(0, 255, 255);
	// circle radius for drawing
	int radius = 2;

	cv::Mat bin = BinaryThresh(GrayscaleImg, thresh_type);
	std::vector<cv::Point2f> Centers = ImageCentroids(bin);
	
	if (MainDropPos_mm >= 0)
	{
		// main drop detected so remove the centroid with max position
		for (int i = 0; i < Centers.size(); i++)
		{
			if (Centers[i] == MaxImageCentroid(Centers))
			{
				Centers.erase(Centers.begin() + i);
				break;
			}
		}
	}

	// Now draw
	drawing = DrawSelectedCentroids(ColorImg, Centers, yellow, radius);

	return drawing;
}

cv::Mat ImageProcessing::DrawLigamentLength(cv::Mat GrayscaleImg, cv::Mat ColorImg, int thresh_type)
{
	std::vector<cv::Point2f>LigPts = LigPoints(GrayscaleImg, thresh_type);
	cv::Scalar white = cv::Scalar(255, 255, 255);
	int thickness = 4;
	cv::Mat drawing;
	/*if (LigPts.size() > 0)
	{
		drawing = DrawLine2Points(ColorImg, LigPts, white, thickness);
	}
	else {
		drawing = ColorImg.clone();
	}*/

	//// do different method
	//// get tuple of lig points
	std::tuple<cv::Point2f, cv::Point2f, cv::Point2f, cv::Point2f> ll_pts = LigLenPoints(GrayscaleImg, thresh_type);
	// make vector of bottom and top most points
	std::vector<cv::Point2f> ll{ std::get<2>(ll_pts), std::get<3>(ll_pts) };
	if (LigPts.size() > 0)
	{
		drawing = DrawLine2Points(ColorImg, ll, white, thickness);
	}
	else {
		drawing = ColorImg.clone();
	}

	return drawing;
}

cv::Mat ImageProcessing::DrawMainDropVolume(cv::Mat GrayscaleImg, cv::Mat ColorImg, int thresh_type)
{
	cv::Mat drawing = ColorImg.clone();

	// canny edge detector params
	// may need to change if changed in MainDropVol
	int LowThresh = 127;
	int HighThresh = 255;
	int edgeThresh = 1;
	int kernal_size = 3;

	// will draw all drops not just main one
	cv::Mat MDMask = CannyEdgeDetect(GrayscaleImg, HighThresh, LowThresh, edgeThresh, kernal_size);

	//cv::Mat MDMask = MainDropMask(GrayscaleImg, thresh_type);
	std::vector<std::vector<cv::Point>> Contours;
	std::vector<cv::Vec4i> Heirarchy;
	cv::Scalar green = cv::Scalar(0, 255, 0);

	// find contours to fill
	cv::findContours(MDMask, Contours, Heirarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

	for (int i = 0; i < Contours.size(); i++)
	{
		cv::drawContours(drawing, Contours, i, green, cv::FILLED);
	}

	return drawing;
}



void ImProcTest::test_ocv(void)
{
	const char* file = "C:/Users/akshi/Google Drive/Thesis/Thesis A Images/binary_thresholding.jpg";
	cv::Mat test = cv::imread(file, cv::IMREAD_COLOR);
	imshow("test", test);
	cv::waitKey();
}

bool ImProcTest::test_preprocessing(std::string input_dir, std::string output_dir, int thresh_type)
{
	bool success = true;
	// filename temp for output
	std::string	fpOut = output_dir + std::string("/binarized_");
	std::string fpFull;
	std::string exten = ".jpg";
	int counter = 1;
	cv::Mat preproc_img;
	// Get a vector of grayscale images
	std::vector<cv::Mat> grayscale_images = ImageProcessing::get_images(input_dir, cv::IMREAD_GRAYSCALE);
	// iterrate through Mat vector and save in output directory
	for (std::vector<cv::Mat>::iterator it = grayscale_images.begin(); it != grayscale_images.end(); ++it)
	{
		// binarise image
		preproc_img = ImageProcessing::BinaryThresh(*it, thresh_type);
		// filename
		fpFull = fpOut + std::to_string(counter) + exten;
		// write to output directory
		if (!cv::imwrite(fpFull, preproc_img))
		{
			// something went wrong so break
			success = false;
			break;
		}
		counter++;

	}

	return success;
}

void ImProcTest::test_DrawContours(std::string input_dir, std::string output_dir, int thresh_type)
{
	std::vector<std::string> fpList = file_system::ListOfFiles(input_dir);
	bool IncludeStaellites = false;
	bool NoiseReduction = false;
	cv::Mat binary_image;
	cv::Mat contour_img;

	// Output string 
	std::string	fpOut = output_dir + std::string("/DrawContour_");
	std::string fpFull;
	std::string exten = ".jpg";

	for (int i = 0; i < fpList.size(); i++)
	{
		// get grayscale images
		std::vector<cv::Mat> grayscale_images = ImageProcessing::get_images(input_dir, cv::IMREAD_GRAYSCALE);
		// get colour images
		std::vector<cv::Mat> colour_images = ImageProcessing::get_images(input_dir, cv::IMREAD_COLOR);

		// loop through and get contoured image from each vector
		// show processed image in imshow
		for (int j = 0; j < colour_images.size(); j++)
		{
			binary_image = ImageProcessing::BinaryThresh(grayscale_images[i], thresh_type);
			contour_img = ImageProcessing::DrawContours(binary_image, colour_images[i], IncludeStaellites, NoiseReduction);

			fpFull = fpOut + std::to_string(j+1) + exten;
			//std::cout << "Saving " << fpFull << std::endl;
		
			cv::imwrite(fpFull, contour_img);
			/*cv::imshow("Contoured Image", contour_img);
			cv::waitKey(0);
			cv::destroyWindow("Contoured Image");*/
		}
	}
}

void ImProcTest::test_image_subtraction(cv::Mat gry_ref, cv::Mat gry_i, cv::Mat clr_i, int thresh_type)
{
	cv::Mat sub = ImageProcessing::GrayImageSubtraction(gry_ref, gry_i);

	// binarise
	cv::Mat bin = ImageProcessing::BinaryThresh(sub, thresh_type);
	cv::Mat bin_i = ImageProcessing::BinaryThresh(gry_i, thresh_type);

	// show images
	//cv::imshow("Reference", gry_ref);
	cv::imshow("ith Grey Image", gry_i);
	cv::imshow("Subtraction", sub);
	cv::imshow("Binary of Subtraction", bin);
	cv::imshow("Binary of ith Gray", bin_i);
	cv::waitKey(0);
	cv::destroyAllWindows();

}

cv::Point2f NumericalMethods::ForwardDifference(cv::Point2f rt0, cv::Point2f rt1)
{
	return 2 * rt1 - rt0;
}
