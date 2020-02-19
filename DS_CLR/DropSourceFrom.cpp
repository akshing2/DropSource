#include "DropSourceFrom.h"
//#include "UI.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace DSCLR;
//using namespace UI_ERROR;

// Definitions
#define MAX_SIM_CROP_ROW	100
// Gravitation acceleration constant
#define g					9.8*1000	// mm/s
// Error Control Constant
#define Ep					0.70
// Default error value
#define DEF_ERR				-100

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
		if (TEST_ENABLE)
		{
			TestFunctions();
		}
		else if (MAIN_ENABLE)
		{
			DropletAnalysis();
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
	// check output file has been selected
	this->Error_OutputFile->Visible = !(this->CSV_cbox->Checked + this->DebugImg_cbox->Checked + this->XLSX_cbox->Checked);
	if (!err) err = this->Error_OutputFile->Visible;
	// check at least one parameter is checked
	this->Error_SelectParam->Visible = !(Position_cbox->Checked + Velocity_cbox->Checked + Satellites_cbox->Checked + LigLength_cbox->Checked + DropVolume_cbox->Checked);
	if (!err) err = this->Error_SelectParam->Visible;
	return err;
}

bool DSCLR::DropSourceFrom::Process_Video()
{
	return false;
}

void DSCLR::DropSourceFrom::ClearAllData()
{
	// reset
	ImageWidth_Px = 0;
	ImageHeight_Px = 0;

	// clear images
	GrayscaleImages->clear();
	ColorImages->clear();

	// Clear centers
	MainDropPoints->clear();
	MainDropPredic->clear();
	MainDropControl->clear();

	// clear velocity
	Velocity_px->clear();

	// clear data set
	TimeVector->clear();
	MainDropPosition->clear();
	MainDropVelocity->clear();
	NumberOfSatellites->clear();
}

void DSCLR::DropSourceFrom::setWidthAndHeight_Px(float width, float height)
{
	this->ImageWidth_Px = width;
	this->ImageHeight_Px = height;
}

float DSCLR::DropSourceFrom::getDeltaT()
{
	float delta_t = 1 / ((float)(Convert::ToDouble(this->FPS_text->Text))) * 1000;

	return delta_t;
}

float DSCLR::DropSourceFrom::Pixels2mm(float data, bool isWidth)
{
	float data_mm = -1;
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

	if (data > -1)
	{
		// -1 means invalid data, so if it isnt -1 then do conversion
		data_mm = data * conversion_factor;										// mm
	}
	
	return data_mm;
}

float DSCLR::DropSourceFrom::mm2Pixel(float data, bool isWidth)
{
	float data_px = 0;
	float conversion_factor = 0;

	float ROI_Width = (float)(Convert::ToDouble(this->Width_text->Text));	// mm
	float ROI_Height = (float)(Convert::ToDouble(this->Height_txt->Text));	// mm

	if (isWidth)
	{
		// converting width
		conversion_factor = this->ImageWidth_Px/ROI_Width;					// px/mm
	}
	else
	{
		// converting height 
		conversion_factor = this->ImageHeight_Px / ROI_Height; 				// px/mm
	}

	data_px = data * conversion_factor;										// px

	return data_px;
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

cv::Point2f DSCLR::DropSourceFrom::PredictNextMainDropPosition(cv::Point2f detected_pos, int index)
{
	cv::Point2f pred = detected_pos;
	cv::Mat bin_image;
	//float delta_t = 1 / ((float)(Convert::ToDouble(this->FPS_text->Text))) * 1000;
	// can only do calculation if index is greater than 2
	// otherwise return the same detected position
	if (index > 2)
	{
		bin_image = ImageProcessing::BinaryThresh(GrayscaleImages->at(index - 2));
		// get previous two positions
		cv::Point2f rt0 = ImageProcessing::MaxImageCentroid(ImageProcessing::ImageCentroids(bin_image));
		bin_image = ImageProcessing::BinaryThresh(GrayscaleImages->at(index - 1));
		cv::Point2f rt1 = ImageProcessing::MaxImageCentroid(ImageProcessing::ImageCentroids(bin_image));

		// predict y position, assume same x
		// Neglect the acceleration due to gravity because each time instant is short
		// This calculation is done using forward difference method
		pred.y = 2 * rt1.y - rt0.y; 
	}

	return pred;
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

void DSCLR::DropSourceFrom::LoadGrayscaleImages()
{
	std::vector<cv::Mat> gs = LoadImages(cv::IMREAD_GRAYSCALE);
	for (int i = 0; i < gs.size(); i++)
	{
		GrayscaleImages->push_back(gs[i]);
	}
}

void DSCLR::DropSourceFrom::LoadColorImages()
{
	std::vector<cv::Mat> cl = LoadImages(cv::IMREAD_COLOR);

	for (int i = 0; i < cl.size(); i++)
	{
		ColorImages->push_back(cl[i]);
	}
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

void DSCLR::DropSourceFrom::MainDropPositions()
{
	cv::Point2f detected_centroid;
	cv::Point2f predicted_centroid;
	cv::Point2f control_centroid;
	std::vector<cv::Point2f> Centers;
	float center_mm_y;
	cv::Mat bin_img;
	float halfway_y = this->ImageHeight_Px / 2;
	bool isMainDrop = false;
	bool endMainDrop = false;


	System::String^ pb_str = "Detecting Main Drop Positions";
	ProgressBarUpdate(pb_str, 0, 100, 0, true);

	// Loop through grayscale images
	for (int i = 0; i < GrayscaleImages->size(); i++)
	{
		bin_img = ImageProcessing::BinaryThresh(GrayscaleImages->at(i));
		Centers = ImageProcessing::ImageCentroids(bin_img);
		pb_str = "Main Drop Pos: " + i + "/" + GrayscaleImages->size();
		// determine if main drop is on screen
		if (Centers.size() > 0 && !endMainDrop)
		{
			// just started main drop or continuing main drop
			isMainDrop = true;
		} else
		{
			isMainDrop = false;
		}
		
		if (!isMainDrop)
		{
			// main drop not found
			detected_centroid.x = -1;
			detected_centroid.y = -1;
			control_centroid = detected_centroid;
			predicted_centroid.x = -1;
			predicted_centroid.y = -1;
		} else 
		{
			// main drop detected
			detected_centroid = ImageProcessing::MaxImageCentroid(Centers);
			// predictionb calculated based off previous detected centers
			predicted_centroid = PredictNextMainDropPosition(detected_centroid, i);
			// Control between detected and predicted
			// This is saved as a percentage error
			control_centroid.x = abs((detected_centroid.x - predicted_centroid.x) / detected_centroid.x);
			control_centroid.y = abs((detected_centroid.y - predicted_centroid.y) / detected_centroid.y);

			if (predicted_centroid.y > this->ImageHeight_Px)
			{
				// main drop has gone off screen
				isMainDrop = false;
				endMainDrop = true;
			}
		}
		
		this->MainDropPoints->push_back(detected_centroid);
		this->MainDropPredic->push_back(predicted_centroid);
		this->MainDropControl->push_back(control_centroid);

		// Save position in mm
		if (detected_centroid.y != -1)
		{
			center_mm_y = Pixels2mm(detected_centroid.y, false);
		}
		else
		{
			center_mm_y = -1;
		}
		
		this->MainDropPosition->push_back(center_mm_y);

		ProgressBarUpdate(pb_str, 0, GrayscaleImages->size(), i, true);
	}

	pb_str = "Finished";
	ProgressBarUpdate(pb_str, 0, ColorImages->size(), ColorImages->size(), true);
	System::Threading::Thread::Sleep(1000);
	ProgressBarUpdate(pb_str, 0, ColorImages->size(), ColorImages->size(), false);
}

void DSCLR::DropSourceFrom::MainDropVelocities()
{
	cv::Point2f Vel(-1,-1);
	float delta_t = getDeltaT();
	System::String^ pb_str = "Calculating Main Drop Velocities";
	ProgressBarUpdate(pb_str, 0, 100, 0, true);

	// first measurement always invalid
	this->Velocity_px->push_back(Vel);
	this->MainDropVelocity->push_back(Pixels2mm(Vel.y, false));

	// loop through main droplet positions and calculate velocity
	for (int i = 1; i < MainDropPoints->size(); i++)
	{
		pb_str = "Main Drop Vel: " + i + "/" + MainDropPoints->size();
		if (MainDropPoints->at(i).y <= -1)
		{
			// keep invalid formatting
			Vel = MainDropPoints->at(i);
		}
		else {
			if (MainDropPoints->at(i - 1).y <= -1)
			{
				// can't make a valid velocity calculation
				Vel = MainDropPoints->at(i-1);
			}
			else
			{
				// can calculate velocity
				Vel = (MainDropPoints->at(i) - MainDropPoints->at(i - 1)) / delta_t;
			}
		}

		this->Velocity_px->push_back(Vel);
		this->MainDropVelocity->push_back(Pixels2mm(Vel.y,false));

		ProgressBarUpdate(pb_str, 0, MainDropPoints->size(), i, true);
	}

	pb_str = "Finished";
	ProgressBarUpdate(pb_str, 0, MainDropPoints->size(), MainDropPoints->size(), true);
	System::Threading::Thread::Sleep(1000);
	ProgressBarUpdate(pb_str, 0, MainDropPoints->size(), MainDropPoints->size(), false);
}

void DSCLR::DropSourceFrom::CountNumberOfSatellites()
{
	int satellites = 0;
	std::vector<cv::Point2f> Centers;
	cv::Mat bin_img;
	bool isMainDrop = false;
	bool endMainDrop = false;


	System::String^ pb_str = "Detecting Number of Satellites";
	ProgressBarUpdate(pb_str, 0, 100, 0, true);

	// Loop through grayscale images
	for (int i = 0; i < GrayscaleImages->size(); i++)
	{
		bin_img = ImageProcessing::BinaryThresh(GrayscaleImages->at(i));
		Centers = ImageProcessing::ImageCentroids(bin_img);
		pb_str = "No. Of Satellites: " + i + "/" + GrayscaleImages->size();
		// determine if main drop is on screen
		if (Centers.size() > 0 && !endMainDrop)
		{
			// just started main drop or continuing main drop
			isMainDrop = true;
		}
		else
		{
			isMainDrop = false;
		}

		if (!isMainDrop)
		{
			// main drop not found
			satellites = Centers.size();
		}
		else
		{
			// main drop detected
			// remove one center, which is main drop
			satellites = int(Centers.size() - 1);
			cv::Point2f pred = this->MainDropPredic->at(i);
			if (pred.y > this->ImageHeight_Px)
			{
				// main drop has gone off screen
				isMainDrop = false;
				endMainDrop = true;
			}
		}

		// Save number of satellites
		NumberOfSatellites->push_back(satellites);

		ProgressBarUpdate(pb_str, 0, GrayscaleImages->size(), i, true);
	}

	pb_str = "Finished";
	ProgressBarUpdate(pb_str, 0, ColorImages->size(), ColorImages->size(), true);
	System::Threading::Thread::Sleep(1000);
	ProgressBarUpdate(pb_str, 0, ColorImages->size(), ColorImages->size(), false);
}

void DSCLR::DropSourceFrom::CalculateLigLength()
{
	float LigLen = -1;
	
	System::String^ pb_str = "Calculating Length of Ligaments";
	ProgressBarUpdate(pb_str, 0, 100, 0, true);

	for (int i = 0; i < this->GrayscaleImages->size(); i++)
	{
		pb_str = "Ligament Length: " + i + "/" + GrayscaleImages->size();

		if (this->MainDropPoints->at(i).y >= 0)
		{
			// main drop exists
			LigLen = ImageProcessing::LengthOfLigament(this->GrayscaleImages->at(i));
		}

		// append to lists
		this->LigamentLength_px->push_back(LigLen);
		this->LigamentLength->push_back(Pixels2mm(LigLen, false));

		ProgressBarUpdate(pb_str, 0, GrayscaleImages->size(), i, true);
	}

	pb_str = "Finished";
	ProgressBarUpdate(pb_str, 0, ColorImages->size(), ColorImages->size(), true);
	System::Threading::Thread::Sleep(1000);
	ProgressBarUpdate(pb_str, 0, ColorImages->size(), ColorImages->size(), false);
}

void DSCLR::DropSourceFrom::CalculateMainDropVol()
{
	float MainDropVol = -1;

	System::String^ pb_str = "Calculating Volume of Main Drop";
	ProgressBarUpdate(pb_str, 0, 100, 0, true);

	float ROI_Width = (float)(Convert::ToDouble(this->Width_text->Text));	// mm
	float ROI_Height = (float)(Convert::ToDouble(this->Height_txt->Text));	// mm
	cv::Mat MainDropImg;

	for (int i = 0; i < this->GrayscaleImages->size(); i++)
	{
		pb_str = "Drop Volume: " + i + "/" + GrayscaleImages->size();
		MainDropVol = -1;

		if (this->MainDropPoints->at(i).y >= 0)
		{
			// main drop exists
			// mask
			MainDropImg = ImageProcessing::MainDropMask(this->GrayscaleImages->at(i));
			MainDropVol = ImageProcessing::MainDropVolume(MainDropImg, ROI_Width, ROI_Height);
		}

		// append to lists
		this->Volume->push_back(MainDropVol);

		ProgressBarUpdate(pb_str, 0, GrayscaleImages->size(), i, true);
	}

	pb_str = "Finished";
	ProgressBarUpdate(pb_str, 0, ColorImages->size(), ColorImages->size(), true);
	System::Threading::Thread::Sleep(1000);
	ProgressBarUpdate(pb_str, 0, ColorImages->size(), ColorImages->size(), false);
}

void DSCLR::DropSourceFrom::DropletAnalysis()
{
	// clear everything before proceeding
	ClearAllData();

	// Load images in
	LoadGrayscaleImages();
	LoadColorImages();
	
	MakeTimeVector(this->GrayscaleImages->size());

	if (this->Position_cbox->Checked)
	{
		// do position analysis
		MainDropPositions();
	}

	if (this->Velocity_cbox->Checked)
	{
		// do velocity analysis
		MainDropVelocities();
	}

	// for future implementation
	if (this->Satellites_cbox->Checked)
	{
		// count number of satellites
		CountNumberOfSatellites();
	}

	if (this->LigLength_cbox->Checked)
	{
		// Record ligament length
		CalculateLigLength();
	}

	if (this->DropVolume_cbox->Checked)
	{
		// record droplet volume
		CalculateMainDropVol();
	}

	
	if (this->XLSX_cbox->Checked)
	{
		// write to excel file
		Write2XLSX();
	}

	if (this->CSV_cbox->Checked)
	{
		// output to csv file
		Write2CSV();
	}

}

void DSCLR::DropSourceFrom::Write2XLSX()
{
	libxl::Book* wbook = xlCreateXMLBook();
	bool success = false;
	int ParamCol[6] = {0, 1, 2, 3, 4, 5};
	System::String^ pb_str = "Writing to XLSX";
	if (wbook)
	{
		success = true;

		// create folder where file is saved
		System::String^ subdir = this->OutputDir_text->Text + "/XLSX";
		// make csv sub directory
		System::IO::Directory::CreateDirectory(subdir);
		// filepath to save to
		std::string fp = UI_ERROR::SYS2std_string(subdir + "/" + this->NameOfTest_Text->Text) + ".xlsx";

		// make sheet for workbook
		libxl::Sheet* sheet = wbook->addSheet(L"Sheet1");
		if (!sheet)
		{
			success = false;
		}
		else {
			// Make the worksheet and fill with values from test

			// make title header for spreadsheet
			sheet->writeStr(1, ParamCol[0], L"Time (ms)");
			sheet->writeStr(1, ParamCol[1], L"Position (mm)");
			sheet->writeStr(1, ParamCol[2], L"Velocity (mm/s)");
			sheet->writeStr(1, ParamCol[3], L"Number of Satellites");
			sheet->writeStr(1, ParamCol[4], L"Ligament Length (mm)");
			sheet->writeStr(1, ParamCol[5], L"Volme of Droplet (mm^3)");

			// loop through data and add
			
			ProgressBarUpdate(pb_str, 0, 100, 0, true);
			for (int row = 0; row < this->GrayscaleImages->size(); row++)
			{
				sheet->writeNum(row + 2, ParamCol[0], this->TimeVector->at(row));

				// always add position as this is always needed
				sheet->writeNum(row + 2, ParamCol[1], this->MainDropPosition->at(row));

				// add velocity if requested
				if (this->Velocity_cbox->Checked)
				{
					sheet->writeNum(row + 2, ParamCol[2], this->MainDropVelocity->at(row));
				}

				// add number of satellites if requested
				if (this->Satellites_cbox->Checked)
				{
					sheet->writeNum(row + 2, ParamCol[3], this->NumberOfSatellites->at(row));
				}

				// add ligament length if requested
				if (this->LigLength_cbox->Checked)
				{
					sheet->writeNum(row + 2, ParamCol[4], this->LigamentLength->at(row));
				}

				// add drop volume if requested
				if (this->DropVolume_cbox->Checked)
				{
					sheet->writeNum(row + 2, ParamCol[5], this->Volume->at(row));
				}
				pb_str = "CSV File: " + row + "/" + GrayscaleImages->size();
				ProgressBarUpdate(pb_str, 0, GrayscaleImages->size(), row, true);
			}
		}
		
		pb_str = "Finished. Success = " + success;
		ProgressBarUpdate(pb_str, 0, ColorImages->size(), ColorImages->size(), true);
		System::Threading::Thread::Sleep(1000);
		ProgressBarUpdate(pb_str, 0, ColorImages->size(), ColorImages->size(), false);
		// write to file

		if (success)
		{
			std::wstring wfp(fp.begin(), fp.end());
			wbook->save(wfp.c_str());
			wbook->release();
		}
		

	}
}

void DSCLR::DropSourceFrom::Write2CSV()
{
	// instantiate csv obj
	CSVWriter csv(";");
	// place holder values of columns for each row
	float Pos = DEF_ERR;
	float Vel = DEF_ERR;
	int NumSat = DEF_ERR;
	float LigLen = DEF_ERR;
	float Volume = DEF_ERR;
	

	// csv sub directory path
	System::String^ subdir = this->OutputDir_text->Text + "/CSV";
	//std::string subdir = UI_ERROR::SYS2std_string(this->OutputDir_text->Text) + "/CSV";
	// make csv sub directory
	System::IO::Directory::CreateDirectory(subdir);
	// filepath to save to
	std::string fp = UI_ERROR::SYS2std_string(subdir + "/" + this->NameOfTest_Text->Text) +
		".csv";

	// title header string
	//std::string header = "Position;Velocity;NumberOfSatellites;LigamentLength;Volume";
	csv.newRow() << "Position" << "Velocity" << "NumberOfSatellites" << "LigamentLength" << "Volume";

	// loop through data and save to csv object
	System::String^ pb_str = "Writing to CSV";
	ProgressBarUpdate(pb_str, 0, 100, 0, true);
	for (int i = 0; i < this->GrayscaleImages->size(); i++)
	{
		pb_str = "CSV File: " + i + "/" + GrayscaleImages->size();
		if (this->Position_cbox->Checked) Pos = this->MainDropPosition->at(i);
		if (this->Velocity_cbox->Checked) Vel = this->MainDropVelocity->at(i);
		if (this->Satellites_cbox->Checked) NumSat = this->NumberOfSatellites->at(i);
		if (this->LigLength_cbox->Checked) LigLen = this->LigamentLength->at(i);
		if (this->DropVolume_cbox->Checked) Volume = this->Volume->at(i);

		csv.newRow() << Pos << Vel << NumSat << LigLen << Volume;
		ProgressBarUpdate(pb_str, 0, GrayscaleImages->size(), i, true);
		
	}
	pb_str = "Finished";
	ProgressBarUpdate(pb_str, 0, ColorImages->size(), ColorImages->size(), true);
	System::Threading::Thread::Sleep(1000);
	ProgressBarUpdate(pb_str, 0, ColorImages->size(), ColorImages->size(), false);
	// write to file
	csv.writeToFile(fp);
}



void DSCLR::DropSourceFrom::ProgressBarUpdate(System::String^ message, int min, int max, int level, bool visible)
{
	this->ProgressBar->Minimum = min;
	this->ProgressBar->Maximum = max;
	this->ProgressBar->Value = level;
	this->ProgressBar->Visible = visible;
	this->PB_Label->Text = message;
	this->PB_Label->Visible = visible;
	this->ProgressBar->Update();
	this->PB_Label->Update();
}

void DSCLR::DropSourceFrom::DrawDetectedAndPredictedCenters(bool enablePredic)
{
	System::String^ pb_str = "Detect and Predict";
	// format input and output directories
	std::string output_dir = UI_ERROR::SYS2std_string(this->OutputDir_text->Text);
	// Output string 
	std::string	fpOut = output_dir + std::string("/DetectPredic_");
	std::string fpFull;
	std::string exten = ".jpg";
	cv::Mat drawing;
	int ProgressVal = 0;
	int radius = 2;
	cv::Scalar red = cv::Scalar(0, 0, 255);
	cv::Scalar green = cv::Scalar(0, 255, 0);
	cv::Scalar yellow = cv::Scalar(0, 255, 255);
	bool success = true;

	ProgressBarUpdate(pb_str, 0, ColorImages->size(), ProgressVal, true);

	for (int i = 0; i < ColorImages->size(); i++)
	{
		pb_str = "Drawing Detect/Precidt: " + i + "/" + ColorImages->size();
		drawing = ColorImages->at(i).clone();
		
		if (MainDropPoints->at(i).y != -1)
		{
			// draw detected in red
			cv::circle(drawing, MainDropPoints->at(i), 2, red, 1);
			// draw predicted in green
			if (enablePredic)
			{
				cv::circle(drawing, MainDropPredic->at(i), 2, green, -1);
				//cv::circle(drawing, MainDropControl->at(i), 2, yellow, -1);
			}
			
			// Write image
			fpFull = fpOut + std::to_string(i + 1) + exten;
			//std::cout << "Saving " << fpFull << std::endl;

			if (!cv::imwrite(fpFull, drawing))
			{
				success = false;
				break;
			}

		}

		ProgressBarUpdate(pb_str, 0, ColorImages->size(), i, true);
	}

	pb_str = "Success = " + success;
	ProgressBarUpdate(pb_str, 0, ColorImages->size(), ProgressVal, true);
	System::Threading::Thread::Sleep(1000);

	ProgressBarUpdate(pb_str, 0, ColorImages->size(), ProgressVal, false);
	
}

void DSCLR::DropSourceFrom::DrawAllCentroids()
{
	std::string output_dir = UI_ERROR::SYS2std_string(this->OutputDir_text->Text);
	// Output string 
	std::string	fpOut = output_dir + std::string("/DrawContour_");
	std::string fpFull;
	std::string exten = ".jpg";
	cv::Mat bin;
	cv::Mat src;
	std::vector<cv::Point2f> Centers;
	System::String^ pb_str = "Drawing All Centers";
	ProgressBarUpdate(pb_str, 0, ColorImages->size(), 0, true);

	for (int i = 0; i < ColorImages->size(); i++)
	{
		pb_str = "Drawing All Centers: " + i + "/" + ColorImages->size();
		bin = ImageProcessing::BinaryThresh(GrayscaleImages->at(i));
		Centers = ImageProcessing::ImageCentroids(bin);
		src = ColorImages->at(i).clone();
		ImageProcessing::DrawCentroids(src, Centers);

		fpFull = fpOut + std::to_string(i + 1) + exten;
		//std::cout << "Saving " << fpFull << std::endl;

		if (!cv::imwrite(fpFull, src))
		{
			//success = false;
			break;
		}

		ProgressBarUpdate(pb_str, 0, ColorImages->size(), i, true);
	}

	pb_str = "Finished";
	ProgressBarUpdate(pb_str, 0, ColorImages->size(), ColorImages->size(), true);
	System::Threading::Thread::Sleep(1000);
	ProgressBarUpdate(pb_str, 0, ColorImages->size(), ColorImages->size(), false);
}

void DSCLR::DropSourceFrom::DrawBoundingRects()
{
	System::String^ pb_str = "Draw Bounding Rectangles";
	// format input and output directories
	std::string output_dir = UI_ERROR::SYS2std_string(this->OutputDir_text->Text);
	// Output string 
	std::string	fpOut = output_dir + std::string("/DrawBoundRects_");
	std::string fpFull;
	std::string exten = ".jpg";
	cv::Mat drawing;
	int ProgressVal = 0;
	int radius = 2;
	cv::Scalar red = cv::Scalar(0, 0, 255);
	cv::Scalar green = cv::Scalar(0, 255, 0);
	cv::Scalar yellow = cv::Scalar(0, 255, 255);
	bool success = true;
	

	std::vector<cv::Rect2f> BoundRects;

	ProgressBarUpdate(pb_str, 0, GrayscaleImages->size(), ProgressVal, true);

	for (int i = 0; i < GrayscaleImages->size(); i++)
	{
		pb_str = "Drawing Bounding Rect: " + i + "/" + GrayscaleImages->size();
		BoundRects = ImageProcessing::FindBoundingRects(GrayscaleImages->at(i));
		drawing = ImageProcessing::DrawBoundingRects(ColorImages->at(i), BoundRects);
		
		// Now save to output directory

		fpFull = fpOut + std::to_string(i + 1) + exten;
		//std::cout << "Saving " << fpFull << std::endl;

		if (!cv::imwrite(fpFull, drawing))
		{
			success = false;
			break;
		}

		ProgressBarUpdate(pb_str, 0, ColorImages->size(), i, true);
	}

	pb_str = "Success = " + success;
	ProgressBarUpdate(pb_str, 0, ColorImages->size(), ProgressVal, true);
	System::Threading::Thread::Sleep(1000);

	ProgressBarUpdate(pb_str, 0, ColorImages->size(), ProgressVal, false);
}

void DSCLR::DropSourceFrom::DrawExtBotPoints()
{
	System::String^ pb_str = "Draw Bounding Rectangles";
	// format input and output directories
	std::string output_dir = UI_ERROR::SYS2std_string(this->OutputDir_text->Text);
	// Output string 
	std::string	fpOut = output_dir + std::string("/DrawExtBotPoints_");
	std::string fpFull;
	std::string exten = ".jpg";
	cv::Mat drawing;
	int ProgressVal = 0;
	int radius = 2;
	cv::Scalar red = cv::Scalar(0, 0, 255);
	cv::Scalar green = cv::Scalar(0, 255, 0);
	cv::Scalar yellow = cv::Scalar(0, 255, 255);
	bool success = true;
	cv::Point2f ExtBot;


	ProgressBarUpdate(pb_str, 0, GrayscaleImages->size(), ProgressVal, true);

	for (int i = 0; i < this->GrayscaleImages->size(); i++)
	{
		pb_str = "Drawing Bounding Rect: " + i + "/" + GrayscaleImages->size();
		ExtBot = ImageProcessing::FindBottomMostPoint(this->GrayscaleImages->at(i));
		drawing = this->ColorImages->at(i).clone();

		// legit position
		if (this->MainDropPoints->at(i).y >= 0)
		{
			// bottom most point in green
			cv::circle(drawing, ExtBot, radius, green, 1);
			// main drop position in red
			cv::circle(drawing, this->MainDropPoints->at(i), radius, red, 1);

			// save to output
			fpFull = fpOut + std::to_string(i + 1) + exten;
			//std::cout << "Saving " << fpFull << std::endl;

			if (!cv::imwrite(fpFull, drawing))
			{
				success = false;
				break;
			}
		}
		
		ProgressBarUpdate(pb_str, 0, ColorImages->size(), i, true);
	}

	pb_str = "Success = " + success;
	ProgressBarUpdate(pb_str, 0, ColorImages->size(), ProgressVal, true);
	System::Threading::Thread::Sleep(1000);

	ProgressBarUpdate(pb_str, 0, ColorImages->size(), ProgressVal, false);
}

void DSCLR::DropSourceFrom::DrawMainDropMask()
{
	System::String^ pb_str = "Draw Main Drop Mask";
	// format input and output directories
	std::string output_dir = UI_ERROR::SYS2std_string(this->OutputDir_text->Text);
	// Output string 
	std::string	fpOut = output_dir + std::string("/DrawMainDropMask_");
	std::string fpFull;
	std::string exten = ".jpg";
	cv::Mat drawing;
	int ProgressVal = 0;
	int radius = 2;
	cv::Scalar red = cv::Scalar(0, 0, 255);
	cv::Scalar green = cv::Scalar(0, 255, 0);
	cv::Scalar yellow = cv::Scalar(0, 255, 255);
	bool success = true;
	cv::Point2f ExtBot;


	ProgressBarUpdate(pb_str, 0, GrayscaleImages->size(), ProgressVal, true);

	for (int i = 0; i < this->GrayscaleImages->size(); i++)
	{
		if (i > 66)
		{
			std::cout << "Stuff" << std::endl;
		}

		pb_str = "Drawing Main Drop Mask: " + i + "/" + GrayscaleImages->size();
		drawing = ImageProcessing::MainDropMask(this->GrayscaleImages->at(i));

		// save to output
		fpFull = fpOut + std::to_string(i + 1) + exten;
		//std::cout << "Saving " << fpFull << std::endl;
		
		

		if (!cv::imwrite(fpFull, drawing))
		{
			success = false;
			break;
		}

		ProgressBarUpdate(pb_str, 0, this->GrayscaleImages->size(), i, true);
	}

	pb_str = "Success = " + success;
	ProgressBarUpdate(pb_str, 0, ColorImages->size(), ProgressVal, true);
	System::Threading::Thread::Sleep(1000);

	ProgressBarUpdate(pb_str, 0, ColorImages->size(), ProgressVal, false);
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
		fpFull = fpOut + std::to_string(i) + exten;
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

	if (TEST_DRAW_DETECT_PREDIC)
	{
		TestDetectPredict();
	}

	if (TEST_DRAW_BOUND_RECT)
	{
		TestBoundingRect();
	}

	if (TEST_EXT_BOT_POINTS)
	{
		TestExtBotPoints();
	}

	if (TEST_DRAW_MAIN_DROP_MASK)
	{
		TestMainDropMask();
	}
}

void DSCLR::DropSourceFrom::TestDetectPredict()
{
	bool enablePredic = true;
	// Load grayscale and color images
	LoadGrayscaleImages();
	LoadColorImages();

	// update main droplet positions
	MainDropPositions();

	// Draw detected and predicted centers
	DrawDetectedAndPredictedCenters(enablePredic);
	//DrawAllCentroids();
}

void DSCLR::DropSourceFrom::TestBoundingRect()
{
	// load images first
	LoadGrayscaleImages();
	LoadColorImages();
	// draw rects
	DrawBoundingRects();
}

void DSCLR::DropSourceFrom::TestExtBotPoints()
{
	// load grayscale images
	LoadGrayscaleImages();
	// load colour images
	LoadColorImages();

	// do main drop points
	MainDropPositions();

	// draw ext bots
	DrawExtBotPoints();
}

void DSCLR::DropSourceFrom::TestMainDropMask()
{
	// load grayscale images
	LoadGrayscaleImages();

	// main drop mask imgs
	DrawMainDropMask();
	
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

