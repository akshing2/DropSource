#include "ImageProc.h"
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
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

std::vector<cv::Mat> ImageProcessing::get_images(std::string input_dir, int IMREAD_TYPE)
{
	std::vector<cv::Mat> images;
	cv::Mat img;
	// Get the filepaths of all the images as a string vector
	std::vector<std::string> fp_input = file_system::ListOfFiles(input_dir);

	// iterate through fp_input and save into grayscale images
	for (std::vector<std::string>::iterator it = fp_input.begin(); it != fp_input.end(); ++it)
	{
		img = cv::imread(*it, IMREAD_TYPE);
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

void ImProcTest::test_ocv(void)
{
	const char* file = "C:/Users/akshi/Google Drive/Thesis/Thesis A Images/binary_thresholding.jpg";
	cv::Mat test = cv::imread(file, cv::IMREAD_COLOR);
	imshow("test", test);
	cv::waitKey();
}

void ImProcTest::test_preprocessing(std::string input_dir, std::string output_dir)
{
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
		cv::imwrite(fpFull, preproc_img);
		counter++;
	}
}
