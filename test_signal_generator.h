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
	SELECT_PREVIOUS_OPTION, 
	SELECT_NEXT_OPTION, 
	EXECUTE_SELECTED_OPTION, 
	QUIT };

#define MAIN_MENU_LIST { \
	"Edit settings", \
	"Add new wave", \
	"Modify existing wave", \
	"Delete wave", \
	"Load wave set", \
	"Save wave set", \
	"Export combined waveform" }

#define MENU_WINDOW_COLUMNS 50
#define MENU_WINDOW_LINES 10
#define EDIT_WINDOW_COLUMNS 50
#define EDIT_WINDOW_LINES 20
