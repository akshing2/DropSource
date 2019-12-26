#include "DropSourceFrom.h"
//#include "UI.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace DSCLR;
//using namespace UI_ERROR;

// Start of Form Functions
System::Void DSCLR::DropSourceFrom::StartAnalysis_button_Click(System::Object^ sender, System::EventArgs^ e)
{
	bool err = false;

	if (UI_INPUR_ERROR_CHECK)
	{
		err = User_Input_Error_Check();
	}

	if (!err)
	{
		if (TEST_OCV)
			ImProcTest::test_ocv();

		if (TEST_LIST_FILES)
			file_system::ListOfFiles(UI_ERROR::SYS2std_string(this->InputDir_text->Text));

		if (TEST_PREPROC)
		{
			TestPreProcessing();
		}

		if (TEST_DRAW_CONTOURS)
		{
			TestDrawContours();
		}

	}
	return System::Void();
}

// Class Function Definitions for DropSourceForm
bool DSCLR::DropSourceFrom::User_Input_Error_Check()
{
	bool err = false;
	// Check name of test is not empty
	this->Error_NoT->Visible = check_non_empty_field(NameOfTest_Text->Text);
	err = this->Error_NoT->Visible;
	//check ROI
	this->Error_ROI->Visible = (check_numerical_vals_input(Width_text->Text)) || (check_numerical_vals_input(Height_txt->Text));
	if (!err) err = this->Error_ROI->Visible;
	// check FPS
	this->Error_FPS->Visible = (check_numerical_vals_input(FPS_text->Text));
	if (!err) err = this->Error_FPS->Visible;
	// check Input Directory
	this->Error_Input->Visible = check_non_empty_field(InputDir_text->Text);
	if (!err) err = this->Error_Input->Visible;
	// check output directory
	this->Error_Output->Visible = check_non_empty_field(OutputDir_text->Text);
	if (!err) err = this->Error_Output->Visible;
	return err;
}

bool DSCLR::DropSourceFrom::Process_Video()
{
	return false;
}

/*IMAGE HANDLING METHODS***************************************************************************/

// returns vector of image in desired load type (ie colour or grayscale)
std::vector<cv::Mat> DSCLR::DropSourceFrom::LoadImages(int IMREAD_TYPE)
{
	std::vector<cv::Mat> images;
	cv::Mat img;
	System::String^ col_type;

	// Get the filepaths of all the images as a string vector
	std::vector<std::string> fp_input = file_system::ListOfFiles(UI_ERROR::SYS2std_string(this->InputDir_text->Text));

	// This type of string needed for form
	System::String^ pb_str = "Reading in images from input directory...";
	int ProgressVal = 0;
	// Set GUI Params
	this->ProgressBar->Visible = true;
	this->ProgressBar->Minimum = 0;
	this->ProgressBar->Maximum = fp_input.size();
	this->PB_Label->Text = pb_str;
	this->ProgressBar->Value = ProgressVal;
	this->PB_Label->Visible = true;
	this->ProgressBar->Visible = true;
	
	switch (IMREAD_TYPE)
	{
	case cv::IMREAD_ANYCOLOR:
		col_type = "IMREAD_ANYCOLOR";
		break;
	case cv::IMREAD_COLOR:
		col_type = "IMREAD_COLOR";
		break;
	case cv::IMREAD_GRAYSCALE:
		col_type = "IMREAD_GRAYSCALE";
		break;
	default:
		col_type = "UNKNOWN IMREAD";
	}

	// iterate through fp_input and save into grayscale images
	for (int i = 0; i < fp_input.size(); i++)
	{
		img = ImageProcessing::GetInputImage(fp_input[i], IMREAD_TYPE);
		images.push_back(img);

		// Don't forget to update progress bar
		//ProgressVal = (int)round((i / fp_input.size()) * 100);
		ProgressVal = i;
		this->ProgressBar->Value = ProgressVal;
		this->ProgressBar->Update();
		pb_str = col_type + "-Reading in file: " + i + "/" + fp_input.size();
		this->PB_Label->Text = pb_str;
		this->PB_Label->Update();
		
	}

	// Finished loading
	pb_str = "Finished Loading!";
	this->PB_Label->Text = pb_str;
	ProgressVal = 100;
	this->ProgressBar->Value = ProgressVal;

	// Set progress par invisible again
	this->PB_Label->Visible = false;
	this->ProgressBar->Visible = false;
	this->ProgressBar->Value = 0;

	return images;
}

std::vector<cv::Point2f> DSCLR::DropSourceFrom::ImageCentroids(cv::Mat binary_image)
{
	std::vector<cv::Point2f> centers;
	std::vector<std::vector<cv::Point>> Contours;
	std::vector<cv::Vec4i> Heirarchy;
	
	// find contours
	// Parameters found in python script
	cv::findContours(binary_image, Contours, Heirarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

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

	return centers;
}

bool DSCLR::DropSourceFrom::TestPreProcessing()
{
	// format input and output directories
	std::string output_dir = UI_ERROR::SYS2std_string(this->OutputDir_text->Text);
	std::string input_dir = UI_ERROR::SYS2std_string(this->InputDir_text->Text);
	bool success = true;
	// filename temp for output
	std::string	fpOut = output_dir + std::string("/binarized_");
	std::string fpFull;
	std::string exten = ".jpg";
	int counter = 1;
	cv::Mat preproc_img;

	// For progress bar
	System::String^ pb_str = "Commencing Binary Thresholding...";
	int ProgressVal = 0;

	// Get a vector of grayscale images
	std::vector<cv::Mat> grayscale_images = LoadImages(cv::IMREAD_GRAYSCALE);

	this->ProgressBar->Value = 0;
	this->ProgressBar->Visible = true;
	this->PB_Label->Text = pb_str;
	this->PB_Label->Visible = true;
	this->ProgressBar->Minimum = 0;
	this->ProgressBar->Maximum = grayscale_images.size();
	this->PB_Label->Update();
	this->ProgressBar->Update();

	// iterrate through Mat vector and save in output directory
	for (int i = 0; i < grayscale_images.size(); i++)
	{
		// binarise image
		preproc_img = ImageProcessing::BinaryThresh(grayscale_images[i]);
		// filename
		fpFull = fpOut + std::to_string(counter) + exten;
		// write to output directory
		if (!cv::imwrite(fpFull, preproc_img))
		{
			// something went wrong so break
			success = false;
			break;
		}

		// Update progress bar
		pb_str = "Binary Thresholding: " + i + "/" + grayscale_images.size();
		this->PB_Label->Text = pb_str;
		ProgressVal = i;
		this->ProgressBar->Value = ProgressVal;
		this->PB_Label->Update();
		this->ProgressBar->Update();

	}

	pb_str = "Success = " + success;
	this->PB_Label->Text = pb_str;

	System::Threading::Thread::Sleep(1000);

	this->ProgressBar->Visible = false;
	this->PB_Label->Visible = false;

	return success;
}

bool DSCLR::DropSourceFrom::TestDrawContours()
{
	bool success = true;

	// format input and output directories
	std::string output_dir = UI_ERROR::SYS2std_string(this->OutputDir_text->Text);
	std::string input_dir = UI_ERROR::SYS2std_string(this->InputDir_text->Text);

	std::vector<std::string> fpList = file_system::ListOfFiles(input_dir);
	bool IncludeStaellites = false;
	cv::Mat binary_image;
	cv::Mat contour_img;

	// Output string 
	std::string	fpOut = output_dir + std::string("/DrawContour_");
	std::string fpFull;
	std::string exten = ".jpg";

	// Progress Bar init
	int ProgressVal = 0;
	System::String^ pb_str = "Commencing Drawing Contours...";


	// get grayscale images
	std::vector<cv::Mat> grayscale_images = LoadImages(cv::IMREAD_GRAYSCALE);
	// get colour images
	std::vector<cv::Mat> colour_images = LoadImages(cv::IMREAD_COLOR);

	// loop through and get contoured image from each vector
	// show processed image in imshow
	this->ProgressBar->Value = ProgressVal;
	this->ProgressBar->Visible = true;
	this->PB_Label->Text = pb_str;
	this->PB_Label->Visible = true;
	this->ProgressBar->Minimum = 0;
	this->ProgressBar->Maximum = colour_images.size();
	this->ProgressBar->Update();
	this->PB_Label->Update();

	for (int j = 0; j < colour_images.size(); j++)
	{
		binary_image = ImageProcessing::BinaryThresh(grayscale_images[j]);
		contour_img = ImageProcessing::DrawContours(binary_image, colour_images[j], IncludeStaellites);

		fpFull = fpOut + std::to_string(j + 1) + exten;
		//std::cout << "Saving " << fpFull << std::endl;

		if (!cv::imwrite(fpFull, contour_img))
		{
			success = false;
			break;
		}
		/*cv::imshow("Contoured Image", contour_img);
		cv::waitKey(0);
		cv::destroyWindow("Contoured Image");*/

		//Update Progress Bar
		pb_str = "Drawing Contour: " + j + "/" + colour_images.size();
		ProgressVal = j;
		this->PB_Label->Text = pb_str;
		this->ProgressBar->Value = ProgressVal;
		this->ProgressBar->Update();
		this->PB_Label->Update();
	}

	// end progress bar
	pb_str = "Success = " + success;
	this->PB_Label->Text = pb_str;

	System::Threading::Thread::Sleep(1000);

	this->ProgressBar->Visible = false;
	this->PB_Label->Visible = false;
	this->Update();

	return success;
}


// Main Function
// Where the form runs
[STAThread]
void main(cli::array<System::String^>^ args)
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	DSCLR::DropSourceFrom form;
	Application::Run(% form);
}

