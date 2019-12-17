#include "DropSourceFrom.h"
//#include "UI.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace DSCLR;
//using namespace UI_ERROR;

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

[STAThread]
void main(cli::array<System::String^>^ args)
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	DSCLR::DropSourceFrom form;
	Application::Run(%form);
}

