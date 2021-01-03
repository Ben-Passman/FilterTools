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
#define POPUP_WINDOW_LOCATION 10, (COLS - 60) / 2

const char * WAVE_LIST[] = {
	"SINE",
	"SAWTOOTH",
	"TRIANGLE",
	"SQUARE"
};

enum field_type {
	PATH_FIELD,
	NUMBER_FIELD,
	LIST_FIELD,
	OK_FIELD,
	CANCEL_FIELD
};
