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
#define UI_INPUR_ERROR_CHECK	0

// Enable Helper Function Tests
// Enable all tests selected
#define TEST_ENABLE				0
// Select tests to enable below
#define TEST_OCV				0
#define TEST_LIST_FILES			0
#define TEST_PREPROC			0
#define TEST_DRAW_CONTOURS		0
#define TEST_DRAW_DETECT_PREDIC	0
// Enable Droplet Analysis Tests
#define TEST_TIME_VECTOR		0
#define TEST_DROPLET_ANALYSIS	0

// enable the main loop function
#define MAIN_ENABLE				1