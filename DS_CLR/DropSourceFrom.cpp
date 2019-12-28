#include "DropSourceFrom.h"
//#include "UI.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace DSCLR;
//using namespace UI_ERROR;

// Definitions
#define MAX_SIM_CROP_ROW	100

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
		TestFunctions();
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

void DSCLR::DropSourceFrom::setWidthAndHeight_Px(float width, float height)
{
	this->ImageWidth_Px = width;
	this->ImageHeight_Px = height;
}

float DSCLR::DropSourceFrom::Pixels2mm(float data, bool isWidth)
{
	float data_mm = 0;
	float conversion_factor = 0;

	float ROI_Width = (float)(Convert::ToDouble(this->Width_text->Text));	// mm
	float ROI_Height = (float)(Convert::ToDouble(this->Height_txt->Text));	// mm

	if (isWidth)
	{
		// converting width
		conversion_factor = ROI_Width / this->ImageWidth_Px;				// mm/px
	}
	else
	{
		// converting height 
		conversion_factor = ROI_Height / this->ImageHeight_Px;				// mm/px
	}

	data_mm = data * conversion_factor;										// mm

	return data_mm;
}

cv::Mat DSCLR::DropSourceFrom::SimCropBinaryImage(cv::Mat bin_image)
{
	cv::Mat crop_img = bin_image.clone();

	for (int row = 0; row < MAX_SIM_CROP_ROW; row++)
	{
		for (int col = 0; col < crop_img.cols; col++)
		{
			crop_img.at<int>(row, col) = 0;
		}
	}
	
	return crop_img;
}

/*IMAGE HANDLING METHODS***************************************************************************/

// returns vector of image in desired load type (ie colour or grayscale)
std::vector<cv::Mat> DSCLR::DropSourceFrom::LoadImages(int IMREAD_TYPE)
{
	std::vector<cv::Mat> images;
	cv::Mat img;
	System::String^ col_type;
	float width, height;

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

	// Don't forget to set width and height
	width = images[0].size().width;
	height = images[0].size().height;
	this->setWidthAndHeight_Px(width, height);

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

void DSCLR::DropSourceFrom::MakeTimeVector(int size)
{
	//std::vector<float> TimeVector;

	float delta_t = 1/((float)(Convert::ToDouble(this->FPS_text->Text)))*1000;
	int count = 0;
	float time_t = 0;
	for (int i = 0; i < size; i++)
	{
		this->TimeVector->push_back(time_t);
		time_t += delta_t;
	}

}

void DSCLR::DropSourceFrom::UpdateMainDropPosition(float drop_pos_data_px)
{
	// Convert to mm first
	float data_mm = Pixels2mm(drop_pos_data_px, false);
	// Append to private vector member
	MainDropPosition->push_back(data_mm);
}

void DSCLR::DropSourceFrom::UpdateNumberOfSatellites(int num_sat_data)
{
	// Append to private vector member
	NumberOfSatellites->push_back(num_sat_data);
}

void DSCLR::DropSourceFrom::DropletAnalysis()
{
	int MaxDrops = 0;
	// data is set to -1 to indicate main drop is not detected
	float DropPosData_px = -1;
	// init number of satellites to 0
	int NumSat = 0;
	// binary image
	cv::Mat bin_img;
	// centroids
	std::vector<cv::Point2f> Centroids;

	// Load grayscale images
	std::vector<cv::Mat> GrayscaleImages = LoadImages(cv::IMREAD_GRAYSCALE);

	// Loop through grayscale images
	for (int i = 0; i < GrayscaleImages.size(); i++)
	{
		// binary thresholding
		bin_img = ImageProcessing::BinaryThresh(GrayscaleImages[i]);
		// find centroids
		Centroids = ImageProcessing::ImageCentroids(bin_img);

		// Following process to figure out if main drop is on screen
		if (MaxDrops <= Centroids.size())
		{
			// Either main drop has broken into more satellites or just appeared
			// Record main drop position now
			DropPosData_px = ImageProcessing::MaxImageCentroid_Y(Centroids);
			//UpdateMainDropPosition(ImageProcessing::MaxImageCentroid_Y(Centroids));

			// Update number of satellites, remember that one of the centroids is the main drop
			NumSat = Centroids.size() - 1;

			// update max drops that have been on screen
			MaxDrops = Centroids.size();
		}
		else 
		{
			// Main drop has dissapeared or has yet to appear
			// check if any satellites present
			NumSat = Centroids.size();
			DropPosData_px = -1;
		}

		// update position and number of satellites
		UpdateMainDropPosition(DropPosData_px);
		UpdateNumberOfSatellites(NumSat);

		// TODO: Velocity, ligament length, volume
	}

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
	bool IncludeStaellites = true;
	bool NoiseReduction = false;
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
		
		if (this->SimulateCrop->Checked)
		{
			binary_image = SimCropBinaryImage(binary_image);
		}

		contour_img = ImageProcessing::DrawContours(binary_image, colour_images[j], IncludeStaellites, NoiseReduction);

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
	this->Update();
	System::Threading::Thread::Sleep(1000);

	this->ProgressBar->Visible = false;
	this->PB_Label->Visible = false;
	

	return success;
}

bool DSCLR::DropSourceFrom::TestTimeVector()
{
	bool success = true;
	std::vector<cv::Mat> imgs = LoadImages(cv::IMREAD_GRAYSCALE);

	int size = imgs.size();

	MakeTimeVector(size);

	return success;
}

void DSCLR::DropSourceFrom::TestFunctions()
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

	if (TEST_TIME_VECTOR)
	{
		TestTimeVector();
	}

	if (TEST_DROPLET_ANALYSIS)
	{
		DropletAnalysis();
	}
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

