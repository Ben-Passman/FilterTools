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

#define MAIN_MENU_SIZE 10, 50
#define MAIN_MENU_LOCATION 2, (COLS - 50) / 2
#define MAIN_MENU_OPTIONS \
{ \
	"Edit settings", \
	"Add new wave", \
	"Modify existing wave", \
	"Delete wave", \
	"Load wave set", \
	"Save wave set", \
	"Export combined waveform" \
}

#define OUTPUT_WINDOW_SIZE 20, 50
#define OUTPUT_WINDOW_LOCATION 13, (COLS - 50) / 2

#define POPUP_WINDOW_SIZE 20, 60
#define POPUP_WINDOW_LOCATION 10, 15

#define FILE_MENU_OPTIONS \
{ \
	"~/Documents/test.txt", \
	"   Ok   ", \
	" Cancel " \
}
