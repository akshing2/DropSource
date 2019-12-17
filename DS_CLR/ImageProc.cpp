#include "ImageProc.h"
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
//#include "DropSourceFrom.h"
#include  <filesystem>

namespace fs = std::filesystem;
using namespace cv;
using recurv_dir_it = std::filesystem::recursive_directory_iterator;
void test_ocv(void)
{
	const char* file = "C:/Users/akshi/Google Drive/Thesis/Thesis A Images/binary_thresholding.jpg";
	cv::Mat test = cv::imread(file, cv::IMREAD_COLOR);
	imshow("test", test);
	cv::waitKey();
}

std::vector<std::string> ListOfFiles(std::string path)
{
	std::vector<std::string> fpList;

	for (auto& dirEntry : std::filesystem::recursive_directory_iterator(path))
	{
		std::filesystem::path file = dirEntry.path();
		fpList.push_back(file.string());
	}
		

	return fpList;
}

