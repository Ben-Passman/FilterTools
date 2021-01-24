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
#include "src/main_settings_form.h"
#include "src/wave_settings_form.h"

#include <stdlib.h>
#include <panel.h>

int main(void)
{
	initscr();	// Init screen, setup memory and clear screen
	cbreak();	// Unbuffered input, enables Ctrl + C to exit ncurses (default)
	noecho();	// User input is not echoed
	curs_set(0);	// Hide cursor
	
	WINDOW *output_window = newwin(OUTPUT_WINDOW_SIZE, OUTPUT_WINDOW_LOCATION);
	WINDOW *wave_settings_window = newwin(POPUP_WINDOW_SIZE, POPUP_WINDOW_LOCATION);
	WINDOW *settings_window = newwin(POPUP_WINDOW_SIZE, POPUP_WINDOW_LOCATION);	

	box(output_window, 0, 0);
	box(wave_settings_window, 0, 0);
	box(settings_window, 0, 0);
	
	PANEL *output_panel = new_panel(output_window);
	PANEL *wave_settings_panel = new_panel(wave_settings_window);
	PANEL *settings_panel = new_panel(settings_window);
	hide_panel(wave_settings_panel);
	hide_panel(settings_panel);
	update_panels();
	doupdate();

	init_custom_fields();
    
    char export_path[FORM_PATH_MAX];
    export_path[0] = '/';
    export_path[1] = '\0';
    
	struct Form wave_form = wave_settings_setup(wave_settings_window);
    struct Form main_settings_form = main_settings_setup(settings_window);
	struct WaveList waves = {
        .export_path = export_path,
        .sample_count = 1000,
        .sample_frequency = 48000.00,
        .first = NULL,
        .selected = NULL
    };
	print_waves(output_window, &waves);

	// MENU INTERFACE
	int c = 0;
	keypad(output_window, TRUE);
	keypad(wave_settings_window, TRUE);
    keypad(settings_window, TRUE);
	post_form(wave_form.form);
    post_form(main_settings_form.form);
	form_highlight_active(wave_form.form);	
    form_highlight_active(main_settings_form.form);

	while((c = wgetch(output_window)) != 'q')
	{
		switch(c)
		{
			case KEY_UP :
				if (waves.first != NULL && waves.selected->previous != NULL)
				{
					waves.selected = waves.selected->previous;
				}
				break;
			case KEY_DOWN :
				if (waves.first != NULL && waves.selected->next != NULL)
				{
					waves.selected = waves.selected->next;
				}
				break;
			case 'a' :
				add_wave(&waves);
				break;
			case 'd' :
				delete_wave(&waves);
				break;
			case 'w' :
				move_selected_wave_up(&waves);
				break;
			case 's' :
				move_selected_wave_down(&waves);
				break;
			case 'e' :
				if (waves.first != NULL)
				{	
					export_wave(&waves);
				}
				break;
			case 'p' :
                set_main_settings_fields(&main_settings_form, &waves);
                
				show_panel(settings_panel);
				update_panels();
				doupdate();
                
                if (form_menu_driver(settings_window, &main_settings_form))
                {
                    get_main_settings_fields(&main_settings_form, &waves);
                }

				hide_panel(settings_panel);
				update_panels();
				doupdate();
				break;
			case '\n' :
				if (waves.selected != NULL)
				{
					set_wave_fields(&wave_form, waves.selected);

					show_panel(wave_settings_panel);
					update_panels();
					doupdate();

			        if (form_menu_driver(wave_settings_window, &wave_form))
				{
					get_wave_fields(&wave_form, waves.selected);
				}

				hide_panel(wave_settings_panel);
				update_panels();
				doupdate();
				}
				break;
		}
        print_waves(output_window, &waves);
	}
	
	// Free resources
	while (waves.first != NULL)
	{
		delete_wave(&waves);
	}
	free_form_struct(wave_form);
    free_form_struct(main_settings_form);
	free(wave_settings_panel);
	free(output_panel);

	endwin();
	
    return 1;
}	
