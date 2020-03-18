/*
*	@brief
*	This header file is used to define testing parameters to enable ease of testing.
*	1 -> Enabled, 0 -> DISABLED
*
*	AUTHOR:		Akshin Goswami
*	DATE:		17/12/2019
*	VERSION:	0.1.0
*/
#pragma once

// Enable UI Error Input
#define UI_INPUT_ERROR_CHECK		1

// Enable Helper Function Tests
// Enable all tests selected
#define TEST_ENABLE					!UI_INPUT_ERROR_CHECK
// Select tests to enable below
#define TEST_OCV					0
#define TEST_LIST_FILES				0
#define TEST_PREPROC				0
#define TEST_DRAW_CONTOURS			0
#define TEST_DRAW_DETECT_PREDIC		0
#define TEST_DRAW_BOUND_RECT		0
#define TEST_EXT_BOT_POINTS			0
#define TEST_DRAW_MAIN_DROP_MASK	0
#define TEST_IMG_SUBTRACTION		1
// Enable Droplet Analysis Tests
#define TEST_TIME_VECTOR			0
#define TEST_DROPLET_ANALYSIS		0

// enable the main loop function
#define MAIN_ENABLE					UI_INPUT_ERROR_CHECK