/*
*	@brief
*	This is the Windows Form Object that drives the GUI. It was created by the Visual Studio IDE.
*	Here, external functions will be used as callbacks for buttons, the user will input parameters
*	for every individual DOD analysis. The purposes of the external files are as follows:
*
*	ImageProc.h		->		Used to do image processing and droplet analysis
*	UI.h			->		Used to handle user inputs
*	Test_Macro.h	->		Used for debugging.
*
*	AUTHOR:		Akshin Goswami
*	DATE:		28/01/20
*	VERSION:	0.2.0
*
*
*	TODO:
*	1. Uncertainty Analysis in Time Vector
*	2. Uncertainty Analysis in Position Vector
*	3. Uncertainty Analysis in Velocity Vector
*	4. Uncertainty Analysis in Ligament Length Vector
*	5. Uncertainty Analysis in Volume Vector
*/

#pragma once

#include "ImageProc.h"
#include "UI.h"
#include "Test_Macro.h"
#include "pid.h"
#include "PID_Params.h"
#include "CSVWriter.h"
#include "libxl.h"
#include "ThresholdingMethodsDefs.h"
#include "UncertaintyAnalysis.h"


using namespace UI_ERROR;

namespace DSCLR {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	//using System::Windows::Forms::DialogResult;
	/// <summary>
	/// Summary for DropSourceFrom
	/// </summary>
	public ref class DropSourceFrom : public System::Windows::Forms::Form
	{
	public:
		DropSourceFrom(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			ImageWidth_Px = 0;
			ImageHeight_Px = 0;

			// thresh type
			
			if (this->GlobalThresh_RdBtn->Checked)
			{
				ThreshType = THRESH_GLOBAL;
			}
			else if (this->AdaptiveThresh_RdBtn->Checked)
			{
				ThreshType = THRESH_ADAPTIVE;
			}
			else
			{
				ThreshType = -1;
			}

			// Initialise images
			// Images
			GrayscaleImages = new std::vector<cv::Mat>;
			ColorImages = new std::vector<cv::Mat>;

			// Initialise centers
			MainDropPoints = new std::vector<cv::Point2f>();
			MainDropPredic = new std::vector<cv::Point2f>();
			MainDropControl = new std::vector<cv::Point2f>();

			// initialise pixel velocity
			Velocity_px = new std::vector<cv::Point2f>();

			// initialise lig length in pixels
			LigamentLength_px = new std::vector<float>;

			// Initialise dataset
			TimeVector = new std::vector<float>;
			MainDropPosition = new std::vector<float>;
			MainDropVelocity = new std::vector<float>;
			NumberOfSatellites = new std::vector<int>;
			LigamentLength = new std::vector<float>;
			Volume = new std::vector<float>;

			// initialise dataset for UA
			UA_TimeVector = new std::vector<float>;
			UA_MainDropPosition = new std::vector<float>;
			UA_MainDropVelocity = new std::vector<float>;
			UA_LigamentLength = new std::vector<float>;
			UA_Volume = new std::vector<float>;

			// Initial Numerical User Inputs
			CFR = -1;
			ROI_W = -1;
			ROI_H = -1;

			// initialise UA user inputs
			del_CFR = -1;		// uncertainty in camera frame rate
			del_ROI_W = -1;	// uncertainty in ROI width
			del_ROI_H = -1;	// uncertainty in ROI height
			del_rpx_rel = -1;

			// initialise UA calculated variables
			del_IMG_W = -1;
			del_IMG_H = -1;
			del_dx = -1;
			del_dy = -1;

		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~DropSourceFrom()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^ NameOfTest_label;
	protected:

	private: System::Windows::Forms::Label^ ROI_label;
	private: System::Windows::Forms::Label^ FPS_label;
	protected:


	private: System::Windows::Forms::Label^ Width;
	private: System::Windows::Forms::Label^ Height;
	private: System::Windows::Forms::Label^ InputDir_label;


	private: System::Windows::Forms::Label^ OutputDir_label;




	private: System::Windows::Forms::TextBox^ NameOfTest_Text;
	private: System::Windows::Forms::TextBox^ Width_text;
	private: System::Windows::Forms::TextBox^ Height_txt;
	private: System::Windows::Forms::TextBox^ FPS_text;
	private: System::Windows::Forms::TextBox^ InputDir_text;
	private: System::Windows::Forms::TextBox^ OutputDir_text;
	private: System::Windows::Forms::Button^ InputDir_button;
	private: System::Windows::Forms::Button^ OutputDir_button;
	private: System::Windows::Forms::Button^ StartAnalysis_button;
	private: System::Windows::Forms::OpenFileDialog^ openFileDialog1;
	private: System::Windows::Forms::FolderBrowserDialog^ folderBrowserDialog1;
	private: System::Windows::Forms::FolderBrowserDialog^ folderBrowserDialog2;
	private: System::Windows::Forms::Label^ Error_NoT;
	private: System::Windows::Forms::Label^ Error_ROI;
	private: System::Windows::Forms::Label^ Error_FPS;
	private: System::Windows::Forms::Label^ Error_Input;
	private: System::Windows::Forms::Label^ Error_Output;
	private: System::Windows::Forms::Label^ PB_Label;

	private: System::Windows::Forms::ProgressBar^ ProgressBar;

		   // Additional private members
		   // Used for Data Handling
	private:
		float ImageWidth_Px;
		float ImageHeight_Px;

		// Thresholding type
		int ThreshType;

		// Images
		std::vector<cv::Mat>* GrayscaleImages;
		std::vector<cv::Mat>* ColorImages;

		// Points on image
		std::vector<cv::Point2f>* MainDropPoints;
		std::vector<cv::Point2f>* MainDropControl;
		std::vector<cv::Point2f>* MainDropPredic;

		// Velocity in terms of pixel position
		std::vector<cv::Point2f>* Velocity_px;

		// ligament length in pixels
		std::vector<float>* LigamentLength_px;

		// Data Sets
		std::vector<float>* TimeVector;
		std::vector<float>* MainDropPosition;
		std::vector<float>* MainDropVelocity;
		std::vector<int>* NumberOfSatellites;
		std::vector<float>* LigamentLength;
		std::vector<float>* Volume;

		// Data Sets for Uncertainty Analysis
		std::vector<float>* UA_TimeVector;
		std::vector<float>* UA_MainDropPosition;
		std::vector<float>* UA_MainDropVelocity;
		std::vector<float>* UA_LigamentLength;
		std::vector<float>* UA_Volume;

		// Numerical User Inputs
		double CFR;		// Camera Frame Rate (Hz)
		double ROI_W;	// Width of Region of Interest (mm)
		double ROI_H;	// Height of Region of Interest (mm)

		// Uncertainty Analysis User Inputs
		double del_CFR;		// uncertainty in camera frame rate
		double del_ROI_W;	// uncertainty in ROI width
		double del_ROI_H;	// uncertainty in ROI height
		double del_rpx_rel;	// relative uncertainty in OpenCV Calculations

		// Uncertainty Analysis Calculated Variables
		double del_IMG_W;	// Uncertainty in image width/height in px
		double del_IMG_H;
		double del_dx;		// uncertainty in width/height conversion factor (mm/px)
		double del_dy;		

	private: System::Windows::Forms::Label^ Parameters_lbl;
	private: System::Windows::Forms::CheckBox^ Position_cbox;
	private: System::Windows::Forms::CheckBox^ Velocity_cbox;
	private: System::Windows::Forms::CheckBox^ Satellites_cbox;
	private: System::Windows::Forms::CheckBox^ LigLength_cbox;
	private: System::Windows::Forms::CheckBox^ DropVolume_cbox;
	private: System::Windows::Forms::Label^ OutputFiles_label;
	private: System::Windows::Forms::CheckBox^ CSV_cbox;
	private: System::Windows::Forms::CheckBox^ DebugImg_cbox;
	private: System::Windows::Forms::Label^ Error_OutputFile;
	private: System::Windows::Forms::Label^ Error_SelectParam;
	private: System::Windows::Forms::CheckBox^ XLSX_cbox;
	private: System::Windows::Forms::Label^ TestInfo_lbl;
	private: System::Windows::Forms::Label^ FileIO_lbl;
	private: System::Windows::Forms::Label^ DropAnalysis_lbl;
	private: System::Windows::Forms::Label^ ImageProcessing_lbl;

	private: System::Windows::Forms::GroupBox^ ThreshGroupBox;
	private: System::Windows::Forms::RadioButton^ AdaptiveThresh_RdBtn;
	private: System::Windows::Forms::RadioButton^ GlobalThresh_RdBtn;
private: System::Windows::Forms::CheckBox^ ImgSubEn_cbox;


private: System::Windows::Forms::Label^ label1;
private: System::Windows::Forms::Label^ UA_CFR_lbl;
private: System::Windows::Forms::CheckBox^ UA_Enable_cbox;
private: System::Windows::Forms::TextBox^ UA_CFR_txt;
private: System::Windows::Forms::Label^ label2;
private: System::Windows::Forms::TextBox^ UA_ROI_Width_txt;
private: System::Windows::Forms::TextBox^ UA_ROI_Height_txt;
private: System::Windows::Forms::Label^ UA_ROI_Width_lbl;
private: System::Windows::Forms::Label^ UA_ROI_Height_lbl;
private: System::Windows::Forms::Label^ Error_UA;


	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(DropSourceFrom::typeid));
			this->NameOfTest_label = (gcnew System::Windows::Forms::Label());
			this->ROI_label = (gcnew System::Windows::Forms::Label());
			this->FPS_label = (gcnew System::Windows::Forms::Label());
			this->Width = (gcnew System::Windows::Forms::Label());
			this->Height = (gcnew System::Windows::Forms::Label());
			this->InputDir_label = (gcnew System::Windows::Forms::Label());
			this->OutputDir_label = (gcnew System::Windows::Forms::Label());
			this->NameOfTest_Text = (gcnew System::Windows::Forms::TextBox());
			this->Width_text = (gcnew System::Windows::Forms::TextBox());
			this->Height_txt = (gcnew System::Windows::Forms::TextBox());
			this->FPS_text = (gcnew System::Windows::Forms::TextBox());
			this->InputDir_text = (gcnew System::Windows::Forms::TextBox());
			this->OutputDir_text = (gcnew System::Windows::Forms::TextBox());
			this->InputDir_button = (gcnew System::Windows::Forms::Button());
			this->OutputDir_button = (gcnew System::Windows::Forms::Button());
			this->StartAnalysis_button = (gcnew System::Windows::Forms::Button());
			this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->folderBrowserDialog1 = (gcnew System::Windows::Forms::FolderBrowserDialog());
			this->folderBrowserDialog2 = (gcnew System::Windows::Forms::FolderBrowserDialog());
			this->Error_NoT = (gcnew System::Windows::Forms::Label());
			this->Error_ROI = (gcnew System::Windows::Forms::Label());
			this->Error_FPS = (gcnew System::Windows::Forms::Label());
			this->Error_Input = (gcnew System::Windows::Forms::Label());
			this->Error_Output = (gcnew System::Windows::Forms::Label());
			this->PB_Label = (gcnew System::Windows::Forms::Label());
			this->ProgressBar = (gcnew System::Windows::Forms::ProgressBar());
			this->Parameters_lbl = (gcnew System::Windows::Forms::Label());
			this->Position_cbox = (gcnew System::Windows::Forms::CheckBox());
			this->Velocity_cbox = (gcnew System::Windows::Forms::CheckBox());
			this->Satellites_cbox = (gcnew System::Windows::Forms::CheckBox());
			this->LigLength_cbox = (gcnew System::Windows::Forms::CheckBox());
			this->DropVolume_cbox = (gcnew System::Windows::Forms::CheckBox());
			this->OutputFiles_label = (gcnew System::Windows::Forms::Label());
			this->CSV_cbox = (gcnew System::Windows::Forms::CheckBox());
			this->DebugImg_cbox = (gcnew System::Windows::Forms::CheckBox());
			this->Error_OutputFile = (gcnew System::Windows::Forms::Label());
			this->Error_SelectParam = (gcnew System::Windows::Forms::Label());
			this->XLSX_cbox = (gcnew System::Windows::Forms::CheckBox());
			this->TestInfo_lbl = (gcnew System::Windows::Forms::Label());
			this->FileIO_lbl = (gcnew System::Windows::Forms::Label());
			this->DropAnalysis_lbl = (gcnew System::Windows::Forms::Label());
			this->ImageProcessing_lbl = (gcnew System::Windows::Forms::Label());
			this->ThreshGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->AdaptiveThresh_RdBtn = (gcnew System::Windows::Forms::RadioButton());
			this->GlobalThresh_RdBtn = (gcnew System::Windows::Forms::RadioButton());
			this->ImgSubEn_cbox = (gcnew System::Windows::Forms::CheckBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->UA_CFR_lbl = (gcnew System::Windows::Forms::Label());
			this->UA_Enable_cbox = (gcnew System::Windows::Forms::CheckBox());
			this->UA_CFR_txt = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->UA_ROI_Width_txt = (gcnew System::Windows::Forms::TextBox());
			this->UA_ROI_Height_txt = (gcnew System::Windows::Forms::TextBox());
			this->UA_ROI_Width_lbl = (gcnew System::Windows::Forms::Label());
			this->UA_ROI_Height_lbl = (gcnew System::Windows::Forms::Label());
			this->Error_UA = (gcnew System::Windows::Forms::Label());
			this->ThreshGroupBox->SuspendLayout();
			this->SuspendLayout();
			// 
			// NameOfTest_label
			// 
			this->NameOfTest_label->AutoSize = true;
			this->NameOfTest_label->Font = (gcnew System::Drawing::Font(L"Arial", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->NameOfTest_label->Location = System::Drawing::Point(19, 56);
			this->NameOfTest_label->Name = L"NameOfTest_label";
			this->NameOfTest_label->Size = System::Drawing::Size(148, 27);
			this->NameOfTest_label->TabIndex = 0;
			this->NameOfTest_label->Text = L"Name of test";
			// 
			// ROI_label
			// 
			this->ROI_label->AutoSize = true;
			this->ROI_label->Font = (gcnew System::Drawing::Font(L"Arial", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ROI_label->Location = System::Drawing::Point(19, 93);
			this->ROI_label->Name = L"ROI_label";
			this->ROI_label->Size = System::Drawing::Size(117, 27);
			this->ROI_label->TabIndex = 1;
			this->ROI_label->Text = L"ROI (mm)";
			this->ROI_label->Click += gcnew System::EventHandler(this, &DropSourceFrom::ROIDimensions_Click);
			// 
			// FPS_label
			// 
			this->FPS_label->AutoSize = true;
			this->FPS_label->Font = (gcnew System::Drawing::Font(L"Arial", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->FPS_label->Location = System::Drawing::Point(19, 146);
			this->FPS_label->Name = L"FPS_label";
			this->FPS_label->Size = System::Drawing::Size(190, 27);
			this->FPS_label->TabIndex = 2;
			this->FPS_label->Text = L"Frame Rate (Hz)";
			// 
			// Width
			// 
			this->Width->AutoSize = true;
			this->Width->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Width->Location = System::Drawing::Point(247, 95);
			this->Width->Name = L"Width";
			this->Width->Size = System::Drawing::Size(68, 29);
			this->Width->TabIndex = 3;
			this->Width->Text = L"Width:";
			this->Width->Click += gcnew System::EventHandler(this, &DropSourceFrom::label1_Click);
			// 
			// Height
			// 
			this->Height->AutoSize = true;
			this->Height->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Height->Location = System::Drawing::Point(485, 95);
			this->Height->Name = L"Height";
			this->Height->Size = System::Drawing::Size(74, 29);
			this->Height->TabIndex = 4;
			this->Height->Text = L"Height:";
			this->Height->Click += gcnew System::EventHandler(this, &DropSourceFrom::label1_Click_1);
			// 
			// InputDir_label
			// 
			this->InputDir_label->AutoSize = true;
			this->InputDir_label->Font = (gcnew System::Drawing::Font(L"Arial", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->InputDir_label->Location = System::Drawing::Point(19, 339);
			this->InputDir_label->Name = L"InputDir_label";
			this->InputDir_label->Size = System::Drawing::Size(169, 27);
			this->InputDir_label->TabIndex = 5;
			this->InputDir_label->Text = L"Input Directory";
			// 
			// OutputDir_label
			// 
			this->OutputDir_label->AutoSize = true;
			this->OutputDir_label->Font = (gcnew System::Drawing::Font(L"Arial", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->OutputDir_label->Location = System::Drawing::Point(19, 417);
			this->OutputDir_label->Name = L"OutputDir_label";
			this->OutputDir_label->Size = System::Drawing::Size(189, 27);
			this->OutputDir_label->TabIndex = 7;
			this->OutputDir_label->Text = L"Output Directory";
			this->OutputDir_label->Click += gcnew System::EventHandler(this, &DropSourceFrom::label2_Click_1);
			// 
			// NameOfTest_Text
			// 
			this->NameOfTest_Text->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 20.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->NameOfTest_Text->Location = System::Drawing::Point(238, 49);
			this->NameOfTest_Text->Name = L"NameOfTest_Text";
			this->NameOfTest_Text->Size = System::Drawing::Size(441, 38);
			this->NameOfTest_Text->TabIndex = 8;
			this->NameOfTest_Text->TextChanged += gcnew System::EventHandler(this, &DropSourceFrom::textBox1_TextChanged);
			// 
			// Width_text
			// 
			this->Width_text->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 20.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Width_text->Location = System::Drawing::Point(321, 93);
			this->Width_text->Name = L"Width_text";
			this->Width_text->Size = System::Drawing::Size(114, 38);
			this->Width_text->TabIndex = 9;
			// 
			// Height_txt
			// 
			this->Height_txt->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 20.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Height_txt->Location = System::Drawing::Point(565, 95);
			this->Height_txt->Name = L"Height_txt";
			this->Height_txt->Size = System::Drawing::Size(114, 38);
			this->Height_txt->TabIndex = 10;
			// 
			// FPS_text
			// 
			this->FPS_text->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 20.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->FPS_text->Location = System::Drawing::Point(238, 146);
			this->FPS_text->Name = L"FPS_text";
			this->FPS_text->Size = System::Drawing::Size(197, 38);
			this->FPS_text->TabIndex = 11;
			// 
			// InputDir_text
			// 
			this->InputDir_text->Enabled = false;
			this->InputDir_text->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 20.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->InputDir_text->Location = System::Drawing::Point(238, 339);
			this->InputDir_text->Name = L"InputDir_text";
			this->InputDir_text->Size = System::Drawing::Size(441, 38);
			this->InputDir_text->TabIndex = 12;
			// 
			// OutputDir_text
			// 
			this->OutputDir_text->Enabled = false;
			this->OutputDir_text->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 20.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->OutputDir_text->Location = System::Drawing::Point(238, 417);
			this->OutputDir_text->Name = L"OutputDir_text";
			this->OutputDir_text->Size = System::Drawing::Size(441, 38);
			this->OutputDir_text->TabIndex = 13;
			// 
			// InputDir_button
			// 
			this->InputDir_button->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->InputDir_button->Location = System::Drawing::Point(24, 371);
			this->InputDir_button->Name = L"InputDir_button";
			this->InputDir_button->Size = System::Drawing::Size(179, 39);
			this->InputDir_button->TabIndex = 14;
			this->InputDir_button->Text = L"Select Input Directory";
			this->InputDir_button->UseVisualStyleBackColor = true;
			this->InputDir_button->Click += gcnew System::EventHandler(this, &DropSourceFrom::InputDir_button_Click);
			// 
			// OutputDir_button
			// 
			this->OutputDir_button->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->OutputDir_button->Location = System::Drawing::Point(24, 449);
			this->OutputDir_button->Name = L"OutputDir_button";
			this->OutputDir_button->Size = System::Drawing::Size(179, 39);
			this->OutputDir_button->TabIndex = 15;
			this->OutputDir_button->Text = L"Select Output Directory";
			this->OutputDir_button->UseVisualStyleBackColor = true;
			this->OutputDir_button->Click += gcnew System::EventHandler(this, &DropSourceFrom::button1_Click_1);
			// 
			// StartAnalysis_button
			// 
			this->StartAnalysis_button->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(192)), static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->StartAnalysis_button->Font = (gcnew System::Drawing::Font(L"Arial", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->StartAnalysis_button->ForeColor = System::Drawing::SystemColors::Control;
			this->StartAnalysis_button->Location = System::Drawing::Point(22, 788);
			this->StartAnalysis_button->Name = L"StartAnalysis_button";
			this->StartAnalysis_button->Size = System::Drawing::Size(179, 52);
			this->StartAnalysis_button->TabIndex = 16;
			this->StartAnalysis_button->Text = L"Start Analysis";
			this->StartAnalysis_button->UseVisualStyleBackColor = false;
			this->StartAnalysis_button->Click += gcnew System::EventHandler(this, &DropSourceFrom::StartAnalysis_button_Click);
			// 
			// openFileDialog1
			// 
			this->openFileDialog1->FileName = L"openFileDialog1";
			this->openFileDialog1->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &DropSourceFrom::openFileDialog1_FileOk);
			// 
			// folderBrowserDialog1
			// 
			this->folderBrowserDialog1->HelpRequest += gcnew System::EventHandler(this, &DropSourceFrom::folderBrowserDialog1_HelpRequest);
			// 
			// Error_NoT
			// 
			this->Error_NoT->AutoSize = true;
			this->Error_NoT->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Error_NoT->ForeColor = System::Drawing::Color::Red;
			this->Error_NoT->Location = System::Drawing::Point(685, 58);
			this->Error_NoT->Name = L"Error_NoT";
			this->Error_NoT->Size = System::Drawing::Size(205, 29);
			this->Error_NoT->TabIndex = 18;
			this->Error_NoT->Text = L"Please Name the Test";
			this->Error_NoT->Visible = false;
			// 
			// Error_ROI
			// 
			this->Error_ROI->AutoSize = true;
			this->Error_ROI->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Error_ROI->ForeColor = System::Drawing::Color::Red;
			this->Error_ROI->Location = System::Drawing::Point(685, 104);
			this->Error_ROI->Name = L"Error_ROI";
			this->Error_ROI->Size = System::Drawing::Size(194, 29);
			this->Error_ROI->TabIndex = 19;
			this->Error_ROI->Text = L"Incorrect ROI Format";
			this->Error_ROI->Visible = false;
			// 
			// Error_FPS
			// 
			this->Error_FPS->AutoSize = true;
			this->Error_FPS->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Error_FPS->ForeColor = System::Drawing::Color::Red;
			this->Error_FPS->Location = System::Drawing::Point(685, 146);
			this->Error_FPS->Name = L"Error_FPS";
			this->Error_FPS->Size = System::Drawing::Size(199, 29);
			this->Error_FPS->TabIndex = 20;
			this->Error_FPS->Text = L"Incorrect FPS Format";
			this->Error_FPS->Visible = false;
			// 
			// Error_Input
			// 
			this->Error_Input->AutoSize = true;
			this->Error_Input->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Error_Input->ForeColor = System::Drawing::Color::Red;
			this->Error_Input->Location = System::Drawing::Point(685, 344);
			this->Error_Input->Name = L"Error_Input";
			this->Error_Input->Size = System::Drawing::Size(171, 29);
			this->Error_Input->TabIndex = 21;
			this->Error_Input->Text = L"Input Not Selected";
			this->Error_Input->Visible = false;
			// 
			// Error_Output
			// 
			this->Error_Output->AutoSize = true;
			this->Error_Output->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Error_Output->ForeColor = System::Drawing::Color::Red;
			this->Error_Output->Location = System::Drawing::Point(685, 422);
			this->Error_Output->Name = L"Error_Output";
			this->Error_Output->Size = System::Drawing::Size(187, 29);
			this->Error_Output->TabIndex = 22;
			this->Error_Output->Text = L"Output Not Selected";
			this->Error_Output->Visible = false;
			// 
			// PB_Label
			// 
			this->PB_Label->AutoSize = true;
			this->PB_Label->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->PB_Label->ForeColor = System::Drawing::Color::Blue;
			this->PB_Label->Location = System::Drawing::Point(231, 779);
			this->PB_Label->Name = L"PB_Label";
			this->PB_Label->Size = System::Drawing::Size(121, 29);
			this->PB_Label->TabIndex = 23;
			this->PB_Label->Text = L"Error In Test";
			this->PB_Label->Visible = false;
			this->PB_Label->Click += gcnew System::EventHandler(this, &DropSourceFrom::PB_Label_Click);
			// 
			// ProgressBar
			// 
			this->ProgressBar->ForeColor = System::Drawing::Color::Lime;
			this->ProgressBar->Location = System::Drawing::Point(236, 811);
			this->ProgressBar->Name = L"ProgressBar";
			this->ProgressBar->Size = System::Drawing::Size(441, 29);
			this->ProgressBar->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
			this->ProgressBar->TabIndex = 24;
			this->ProgressBar->Visible = false;
			// 
			// Parameters_lbl
			// 
			this->Parameters_lbl->AutoSize = true;
			this->Parameters_lbl->Font = (gcnew System::Drawing::Font(L"Arial", 24, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Parameters_lbl->Location = System::Drawing::Point(17, 527);
			this->Parameters_lbl->Name = L"Parameters_lbl";
			this->Parameters_lbl->Size = System::Drawing::Size(373, 37);
			this->Parameters_lbl->TabIndex = 25;
			this->Parameters_lbl->Text = L"Processing Parameters";
			// 
			// Position_cbox
			// 
			this->Position_cbox->AutoSize = true;
			this->Position_cbox->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Position_cbox->Location = System::Drawing::Point(237, 697);
			this->Position_cbox->Name = L"Position_cbox";
			this->Position_cbox->Size = System::Drawing::Size(94, 29);
			this->Position_cbox->TabIndex = 27;
			this->Position_cbox->Text = L"Position";
			this->Position_cbox->UseVisualStyleBackColor = true;
			this->Position_cbox->CheckedChanged += gcnew System::EventHandler(this, &DropSourceFrom::Position_cbox_CheckedChanged);
			// 
			// Velocity_cbox
			// 
			this->Velocity_cbox->AutoSize = true;
			this->Velocity_cbox->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Velocity_cbox->Location = System::Drawing::Point(354, 697);
			this->Velocity_cbox->Name = L"Velocity_cbox";
			this->Velocity_cbox->Size = System::Drawing::Size(93, 29);
			this->Velocity_cbox->TabIndex = 28;
			this->Velocity_cbox->Text = L"Velocity";
			this->Velocity_cbox->UseVisualStyleBackColor = true;
			this->Velocity_cbox->CheckedChanged += gcnew System::EventHandler(this, &DropSourceFrom::Velocity_cbox_CheckedChanged);
			// 
			// Satellites_cbox
			// 
			this->Satellites_cbox->AutoSize = true;
			this->Satellites_cbox->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Satellites_cbox->Location = System::Drawing::Point(474, 697);
			this->Satellites_cbox->Name = L"Satellites_cbox";
			this->Satellites_cbox->Size = System::Drawing::Size(107, 29);
			this->Satellites_cbox->TabIndex = 29;
			this->Satellites_cbox->Text = L"Satelllites";
			this->Satellites_cbox->UseVisualStyleBackColor = true;
			this->Satellites_cbox->CheckedChanged += gcnew System::EventHandler(this, &DropSourceFrom::Satellites_cbox_CheckedChanged);
			// 
			// LigLength_cbox
			// 
			this->LigLength_cbox->AutoSize = true;
			this->LigLength_cbox->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->LigLength_cbox->Location = System::Drawing::Point(236, 747);
			this->LigLength_cbox->Name = L"LigLength_cbox";
			this->LigLength_cbox->Size = System::Drawing::Size(164, 29);
			this->LigLength_cbox->TabIndex = 30;
			this->LigLength_cbox->Text = L"Ligament Length";
			this->LigLength_cbox->UseVisualStyleBackColor = true;
			this->LigLength_cbox->CheckedChanged += gcnew System::EventHandler(this, &DropSourceFrom::LigLength_cbox_CheckedChanged);
			// 
			// DropVolume_cbox
			// 
			this->DropVolume_cbox->AutoSize = true;
			this->DropVolume_cbox->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->DropVolume_cbox->Location = System::Drawing::Point(421, 747);
			this->DropVolume_cbox->Name = L"DropVolume_cbox";
			this->DropVolume_cbox->Size = System::Drawing::Size(152, 29);
			this->DropVolume_cbox->TabIndex = 31;
			this->DropVolume_cbox->Text = L"Droplet Volume";
			this->DropVolume_cbox->UseVisualStyleBackColor = true;
			this->DropVolume_cbox->CheckedChanged += gcnew System::EventHandler(this, &DropSourceFrom::DropVolume_cbox_CheckedChanged);
			// 
			// OutputFiles_label
			// 
			this->OutputFiles_label->AutoSize = true;
			this->OutputFiles_label->Font = (gcnew System::Drawing::Font(L"Arial", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->OutputFiles_label->Location = System::Drawing::Point(25, 495);
			this->OutputFiles_label->Name = L"OutputFiles_label";
			this->OutputFiles_label->Size = System::Drawing::Size(145, 27);
			this->OutputFiles_label->TabIndex = 32;
			this->OutputFiles_label->Text = L"Output Files";
			// 
			// CSV_cbox
			// 
			this->CSV_cbox->AutoSize = true;
			this->CSV_cbox->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->CSV_cbox->Location = System::Drawing::Point(478, 497);
			this->CSV_cbox->Name = L"CSV_cbox";
			this->CSV_cbox->Size = System::Drawing::Size(99, 29);
			this->CSV_cbox->TabIndex = 33;
			this->CSV_cbox->Text = L"CSV File";
			this->CSV_cbox->UseVisualStyleBackColor = true;
			this->CSV_cbox->Visible = false;
			// 
			// DebugImg_cbox
			// 
			this->DebugImg_cbox->AutoSize = true;
			this->DebugImg_cbox->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->DebugImg_cbox->Location = System::Drawing::Point(324, 495);
			this->DebugImg_cbox->Name = L"DebugImg_cbox";
			this->DebugImg_cbox->Size = System::Drawing::Size(145, 29);
			this->DebugImg_cbox->TabIndex = 34;
			this->DebugImg_cbox->Text = L"Debug Images";
			this->DebugImg_cbox->UseVisualStyleBackColor = true;
			// 
			// Error_OutputFile
			// 
			this->Error_OutputFile->AutoSize = true;
			this->Error_OutputFile->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Error_OutputFile->ForeColor = System::Drawing::Color::Red;
			this->Error_OutputFile->Location = System::Drawing::Point(688, 495);
			this->Error_OutputFile->Name = L"Error_OutputFile";
			this->Error_OutputFile->Size = System::Drawing::Size(166, 29);
			this->Error_OutputFile->TabIndex = 35;
			this->Error_OutputFile->Text = L"Select Output File";
			this->Error_OutputFile->Visible = false;
			// 
			// Error_SelectParam
			// 
			this->Error_SelectParam->AutoSize = true;
			this->Error_SelectParam->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Error_SelectParam->ForeColor = System::Drawing::Color::Red;
			this->Error_SelectParam->Location = System::Drawing::Point(681, 699);
			this->Error_SelectParam->Name = L"Error_SelectParam";
			this->Error_SelectParam->Size = System::Drawing::Size(147, 29);
			this->Error_SelectParam->TabIndex = 36;
			this->Error_SelectParam->Text = L"Select Atleast 1";
			this->Error_SelectParam->Visible = false;
			// 
			// XLSX_cbox
			// 
			this->XLSX_cbox->AutoSize = true;
			this->XLSX_cbox->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->XLSX_cbox->Location = System::Drawing::Point(241, 495);
			this->XLSX_cbox->Name = L"XLSX_cbox";
			this->XLSX_cbox->Size = System::Drawing::Size(60, 29);
			this->XLSX_cbox->TabIndex = 37;
			this->XLSX_cbox->Text = L"xlsx";
			this->XLSX_cbox->UseVisualStyleBackColor = true;
			// 
			// TestInfo_lbl
			// 
			this->TestInfo_lbl->AutoSize = true;
			this->TestInfo_lbl->Font = (gcnew System::Drawing::Font(L"Arial", 24, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->TestInfo_lbl->Location = System::Drawing::Point(12, 9);
			this->TestInfo_lbl->Name = L"TestInfo_lbl";
			this->TestInfo_lbl->Size = System::Drawing::Size(266, 37);
			this->TestInfo_lbl->TabIndex = 38;
			this->TestInfo_lbl->Text = L"Test Information";
			this->TestInfo_lbl->UseMnemonic = false;
			// 
			// FileIO_lbl
			// 
			this->FileIO_lbl->AutoSize = true;
			this->FileIO_lbl->Font = (gcnew System::Drawing::Font(L"Arial", 24, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->FileIO_lbl->Location = System::Drawing::Point(17, 299);
			this->FileIO_lbl->Name = L"FileIO_lbl";
			this->FileIO_lbl->Size = System::Drawing::Size(272, 37);
			this->FileIO_lbl->TabIndex = 39;
			this->FileIO_lbl->Text = L"File Input/Output";
			this->FileIO_lbl->UseMnemonic = false;
			// 
			// DropAnalysis_lbl
			// 
			this->DropAnalysis_lbl->AutoSize = true;
			this->DropAnalysis_lbl->Font = (gcnew System::Drawing::Font(L"Arial", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->DropAnalysis_lbl->Location = System::Drawing::Point(21, 699);
			this->DropAnalysis_lbl->Name = L"DropAnalysis_lbl";
			this->DropAnalysis_lbl->Size = System::Drawing::Size(185, 27);
			this->DropAnalysis_lbl->TabIndex = 40;
			this->DropAnalysis_lbl->Text = L"Droplet Analysis";
			// 
			// ImageProcessing_lbl
			// 
			this->ImageProcessing_lbl->AutoSize = true;
			this->ImageProcessing_lbl->Font = (gcnew System::Drawing::Font(L"Arial", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ImageProcessing_lbl->Location = System::Drawing::Point(14, 582);
			this->ImageProcessing_lbl->Name = L"ImageProcessing_lbl";
			this->ImageProcessing_lbl->Size = System::Drawing::Size(161, 27);
			this->ImageProcessing_lbl->TabIndex = 41;
			this->ImageProcessing_lbl->Text = L"Segmentation";
			// 
			// ThreshGroupBox
			// 
			this->ThreshGroupBox->Controls->Add(this->AdaptiveThresh_RdBtn);
			this->ThreshGroupBox->Controls->Add(this->GlobalThresh_RdBtn);
			this->ThreshGroupBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ThreshGroupBox->Location = System::Drawing::Point(238, 582);
			this->ThreshGroupBox->Name = L"ThreshGroupBox";
			this->ThreshGroupBox->Size = System::Drawing::Size(190, 109);
			this->ThreshGroupBox->TabIndex = 42;
			this->ThreshGroupBox->TabStop = false;
			this->ThreshGroupBox->Text = L"Binary Thresholding Method";
			// 
			// AdaptiveThresh_RdBtn
			// 
			this->AdaptiveThresh_RdBtn->AutoSize = true;
			this->AdaptiveThresh_RdBtn->Location = System::Drawing::Point(6, 69);
			this->AdaptiveThresh_RdBtn->Name = L"AdaptiveThresh_RdBtn";
			this->AdaptiveThresh_RdBtn->Size = System::Drawing::Size(144, 20);
			this->AdaptiveThresh_RdBtn->TabIndex = 1;
			this->AdaptiveThresh_RdBtn->Text = L"Adaptive Threshold";
			this->AdaptiveThresh_RdBtn->TextImageRelation = System::Windows::Forms::TextImageRelation::ImageAboveText;
			this->AdaptiveThresh_RdBtn->UseVisualStyleBackColor = true;
			this->AdaptiveThresh_RdBtn->CheckedChanged += gcnew System::EventHandler(this, &DropSourceFrom::AdaptiveThresh_RdBtn_CheckedChanged);
			// 
			// GlobalThresh_RdBtn
			// 
			this->GlobalThresh_RdBtn->AutoSize = true;
			this->GlobalThresh_RdBtn->Checked = true;
			this->GlobalThresh_RdBtn->Location = System::Drawing::Point(6, 33);
			this->GlobalThresh_RdBtn->Name = L"GlobalThresh_RdBtn";
			this->GlobalThresh_RdBtn->Size = System::Drawing::Size(148, 20);
			this->GlobalThresh_RdBtn->TabIndex = 0;
			this->GlobalThresh_RdBtn->TabStop = true;
			this->GlobalThresh_RdBtn->Text = L"Global Thresholding";
			this->GlobalThresh_RdBtn->UseVisualStyleBackColor = true;
			this->GlobalThresh_RdBtn->CheckedChanged += gcnew System::EventHandler(this, &DropSourceFrom::GlobalThresh_RdBtn_CheckedChanged);
			// 
			// ImgSubEn_cbox
			// 
			this->ImgSubEn_cbox->AutoSize = true;
			this->ImgSubEn_cbox->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ImgSubEn_cbox->Location = System::Drawing::Point(475, 609);
			this->ImgSubEn_cbox->Name = L"ImgSubEn_cbox";
			this->ImgSubEn_cbox->Size = System::Drawing::Size(234, 29);
			this->ImgSubEn_cbox->TabIndex = 43;
			this->ImgSubEn_cbox->Text = L"Enable Image Subtraction";
			this->ImgSubEn_cbox->UseVisualStyleBackColor = true;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Arial", 24, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(17, 187);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(476, 37);
			this->label1->TabIndex = 46;
			this->label1->Text = L"Uncertainties in Measurement";
			this->label1->UseMnemonic = false;
			// 
			// UA_CFR_lbl
			// 
			this->UA_CFR_lbl->AutoSize = true;
			this->UA_CFR_lbl->Font = (gcnew System::Drawing::Font(L"Arial", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->UA_CFR_lbl->Location = System::Drawing::Point(27, 265);
			this->UA_CFR_lbl->Name = L"UA_CFR_lbl";
			this->UA_CFR_lbl->Size = System::Drawing::Size(190, 27);
			this->UA_CFR_lbl->TabIndex = 47;
			this->UA_CFR_lbl->Text = L"Frame Rate (Hz)";
			// 
			// UA_Enable_cbox
			// 
			this->UA_Enable_cbox->AutoSize = true;
			this->UA_Enable_cbox->Font = (gcnew System::Drawing::Font(L"Arial", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->UA_Enable_cbox->Location = System::Drawing::Point(30, 227);
			this->UA_Enable_cbox->Name = L"UA_Enable_cbox";
			this->UA_Enable_cbox->Size = System::Drawing::Size(288, 28);
			this->UA_Enable_cbox->TabIndex = 48;
			this->UA_Enable_cbox->Text = L"Enable Uncertainty Analysis";
			this->UA_Enable_cbox->UseVisualStyleBackColor = true;
			this->UA_Enable_cbox->CheckedChanged += gcnew System::EventHandler(this, &DropSourceFrom::UA_Enable_cbox_CheckedChanged);
			// 
			// UA_CFR_txt
			// 
			this->UA_CFR_txt->Enabled = false;
			this->UA_CFR_txt->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 20.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->UA_CFR_txt->Location = System::Drawing::Point(217, 258);
			this->UA_CFR_txt->Name = L"UA_CFR_txt";
			this->UA_CFR_txt->Size = System::Drawing::Size(72, 38);
			this->UA_CFR_txt->TabIndex = 49;
			this->UA_CFR_txt->TextChanged += gcnew System::EventHandler(this, &DropSourceFrom::UA_CFR_txt_TextChanged);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Arial", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label2->Location = System::Drawing::Point(295, 265);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(117, 27);
			this->label2->TabIndex = 50;
			this->label2->Text = L"ROI (mm)";
			// 
			// UA_ROI_Width_txt
			// 
			this->UA_ROI_Width_txt->Enabled = false;
			this->UA_ROI_Width_txt->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 20.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->UA_ROI_Width_txt->Location = System::Drawing::Point(470, 258);
			this->UA_ROI_Width_txt->Name = L"UA_ROI_Width_txt";
			this->UA_ROI_Width_txt->Size = System::Drawing::Size(72, 38);
			this->UA_ROI_Width_txt->TabIndex = 51;
			// 
			// UA_ROI_Height_txt
			// 
			this->UA_ROI_Height_txt->Enabled = false;
			this->UA_ROI_Height_txt->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 20.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->UA_ROI_Height_txt->Location = System::Drawing::Point(607, 258);
			this->UA_ROI_Height_txt->Name = L"UA_ROI_Height_txt";
			this->UA_ROI_Height_txt->Size = System::Drawing::Size(72, 38);
			this->UA_ROI_Height_txt->TabIndex = 52;
			// 
			// UA_ROI_Width_lbl
			// 
			this->UA_ROI_Width_lbl->AutoSize = true;
			this->UA_ROI_Width_lbl->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->UA_ROI_Width_lbl->Location = System::Drawing::Point(416, 267);
			this->UA_ROI_Width_lbl->Name = L"UA_ROI_Width_lbl";
			this->UA_ROI_Width_lbl->Size = System::Drawing::Size(53, 23);
			this->UA_ROI_Width_lbl->TabIndex = 53;
			this->UA_ROI_Width_lbl->Text = L"Width:";
			// 
			// UA_ROI_Height_lbl
			// 
			this->UA_ROI_Height_lbl->AutoSize = true;
			this->UA_ROI_Height_lbl->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->UA_ROI_Height_lbl->Location = System::Drawing::Point(548, 267);
			this->UA_ROI_Height_lbl->Name = L"UA_ROI_Height_lbl";
			this->UA_ROI_Height_lbl->Size = System::Drawing::Size(58, 23);
			this->UA_ROI_Height_lbl->TabIndex = 54;
			this->UA_ROI_Height_lbl->Text = L"Height:";
			// 
			// Error_UA
			// 
			this->Error_UA->AutoSize = true;
			this->Error_UA->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Error_UA->ForeColor = System::Drawing::Color::Red;
			this->Error_UA->Location = System::Drawing::Point(688, 263);
			this->Error_UA->Name = L"Error_UA";
			this->Error_UA->Size = System::Drawing::Size(210, 29);
			this->Error_UA->TabIndex = 55;
			this->Error_UA->Text = L"Numerical Values Only";
			this->Error_UA->Visible = false;
			// 
			// DropSourceFrom
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::Control;
			this->ClientSize = System::Drawing::Size(892, 861);
			this->Controls->Add(this->Error_UA);
			this->Controls->Add(this->UA_ROI_Height_lbl);
			this->Controls->Add(this->UA_ROI_Width_lbl);
			this->Controls->Add(this->UA_ROI_Height_txt);
			this->Controls->Add(this->UA_ROI_Width_txt);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->UA_CFR_txt);
			this->Controls->Add(this->UA_Enable_cbox);
			this->Controls->Add(this->UA_CFR_lbl);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->ImgSubEn_cbox);
			this->Controls->Add(this->ThreshGroupBox);
			this->Controls->Add(this->ImageProcessing_lbl);
			this->Controls->Add(this->DropAnalysis_lbl);
			this->Controls->Add(this->FileIO_lbl);
			this->Controls->Add(this->TestInfo_lbl);
			this->Controls->Add(this->XLSX_cbox);
			this->Controls->Add(this->Error_SelectParam);
			this->Controls->Add(this->Error_OutputFile);
			this->Controls->Add(this->DebugImg_cbox);
			this->Controls->Add(this->CSV_cbox);
			this->Controls->Add(this->OutputFiles_label);
			this->Controls->Add(this->DropVolume_cbox);
			this->Controls->Add(this->LigLength_cbox);
			this->Controls->Add(this->Satellites_cbox);
			this->Controls->Add(this->Velocity_cbox);
			this->Controls->Add(this->Position_cbox);
			this->Controls->Add(this->Parameters_lbl);
			this->Controls->Add(this->ProgressBar);
			this->Controls->Add(this->PB_Label);
			this->Controls->Add(this->Error_Output);
			this->Controls->Add(this->Error_Input);
			this->Controls->Add(this->Error_FPS);
			this->Controls->Add(this->Error_ROI);
			this->Controls->Add(this->Error_NoT);
			this->Controls->Add(this->StartAnalysis_button);
			this->Controls->Add(this->OutputDir_button);
			this->Controls->Add(this->InputDir_button);
			this->Controls->Add(this->OutputDir_text);
			this->Controls->Add(this->InputDir_text);
			this->Controls->Add(this->FPS_text);
			this->Controls->Add(this->Height_txt);
			this->Controls->Add(this->Width_text);
			this->Controls->Add(this->NameOfTest_Text);
			this->Controls->Add(this->OutputDir_label);
			this->Controls->Add(this->InputDir_label);
			this->Controls->Add(this->Height);
			this->Controls->Add(this->Width);
			this->Controls->Add(this->FPS_label);
			this->Controls->Add(this->ROI_label);
			this->Controls->Add(this->NameOfTest_label);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->Name = L"DropSourceFrom";
			this->Text = L"DropSource";
			this->Load += gcnew System::EventHandler(this, &DropSourceFrom::DropSourceFrom_Load);
			this->ThreshGroupBox->ResumeLayout(false);
			this->ThreshGroupBox->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void DropSourceFrom_Load(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void label2_Click(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void button1_Click(System::Object^ sender, System::EventArgs^ e) {
		//test_ocv();
	}
	private: System::Void label1_Click(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void label1_Click_1(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void label2_Click_1(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void textBox1_TextChanged(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void maskedTextBox1_MaskInputRejected(System::Object^ sender, System::Windows::Forms::MaskInputRejectedEventArgs^ e) {
	}
	private: System::Void ROIDimensions_Click(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void button1_Click_1(System::Object^ sender, System::EventArgs^ e) {
		System::Windows::Forms::DialogResult dia_res = System::Windows::Forms::DialogResult::OK;
		folderBrowserDialog2->ShowNewFolderButton = true;
		if (folderBrowserDialog2->ShowDialog() == dia_res)
		{
			OutputDir_text->Text = folderBrowserDialog2->SelectedPath;
		}
	}
	private: System::Void openFileDialog1_FileOk(System::Object^ sender, System::ComponentModel::CancelEventArgs^ e) {
	}
	private: System::Void InputDir_button_Click(System::Object^ sender, System::EventArgs^ e) {
		System::Windows::Forms::DialogResult dia_res = System::Windows::Forms::DialogResult::OK;
		folderBrowserDialog1->ShowNewFolderButton = false;
		if (folderBrowserDialog1->ShowDialog() == dia_res)
		{
			InputDir_text->Text = folderBrowserDialog1->SelectedPath;
		}
	}
	private: System::Void folderBrowserDialog1_HelpRequest(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void StartAnalysis_button_Click(System::Object^ sender, System::EventArgs^ e);

	private: System::Void PB_Label_Click(System::Object^ sender, System::EventArgs^ e) {

	}

		   // Define own methods for the class here
	public:
		/*HELPER METHODS***********************************************************************************/
		bool User_Input_Error_Check();
		bool Process_Video();
		// Clear all data from previous session
		void ClearAllData();
		// Setter Functions
		void setWidthAndHeight_Px(float width, float height);
		// Getter Functions
		float getDeltaT();
		// Converter Functions
		float Pixels2mm(float data, bool isWidth);
		float mm2Pixel(float data, bool isWidth);
		// Simulate cropping of image (for testing purposes)
		cv::Mat SimCropBinaryImage(cv::Mat bin_image);
		// Predicts position of main drop at next time instance
		cv::Point2f PredictNextMainDropPosition(cv::Point2f detected_pos, int index);

		/*IMAGE HANDLING METHODS***************************************************************************/
		// returns vector of image frames in desired load type (ie colour or grayscale)
		// TODO: Maybe save as private member?
		std::vector<cv::Mat> LoadImages(int IMREAD_TYPE);
		void LoadGrayscaleImages();
		void LoadColorImages();

		// function to get vector of time values of test
		// Units: ms
		void MakeTimeVector(int size);
		// function to get y position of main drop
		void MainDropPositions();
		// function to get y velocity of main drop (TODO)
		void MainDropVelocities();
		// function to get number of satellites
		void CountNumberOfSatellites();
		// function to calculate ligament length
		void CalculateLigLength();
		// function to calculate main drop volume
		void CalculateMainDropVol();
		// function to gather all parameter data
		void DropletAnalysis();

		// Function to write to xlsx file
		void Write2XLSX();
		// Function to write to CSV file
		void Write2CSV();

		// Debug Images Function
		void DebugImages();

		/*GUI HANDLING METHODS*****************************************************************************/
		void ProgressBarUpdate(System::String^ message, int min, int max, int level, bool visible);

		/*DEBUG METHODS************************************************************************************/
		void DrawDetectedAndPredictedCenters(bool enablePredic);
		void DrawAllCentroids();
		void DrawBoundingRects();
		void DrawExtBotPoints();
		void DrawMainDropMask();

		/*TESTING METHODS**********************************************************************************/
		bool TestPreProcessing();
		bool TestDrawContours();
		bool TestTimeVector();
		void TestFunctions();
		void TestDetectPredict();
		void TestBoundingRect();
		void TestExtBotPoints();
		void TestMainDropMask();
		void TestImgSubtraction();

	// Functions to see if user has changed inputs
	private: System::Void SimulateCrop_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void Velocity_cbox_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		this->Position_cbox->Checked = this->Velocity_cbox->Checked;
	}
	private: System::Void Position_cbox_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		if (!this->Position_cbox->Checked)
		{
			this->Velocity_cbox->Checked = false;
			this->Satellites_cbox->Checked = false;
			this->LigLength_cbox->Checked = false;
			this->DropVolume_cbox->Checked = false;
		}
	}
	private: System::Void Satellites_cbox_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		this->Position_cbox->Checked = this->Satellites_cbox->Checked;
	}
	private: System::Void LigLength_cbox_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		this->Position_cbox->Checked = this->LigLength_cbox->Checked;
	}
	private: System::Void DropVolume_cbox_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		this->Position_cbox->Checked = this->DropVolume_cbox->Checked;
	}
	private: System::Void GlobalThresh_RdBtn_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		if (this->GlobalThresh_RdBtn->Checked)
		{
			this->AdaptiveThresh_RdBtn->Checked = false;
			this->ThreshType = THRESH_GLOBAL;
			
		}
	}
	private: System::Void AdaptiveThresh_RdBtn_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		if (this->AdaptiveThresh_RdBtn->Checked)
		{
			this->GlobalThresh_RdBtn->Checked = false;
			this->ThreshType = THRESH_ADAPTIVE;
		}

	
	}
	private: System::Void UA_Enable_cbox_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
			   this->UA_CFR_txt->Enabled = this->UA_Enable_cbox->Checked;
			   this->UA_ROI_Width_txt->Enabled = this->UA_Enable_cbox->Checked;
			   this->UA_ROI_Height_txt->Enabled = this->UA_Enable_cbox->Checked;
	}
	private: System::Void UA_CFR_txt_TextChanged(System::Object^ sender, System::EventArgs^ e) {
	}

};
}


