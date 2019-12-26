/*
*	@brief
*	This is the Windows Form Object that drives the GUI. It was created by the Visual Studio IDE.
*	Here, external functions will be used as callbacks for buttons, the user will input parameters
*	for every individual DOD analysis. The purposes of the external files are as follows:
*
*	ImageProc.h		->		Used to do image processing and droplet analysis
*	UI.h			->		Used to handle user inputs
*
*	AUTHOR:		Akshin Goswami
*	DATE:		17/12/19
*	VERSION:	0.1.0
*/

#pragma once

#include "ImageProc.h"
#include "UI.h"
#include "Test_Macro.h"
//
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

	private:
		

















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
			this->ROI_label->Size = System::Drawing::Size(146, 29);
			this->ROI_label->TabIndex = 1;
			this->ROI_label->Text = L"ROI (pixels)";
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
			this->StartAnalysis_button->Location = System::Drawing::Point(24, 542);
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
			this->PB_Label->Location = System::Drawing::Point(233, 533);
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
			this->ProgressBar->Location = System::Drawing::Point(238, 565);
			this->ProgressBar->Name = L"ProgressBar";
			this->ProgressBar->Size = System::Drawing::Size(641, 29);
			this->ProgressBar->TabIndex = 24;
			this->ProgressBar->Visible = false;
			// 
			// DropSourceFrom
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::Control;
			this->ClientSize = System::Drawing::Size(1352, 635);
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
	bool User_Input_Error_Check();
	bool Process_Video();

	/*IMAGE HANDLING METHODS***************************************************************************/
	// returns vector of image in desired load type (ie colour or grayscale)
	// TODO: Maybe save as private member?
	std::vector<cv::Mat> LoadImages(int IMREAD_TYPE);

	/*TESTING METHODS**********************************************************************************/
	bool TestPreProcessing();

	bool TestDrawContours();
};
}
