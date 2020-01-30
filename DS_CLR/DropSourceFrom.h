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
*	VERSION:	0.1.0
*
*
*	TODO:
*	1. CSV Writer functions for data output
*	2. Place info above on contoured image for validation
*	3. Determine length of ligament
*	4. Determine volume of main drop.
*/

#pragma once

#include "ImageProc.h"
#include "UI.h"
#include "Test_Macro.h"
#include "pid.h"
#include "PID_Params.h"
#include "CSVWriter.h"

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

			// Initialise dataset
			TimeVector = new std::vector<float>;
			MainDropPosition = new std::vector<float>;
			MainDropVelocity = new std::vector<float>;
			NumberOfSatellites = new std::vector<int>;
			LigamentLength = new std::vector<float>;
			Volume = new std::vector<float>;
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

		// Images
		std::vector<cv::Mat>* GrayscaleImages;
		std::vector<cv::Mat>* ColorImages;

		// Points on image
		std::vector<cv::Point2f>* MainDropPoints;
		std::vector<cv::Point2f>* MainDropControl;
		std::vector<cv::Point2f>* MainDropPredic;

		// Velocity in terms of pixel position
		std::vector<cv::Point2f>* Velocity_px;

		// Data Sets
		std::vector<float>* TimeVector;
		std::vector<float>* MainDropPosition;
		std::vector<float>* MainDropVelocity;
		std::vector<int>* NumberOfSatellites;
		std::vector<float>* LigamentLength;
		std::vector<float>* Volume;

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




		   // TODO: Ligament length and Volume
		

	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

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
			this->SuspendLayout();
			// 
			// NameOfTest_label
			// 
			this->NameOfTest_label->AutoSize = true;
			this->NameOfTest_label->Font = (gcnew System::Drawing::Font(L"Arial", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->NameOfTest_label->Location = System::Drawing::Point(19, 31);
			this->NameOfTest_label->Name = L"NameOfTest_label";
			this->NameOfTest_label->Size = System::Drawing::Size(156, 29);
			this->NameOfTest_label->TabIndex = 0;
			this->NameOfTest_label->Text = L"Name of test";
			// 
			// ROI_label
			// 
			this->ROI_label->AutoSize = true;
			this->ROI_label->Font = (gcnew System::Drawing::Font(L"Arial", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ROI_label->Location = System::Drawing::Point(19, 99);
			this->ROI_label->Name = L"ROI_label";
			this->ROI_label->Size = System::Drawing::Size(120, 29);
			this->ROI_label->TabIndex = 1;
			this->ROI_label->Text = L"ROI (mm)";
			this->ROI_label->Click += gcnew System::EventHandler(this, &DropSourceFrom::ROIDimensions_Click);
			// 
			// FPS_label
			// 
			this->FPS_label->AutoSize = true;
			this->FPS_label->Font = (gcnew System::Drawing::Font(L"Arial", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->FPS_label->Location = System::Drawing::Point(19, 197);
			this->FPS_label->Name = L"FPS_label";
			this->FPS_label->Size = System::Drawing::Size(112, 29);
			this->FPS_label->TabIndex = 2;
			this->FPS_label->Text = L"FPS (Hz)";
			// 
			// Width
			// 
			this->Width->AutoSize = true;
			this->Width->Font = (gcnew System::Drawing::Font(L"Arial", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Width->Location = System::Drawing::Point(233, 101);
			this->Width->Name = L"Width";
			this->Width->Size = System::Drawing::Size(82, 27);
			this->Width->TabIndex = 3;
			this->Width->Text = L"Width:";
			this->Width->Click += gcnew System::EventHandler(this, &DropSourceFrom::label1_Click);
			// 
			// Height
			// 
			this->Height->AutoSize = true;
			this->Height->Font = (gcnew System::Drawing::Font(L"Arial", 18, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Height->Location = System::Drawing::Point(470, 101);
			this->Height->Name = L"Height";
			this->Height->Size = System::Drawing::Size(89, 27);
			this->Height->TabIndex = 4;
			this->Height->Text = L"Height:";
			this->Height->Click += gcnew System::EventHandler(this, &DropSourceFrom::label1_Click_1);
			// 
			// InputDir_label
			// 
			this->InputDir_label->AutoSize = true;
			this->InputDir_label->Font = (gcnew System::Drawing::Font(L"Arial", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->InputDir_label->Location = System::Drawing::Point(19, 290);
			this->InputDir_label->Name = L"InputDir_label";
			this->InputDir_label->Size = System::Drawing::Size(184, 29);
			this->InputDir_label->TabIndex = 5;
			this->InputDir_label->Text = L"Input Directory";
			// 
			// OutputDir_label
			// 
			this->OutputDir_label->AutoSize = true;
			this->OutputDir_label->Font = (gcnew System::Drawing::Font(L"Arial", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->OutputDir_label->Location = System::Drawing::Point(19, 428);
			this->OutputDir_label->Name = L"OutputDir_label";
			this->OutputDir_label->Size = System::Drawing::Size(203, 29);
			this->OutputDir_label->TabIndex = 7;
			this->OutputDir_label->Text = L"Output Directory";
			this->OutputDir_label->Click += gcnew System::EventHandler(this, &DropSourceFrom::label2_Click_1);
			// 
			// NameOfTest_Text
			// 
			this->NameOfTest_Text->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 20.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->NameOfTest_Text->Location = System::Drawing::Point(238, 31);
			this->NameOfTest_Text->Name = L"NameOfTest_Text";
			this->NameOfTest_Text->Size = System::Drawing::Size(441, 38);
			this->NameOfTest_Text->TabIndex = 8;
			this->NameOfTest_Text->TextChanged += gcnew System::EventHandler(this, &DropSourceFrom::textBox1_TextChanged);
			// 
			// Width_text
			// 
			this->Width_text->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 20.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Width_text->Location = System::Drawing::Point(321, 99);
			this->Width_text->Name = L"Width_text";
			this->Width_text->Size = System::Drawing::Size(114, 38);
			this->Width_text->TabIndex = 9;
			// 
			// Height_txt
			// 
			this->Height_txt->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 20.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Height_txt->Location = System::Drawing::Point(565, 101);
			this->Height_txt->Name = L"Height_txt";
			this->Height_txt->Size = System::Drawing::Size(114, 38);
			this->Height_txt->TabIndex = 10;
			// 
			// FPS_text
			// 
			this->FPS_text->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 20.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->FPS_text->Location = System::Drawing::Point(238, 197);
			this->FPS_text->Name = L"FPS_text";
			this->FPS_text->Size = System::Drawing::Size(197, 38);
			this->FPS_text->TabIndex = 11;
			// 
			// InputDir_text
			// 
			this->InputDir_text->Enabled = false;
			this->InputDir_text->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 20.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->InputDir_text->Location = System::Drawing::Point(238, 290);
			this->InputDir_text->Name = L"InputDir_text";
			this->InputDir_text->Size = System::Drawing::Size(441, 38);
			this->InputDir_text->TabIndex = 12;
			// 
			// OutputDir_text
			// 
			this->OutputDir_text->Enabled = false;
			this->OutputDir_text->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 20.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->OutputDir_text->Location = System::Drawing::Point(238, 428);
			this->OutputDir_text->Name = L"OutputDir_text";
			this->OutputDir_text->Size = System::Drawing::Size(441, 38);
			this->OutputDir_text->TabIndex = 13;
			// 
			// InputDir_button
			// 
			this->InputDir_button->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->InputDir_button->Location = System::Drawing::Point(24, 322);
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
			this->OutputDir_button->Location = System::Drawing::Point(24, 460);
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
			this->StartAnalysis_button->Location = System::Drawing::Point(24, 740);
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
			this->Error_NoT->Location = System::Drawing::Point(685, 40);
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
			this->Error_ROI->Location = System::Drawing::Point(685, 110);
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
			this->Error_FPS->Location = System::Drawing::Point(685, 197);
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
			this->Error_Input->Location = System::Drawing::Point(685, 295);
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
			this->Error_Output->Location = System::Drawing::Point(685, 433);
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
			this->PB_Label->Location = System::Drawing::Point(233, 731);
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
			this->ProgressBar->Location = System::Drawing::Point(238, 763);
			this->ProgressBar->Name = L"ProgressBar";
			this->ProgressBar->Size = System::Drawing::Size(441, 29);
			this->ProgressBar->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
			this->ProgressBar->TabIndex = 24;
			this->ProgressBar->Visible = false;
			// 
			// Parameters_lbl
			// 
			this->Parameters_lbl->AutoSize = true;
			this->Parameters_lbl->Font = (gcnew System::Drawing::Font(L"Arial", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Parameters_lbl->Location = System::Drawing::Point(19, 640);
			this->Parameters_lbl->Name = L"Parameters_lbl";
			this->Parameters_lbl->Size = System::Drawing::Size(141, 29);
			this->Parameters_lbl->TabIndex = 25;
			this->Parameters_lbl->Text = L"Parameters";
			// 
			// Position_cbox
			// 
			this->Position_cbox->AutoSize = true;
			this->Position_cbox->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Position_cbox->Location = System::Drawing::Point(238, 614);
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
			this->Velocity_cbox->Location = System::Drawing::Point(420, 614);
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
			this->Satellites_cbox->Location = System::Drawing::Point(572, 614);
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
			this->LigLength_cbox->Enabled = false;
			this->LigLength_cbox->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->LigLength_cbox->Location = System::Drawing::Point(238, 664);
			this->LigLength_cbox->Name = L"LigLength_cbox";
			this->LigLength_cbox->Size = System::Drawing::Size(164, 29);
			this->LigLength_cbox->TabIndex = 30;
			this->LigLength_cbox->Text = L"Ligament Length";
			this->LigLength_cbox->UseVisualStyleBackColor = true;
			// 
			// DropVolume_cbox
			// 
			this->DropVolume_cbox->AutoSize = true;
			this->DropVolume_cbox->Enabled = false;
			this->DropVolume_cbox->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->DropVolume_cbox->Location = System::Drawing::Point(420, 664);
			this->DropVolume_cbox->Name = L"DropVolume_cbox";
			this->DropVolume_cbox->Size = System::Drawing::Size(152, 29);
			this->DropVolume_cbox->TabIndex = 31;
			this->DropVolume_cbox->Text = L"Droplet Volume";
			this->DropVolume_cbox->UseVisualStyleBackColor = true;
			// 
			// OutputFiles_label
			// 
			this->OutputFiles_label->AutoSize = true;
			this->OutputFiles_label->Font = (gcnew System::Drawing::Font(L"Arial", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->OutputFiles_label->Location = System::Drawing::Point(19, 530);
			this->OutputFiles_label->Name = L"OutputFiles_label";
			this->OutputFiles_label->Size = System::Drawing::Size(154, 29);
			this->OutputFiles_label->TabIndex = 32;
			this->OutputFiles_label->Text = L"Output Files";
			// 
			// CSV_cbox
			// 
			this->CSV_cbox->AutoSize = true;
			this->CSV_cbox->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->CSV_cbox->Location = System::Drawing::Point(238, 534);
			this->CSV_cbox->Name = L"CSV_cbox";
			this->CSV_cbox->Size = System::Drawing::Size(99, 29);
			this->CSV_cbox->TabIndex = 33;
			this->CSV_cbox->Text = L"CSV File";
			this->CSV_cbox->UseVisualStyleBackColor = true;
			// 
			// DebugImg_cbox
			// 
			this->DebugImg_cbox->AutoSize = true;
			this->DebugImg_cbox->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->DebugImg_cbox->Location = System::Drawing::Point(414, 532);
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
			this->Error_OutputFile->Location = System::Drawing::Point(685, 530);
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
			this->Error_SelectParam->Location = System::Drawing::Point(636, 662);
			this->Error_SelectParam->Name = L"Error_SelectParam";
			this->Error_SelectParam->Size = System::Drawing::Size(261, 29);
			this->Error_SelectParam->TabIndex = 36;
			this->Error_SelectParam->Text = L"Select At Least 1 Parameter ";
			this->Error_SelectParam->Visible = false;
			// 
			// DropSourceFrom
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::Control;
			this->ClientSize = System::Drawing::Size(909, 836);
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
	// function to gather all parameter data
	void DropletAnalysis();

	// Function to write to CSV file
	void Write2CSV();

	// TODO: Debug Images Function

	/*GUI HANDLING METHODS*****************************************************************************/
	void ProgressBarUpdate(System::String^ message, int min, int max, int level, bool visible);

	/*DEBUG METHODS************************************************************************************/
	void DrawDetectedAndPredictedCenters(bool enablePredic);
	void DrawAllCentroids();
	void DrawBoundingRects();

	/*TESTING METHODS**********************************************************************************/
	bool TestPreProcessing();

	bool TestDrawContours();

	bool TestTimeVector();

	void TestFunctions();

	void TestDetectPredict();

	void TestBoundingRect();

private: System::Void SimulateCrop_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
}
private: System::Void Velocity_cbox_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
	this->Position_cbox->Checked = this->Velocity_cbox->Checked;
}
private: System::Void Position_cbox_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
	if (!this->Position_cbox->Checked)
	{
		this->Velocity_cbox->Checked = false;
	}

	if (!this->Position_cbox->Checked)
	{
		this->Satellites_cbox->Checked = false;
	}
}
private: System::Void Satellites_cbox_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
	this->Position_cbox->Checked = this->Satellites_cbox->Checked;
}
};
}
