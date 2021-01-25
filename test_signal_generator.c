/************************************************************************************************
 * FilterTools/test_signal_generator.c
 * 
 * Author	: Ben Passman
 * Created	: 22/12/2020
 * 
 * Description	: Generator functions for creating test waveforms. 
 *
 * Revision History:
 * Date		Author		Rev	Notes
 * 22/12/2020	Ben P		1.0	File created.
 *
 ************************************************************************************************ */

#include "test_signal_generator.h"
#include "src/main_menu.h"

int main(void)
{
	initialise_ncurses();
	initialise_windows();
	initialise_panels();
	initialise_forms();
	
	main_menu_refresh();
	main_menu_driver();
	
	free_ncurses();

	return 1;
}	
