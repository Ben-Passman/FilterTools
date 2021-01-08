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
 * 22/12/2020	Ben P		1.0	Sine, saw, triangle and square wave generators implemented.
 * 25/12/2020	Ben P		1.01	Generator code freeze, begin work on ncurses menu code.
 *
 ************************************************************************************************ */

#include "test_signal_generator.h"
#include "src/form_handler.h"
#include "src/menu_handler.h"
#include "src/waveforms.h"

#include "src/input_validation.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#include <menu.h>
#include <form.h>
#include <panel.h>

struct FormTemplate wave_controls[] = {
	{ LABEL_FIELD, 0, 0, 2, 4, "Path:" },
	{ PATH_FIELD, 1, 20, 0, 0, "" },
	{ LABEL_FIELD, 0, 0, 4, 4, "Number:" },
	{ NUMBER_FIELD, 1, 20, 2, 0, "" },
	{ LABEL_FIELD, 0, 0, 6, 4, "List:" },
	{ LIST_FIELD, 1, 20, 4, 0, "" },
	{ LABEL_FIELD, 0, 0, 8, 4, "Button:" },
	{ OK_FIELD, 1, 8, 6, 0, "   Ok   " },
	{ CANCEL_FIELD, 1, 8, 6, 10, " Cancel " }
};

int main(int argc, char **argv)
{
	initscr();	// Init screen, setup memory and clear screen
	cbreak();	// Unbuffered input, enables Ctrl + C to exit ncurses (default)
	noecho();	// User input is not echoed
	curs_set(0);	// Hide cursor
	
	WINDOW *menu_window = newwin(MAIN_MENU_SIZE, MAIN_MENU_LOCATION);
	WINDOW *output_window = newwin(OUTPUT_WINDOW_SIZE, OUTPUT_WINDOW_LOCATION);
	WINDOW *popup_window = newwin(POPUP_WINDOW_SIZE, POPUP_WINDOW_LOCATION);
		
	box(menu_window, 0, 0);
	box(output_window, 0, 0);
	box(popup_window, 0, 0);
	
	struct Menu main_menu = main_menu_setup(menu_window);

	PANEL *menu_panel;
	PANEL *output_panel;
	PANEL *popup_panel;
	menu_panel = new_panel(menu_window);
	output_panel = new_panel(output_window);
	popup_panel = new_panel(popup_window);

//	hide_panel(popup_panel);

	struct Form file_form = form_setup(popup_window, &wave_controls[0], sizeof wave_controls / sizeof wave_controls[0] );
	update_panels();
	
	doupdate();

	// MENU INTERFACE
	int c = 0;
//	keypad(menu_window, TRUE);
//	while((c = wgetch(menu_window)) != 'q')
//	{
//	main_menu_driver(main_menu.menu, c);
               
//		update_panels();
//        doupdate();
//	}
	
	// FORM INTERFACE

	form_highlight_active(file_form.form);
	keypad(popup_window, TRUE);
	
	while((c = wgetch(popup_window)) != 'q')
	{
        form_menu_driver(popup_window, &file_form, c);
        
        int index = field_index(current_field(file_form.form));
        mvwprintw(popup_window, 12, 2, "Selected: %d", field_index(current_field(file_form.form)));
        mvwprintw(popup_window, 13, 2, "Buffer contents: %s", field_buffer(current_field(file_form.form), 0));             
	}

	free_menu_struct(main_menu);
	free_form_struct(file_form);
	endwin();
	
    return 1;
}	
