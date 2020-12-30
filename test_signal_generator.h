/********************************************************************************************************************************
 * FilterTools/test_signal_generator.h
 *
 * Author	: Ben Passman
 * Created	: 25/12/2020
 *
 * Description	: Generator functions for creating test waveforms
 *
 * Revision History:
 * Date		Author		Rev	Notes
 * 25/12/2020	Ben P		1.01	Created header file.
 *
 * */

enum menu_commands{ 
	DO_NOTHING,
	SELECT_PREVIOUS_OPTION, 
	SELECT_NEXT_OPTION, 
	EXECUTE_SELECTED_OPTION, 
	QUIT };

#define MAIN_MENU_WINDOW_LINES 10
#define MAIN_MENU_WINDOW_COLUMNS 50
#define MAIN_MENU_OPTIONS \
{ \
	{ 1, 2, "Edit settings" }, \
	{ 2, 2, "Add new wave" }, \
	{ 3, 2, "Modify existing wave" }, \
	{ 4, 2, "Delete wave" }, \
	{ 5, 2, "Load wave set" }, \
	{ 6, 2, "Save wave set" }, \
	{ 7, 2, "Export combined waveform" } \
}

#define FILE_WINDOW_LINES 8
#define FILE_WINDOW_COLUMNS 50
#define FILE_MENU_LABELS \
{ \
	{ 2, 2, "Path:" } \
}

#define FILE_MENU_OPTIONS \
{ \
	{ 2, 8, "~/Documents/test.txt" }, \
	{ 5, 8, "   Ok   " }, \
	{ 5, 20, " Cancel " } \
}

#define EDIT_WINDOW_LINES 20
#define EDIT_WINDOW_COLUMNS 50
