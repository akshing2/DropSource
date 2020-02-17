#include "ImageProc.h"
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <iostream>
//#include "DropSourceFrom.h"
#include  <filesystem>

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

cv::Mat ImageProcessing::BinaryThresh(cv::Mat image)
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
	cv::adaptiveThreshold(pre_proc, pre_proc, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 9, 2);

	return pre_proc;
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

cv::Point2f ImageProcessing::FindMainDropPos(cv::Mat grayscale_img)
{
	cv:Point2f pos;

	cv::Mat bin_img = BinaryThresh(grayscale_img);

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

std::vector<cv::Rect2f> ImageProcessing::FindBoundingRects(cv::Mat grayscale_img)
{
	std::vector<cv::Rect2f> BoundRects;
	std::vector<std::vector<cv::Point>> Contours;
	std::vector<cv::Vec4i> Heirarchy;
	// Set colour of drawing
	cv::Scalar colour = Scalar(0, 0, 255);
	cv::Scalar colour2 = Scalar(0, 255, 0);
	
	// binary threshold
	cv::Mat bin_img = BinaryThresh(grayscale_img);

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

cv::Rect2f ImageProcessing::FindMainDropRect(cv::Mat grayscale_img)
{
	cv::Rect2f MainDropRect;
	std::vector<cv::Rect2f> Rects;

	Rects = FindBoundingRects(grayscale_img);
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

cv::Point2f ImageProcessing::FindBottomMostPoint(cv::Mat grayscale_img)
{
	cv::Point2f bottom;
	std::vector<std::vector<cv::Point>> Contours;
	std::vector<cv::Point> cnts;
	std::vector<cv::Vec4i> Heirarchy;
	cv::Point extBot(-1,-1);

	// binarise image
	cv::Mat bin_img = BinaryThresh(grayscale_img);

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

float ImageProcessing::LengthOfLigament(cv::Mat grayscale_img)
{
	float LigLength = -1;

	// 1. find main drop position
	cv::Point2f pos = FindMainDropPos(grayscale_img);

	// 2. find extreme bottom
	cv::Point2f ExtBot = FindBottomMostPoint(grayscale_img);

	// 3. find radius of head
	float HeadRadius = Distance2Points(pos, ExtBot);

	// 4. find bounding rectangle
	cv::Rect2f Rect = FindMainDropRect(grayscale_img);

	// 5. find edge of head and ligament, p1
	cv::Point2f p1 = pos;
	p1.y = p1.y - HeadRadius;

	// 6. find tail end of ligament, p2
	cv::Point2f p2 = pos;
	p2.y = p2.y + HeadRadius - Rect.height;

	if (Rect.height > HEIGHT_WIDTH_CMP* Rect.width)
	{
		// if height much greater than width, calculate ligament
		LigLength = Distance2Points(p2, p1);
	}

	return LigLength;
}

double ImageProcessing::FindMaxContourArea(cv::Mat grayscale_img)
{
	std::vector<std::vector<cv::Point>> Contours;
	std::vector<cv::Vec4i> Heirarchy;
	float MaxContourArea = 0;

	// image processing
	cv::Mat bin_img = BinaryThresh(grayscale_img);

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

double ImageProcessing::FindMaxCCArea(cv::Mat Stats)
{
	double MaxCCArea;
	double MinCCArea;
	cv::Point minloc;
	cv::Point maxloc;

	cv::Mat inspect = Stats.col(4);
	
	cv::minMaxLoc(inspect, &MinCCArea, &MaxCCArea, &minloc, &maxloc);

	return MaxCCArea;
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


cv::Mat ImageProcessing::MainDropMask(cv::Mat grayscale_img)
{
	// for connected components
	cv::Mat stats, centroids, labelImage;

	// binarize image
	cv::Mat bin_img = BinaryThresh(grayscale_img);

	// connected components
	int nLabels = cv::connectedComponentsWithStats(bin_img, labelImage, stats, centroids, 4);

	// make mask
	cv::Mat mask(labelImage.size(), bin_img.type(), cv::Scalar(255));
	cv::Mat surfSup = stats.col(4) > FindMaxContourArea(grayscale_img);

	for (int i = 1; i < nLabels; i++)
	{
		if (surfSup.at<uchar>(i, 0))
		{
			mask = mask & (labelImage == i);
		}

		/*if (i == FindHighestLabel(centroids, nLabels))
		{
			mask = mask & (labelImage == i);
		}*/
	}

	cv::Mat r(bin_img.size(), bin_img.type(), cv::Scalar(0));
	bin_img.copyTo(r, mask);

	//cv::Mat r = labelImage;

	return r;
}

void ImProcTest::test_ocv(void)
{
	const char* file = "C:/Users/akshi/Google Drive/Thesis/Thesis A Images/binary_thresholding.jpg";
	cv::Mat test = cv::imread(file, cv::IMREAD_COLOR);
	imshow("test", test);
	cv::waitKey();
}

bool ImProcTest::test_preprocessing(std::string input_dir, std::string output_dir)
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
		preproc_img = ImageProcessing::BinaryThresh(*it);
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

void ImProcTest::test_DrawContours(std::string input_dir, std::string output_dir)
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
			binary_image = ImageProcessing::BinaryThresh(grayscale_images[i]);
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

cv::Point2f NumericalMethods::ForwardDifference(cv::Point2f rt0, cv::Point2f rt1)
{
	return 2 * rt1 - rt0;
}
