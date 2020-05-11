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

	// Function to determine if std::string is a valid number
    int valid_number(std::string str)
    {
        int i = 0, j = str.length() - 1;

        // Handling whitespaces 
        while (i < str.length() && str[i] == ' ')
            i++;
        while (j >= 0 && str[j] == ' ')
            j--;

        if (i > j)
            return false;

        // if string is of length 1 and the only 
        // character is not a digit 
        if (i == j && !(str[i] >= '0' && str[i] <= '9'))
            return false;

        // If the 1st char is not '+', '-', '.' or digit 
        if (str[i] != '.' && str[i] != '+'
            && str[i] != '-' && !(str[i] >= '0' && str[i] <= '9'))
            return false;

        // To check if a '.' or 'e' is found in given 
        // string. We use this flag to make sure that 
        // either of them appear only once. 
        bool flagDotOrE = false;

        for (i; i <= j; i++) {
            // If any of the char does not belong to 
            // {digit, +, -, ., e} 
            if (str[i] != 'e' && str[i] != '.'
                && str[i] != '+' && str[i] != '-'
                && !(str[i] >= '0' && str[i] <= '9'))
                return false;

            if (str[i] == '.') {
                // checks if the char 'e' has already 
                // occurred before '.' If yes, return 0. 
                if (flagDotOrE == true)
                    return false;

                // If '.' is the last character. 
                if (i + 1 > str.length())
                    return false;

                // if '.' is not followed by a digit. 
                if (!(str[i + 1] >= '0' && str[i + 1] <= '9'))
                    return false;
            }

            else if (str[i] == 'e') {
                // set flagDotOrE = 1 when e is encountered. 
                flagDotOrE = true;

                // if there is no digit before 'e'. 
                if (!(str[i - 1] >= '0' && str[i - 1] <= '9'))
                    return false;

                // If 'e' is the last Character 
                if (i + 1 > str.length())
                    return false;

                // if e is not followed either by 
                // '+', '-' or a digit 
                if (str[i + 1] != '+' && str[i + 1] != '-'
                    && (str[i + 1] >= '0' && str[i] <= '9'))
                    return false;
            }
        }

        /* If the string skips all above cases, then
        it is numeric*/
        return true;
    }

	std::string SYS2std_string(System::String^ conv)
	{
		msclr::interop::marshal_context context;
		std::string std_test = context.marshal_as<std::string>(conv);

		return std_test;
	}


}


