/************************************************************************************************
 * FilterTools/main_menu.c
 * 
 * Author	: Ben Passman
 * Created	: 25/1/2021
 * 
 * Description	: Main window controls 
 *
 * Revision History:
 * Date		Author		Rev	Notes
 * 25/1/2021	Ben P		1.0	File created.
 *
 ************************************************************************************************ */

#include "main_menu.h"
#include "main_settings_form.h"
#include "wave_settings_form.h"
#include "input_validation.h"

#include <stdlib.h>
#include <panel.h>

static WINDOW *output_window;
static WINDOW *wave_settings_window;
static WINDOW *settings_window;	

static PANEL *output_panel;
static PANEL *wave_settings_panel;
static PANEL *settings_panel;
    
static char export_path[FORM_PATH_MAX];
static struct Form wave_form;
static struct Form main_settings_form;
static struct WaveList waves = {
	.export_path = export_path,
	.sample_count = 1000,
	.sample_frequency = 48000.00,
	.first = NULL,
	.selected = NULL
};
		
void initialise_ncurses()
{
	initscr();	// Init screen, setup memory and clear screen
	cbreak();	// Unbuffered input, enables Ctrl + C to exit ncurses (default)
	noecho();	// User input is not echoed
	curs_set(0);	// Hide cursor
}

void initialise_windows()
{
	output_window = newwin(OUTPUT_WINDOW_SIZE, OUTPUT_WINDOW_LOCATION);
	wave_settings_window = newwin(POPUP_WINDOW_SIZE, POPUP_WINDOW_LOCATION);
	settings_window = newwin(POPUP_WINDOW_SIZE, POPUP_WINDOW_LOCATION);

	keypad(output_window, TRUE);
	keypad(wave_settings_window, TRUE);
    keypad(settings_window, TRUE);
	
	box(output_window, 0, 0);
	box(wave_settings_window, 0, 0);
	box(settings_window, 0, 0);
	}

void initialise_panels()
{
	output_panel = new_panel(output_window);
	wave_settings_panel = new_panel(wave_settings_window);
	settings_panel = new_panel(settings_window);
	
	hide_panel(wave_settings_panel);
	hide_panel(settings_panel);
	update_panels();
	doupdate();
 
}

void initialise_forms()
{
	init_custom_fields();
	
	wave_form = wave_settings_setup(wave_settings_window);
    main_settings_form = main_settings_setup(settings_window);
	
	post_form(wave_form.form);
    post_form(main_settings_form.form);
	form_highlight_active(wave_form.form);	
    form_highlight_active(main_settings_form.form);	
		
}

void main_menu_refresh()
{
    wattron(output_window, A_REVERSE);
    mvwprintw(output_window, 1, 2, "Samples: %d Sampling Frequency: %.2f", waves.sample_count, waves.sample_frequency);
    mvwprintw(output_window, 2, 2, "Shape:     Amplitude:  Frequency:  Phase:    Duty:     DC Offset:         ");
  
    struct WaveForm *wave = waves.first;
	char wave_data[72];
    int row = 3;
    while(wave != NULL)
	{
		if (wave == waves.selected)
		{
			wattron(output_window, A_REVERSE);
		}
		else
		{
			wattroff(output_window, A_REVERSE);
		}
		
        sprint_SI(&wave_data[0], wave->amplitude, 8, 3);
        wave_data[9] = ' ';
        wave_data[10] = ' ';
        wave_data[11] = ' ';
        sprint_SI(&wave_data[12], wave->frequency, 8, 3);
        wave_data[21] = ' ';
        wave_data[22] = ' ';
        wave_data[23] = ' ';
        sprint_SI(&wave_data[24], wave->phase, 8, 3);
        wave_data[33] = ' ';
        sprint_SI(&wave_data[34], wave->duty, 8, 3);
        wave_data[43] = ' ';
        sprint_SI(&wave_data[44], wave->dc_offset, 8, 3);
		
        mvwprintw(output_window, row, 2, "%s  %s   %s",
            wave_types[wave->type],
            &wave_data[0],
            wave_modes[wave->mode]
        );
		wave = wave->next;
		row++;
	}
	wattroff(output_window, A_REVERSE);
	mvwprintw(output_window, row, 2, "                                                                        ");
	wrefresh(output_window);
}

int main_menu_driver()
{
	int c = 0;
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
		main_menu_refresh();
	}
    return 1;
}

void free_ncurses()
{
	while (waves.first != NULL)
	{
		delete_wave(&waves);
	}
	free_form_struct(wave_form);
    free_form_struct(main_settings_form);
	free(wave_settings_panel);
	free(output_panel);
    free(settings_panel);

	endwin();		
}

