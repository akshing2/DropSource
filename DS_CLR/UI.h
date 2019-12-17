/*
*	@brief
*	These are a set of functions used to handle user input.
*
*	AUTHOR:		Akshin Goswami
*	DATE:		17/12/19
*	VERSION:	0.1.0
*/

#pragma once

#include <iostream>
#include <msclr/marshal_cppstd.h>

// Check input errors, program won't start if there are any.
namespace UI_ERROR
{
	enum FieldErrors
	{
		Passed		= 0,
		NameOfTest	= -1,
		ROI_Width	= -2,
		ROI_Height	= -3,
		FPS			= -4,
		InputDir	= -5,
		OutputDir	= -6,
	};

	bool check_non_empty_field(System::String^ test)
	{
		bool ret = false;
		if (test->Length <=0)
		{
			ret = true;
		}

		return ret;
	}

	bool check_numerical_vals_input(System::String^ test)
	{
		bool ret = false;
		//convert to std::string
		msclr::interop::marshal_context context;
		std::string std_test = context.marshal_as<std::string>(test);
		bool empty = check_non_empty_field(test);

		if (!empty)
		{
			for (std::string::iterator it = std_test.begin(); it != std_test.end(); ++it)
			{
				// if not a digit
				if (isdigit(*it) == 0)
				{
					ret = true;
					break;
				}
			}
		}
		else {
			ret = empty;
		}
		

		

		return ret;
	}

	std::string convert_SYS_to_std_string(System::String^ conv)
	{
		msclr::interop::marshal_context context;
		std::string std_test = context.marshal_as<std::string>(conv);

		return std_test;
	}


}


