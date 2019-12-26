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
	cv::adaptiveThreshold(pre_proc, pre_proc, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 11, 2);

	return pre_proc;
}

cv::Mat ImageProcessing::DrawContours(cv::Mat bin_img, cv::Mat colr_img, bool IncludeSatellites)
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
		// check if you want to include satellites or not
		if (!IncludeSatellites)
		{
			// Less than max satellite size, consider as satellite
			if (cv::contourArea(Contours[i]) < MAX_SATELLITE_SIZE) {
				continue;
			}
		}
		// Draw contours
		cv::drawContours(cont_img, Contours, i, colour, -1);
		// Put center of mass on
		cv::circle(cont_img, mc[i], radius, colour2, -1);
	}

	return cont_img;
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
			contour_img = ImageProcessing::DrawContours(binary_image, colour_images[i], IncludeStaellites);

			fpFull = fpOut + std::to_string(j+1) + exten;
			//std::cout << "Saving " << fpFull << std::endl;
		
			cv::imwrite(fpFull, contour_img);
			/*cv::imshow("Contoured Image", contour_img);
			cv::waitKey(0);
			cv::destroyWindow("Contoured Image");*/
		}
	}
}
