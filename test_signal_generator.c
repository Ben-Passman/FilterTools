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
#include "src/ui_forms.h"

#include <stdlib.h>

#include <panel.h>

void print_waves(WINDOW *window, struct WaveList *wlist)
{
	struct WaveForm *wave = wlist->first;
	int row = 2;
        wattron(window, A_REVERSE);
    mvwprintw(window, 1, 2, "Shape:     Amplitude:  Frequency:    Phase:    Duty:  DC Offset:        ");
	wattroff(window, A_REVERSE);
	while(wave != NULL)
	{
		if (wave == wlist->selected)
		{
			wattron(window, A_REVERSE);
		}
		else
		{
			wattroff(window, A_REVERSE);
		}
		mvwprintw(window, row, 2, "SAWTOOTH   1000.000    1000.000 Hz   1000.000  1.0    1000.000  SUBTRACT");
		wave = wave->next;
		row++;
	}
	wattroff(window, A_REVERSE);
	mvwprintw(window, row, 2, "                                                                        ");
	wrefresh(window);
}

int main(void)
{
	initscr();	// Init screen, setup memory and clear screen
	cbreak();	// Unbuffered input, enables Ctrl + C to exit ncurses (default)
	noecho();	// User input is not echoed
	curs_set(0);	// Hide cursor
	
	WINDOW *output_window = newwin(OUTPUT_WINDOW_SIZE, OUTPUT_WINDOW_LOCATION);
	WINDOW *popup_window = newwin(POPUP_WINDOW_SIZE, POPUP_WINDOW_LOCATION);
		
	box(output_window, 0, 0);
	box(popup_window, 0, 0);
	
	PANEL *output_panel = new_panel(output_window);
	PANEL *popup_panel = new_panel(popup_window);
	hide_panel(popup_panel);
	update_panels();
	doupdate();

	init_form_handler();
	struct Form wave_form = wave_settings_setup(popup_window);
	struct Form *active_form = &wave_form;
	struct WaveList waves = { NULL, NULL };
	print_waves(output_window, &waves);

	// MENU INTERFACE
	int c = 0;
	keypad(output_window, TRUE);
	keypad(popup_window, TRUE);
	post_form(active_form->form);
	form_highlight_active(active_form->form);	

	while((c = wgetch(output_window)) != 'q')
	{
		struct Dropdown *d;
		switch(c)
		{
			case ' ' :
				// Read field index:
				d = (struct Dropdown *) field_userptr(active_form->fields[0]);
				mvwprintw(output_window, 17, 2, "Form field 0 index: %d", d->index);
				mvwprintw(output_window, 18, 2, "Selected wave %d", waves.selected->type);
				

				break;
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
			case '\n' :
				if (waves.selected != NULL)
				{
					set_wave_fields(active_form, waves.selected);

					show_panel(popup_panel);
					update_panels();
					doupdate();
					while((c = wgetch(popup_window)) != 'q')
					{
					        form_menu_driver(popup_window, active_form, c);
					}
					hide_panel(popup_panel);
					update_panels();
					doupdate();

					get_wave_fields(active_form, waves.selected);
				
					mvwprintw(output_window, 18, 2, "%d, %lf, %lf, %lf, %lf, %d, %lf", waves.selected->type, waves.selected->amplitude, waves.selected->frequency, waves.selected->phase, waves.selected->duty, waves.selected->mode, waves.selected->dc_offset);
				}
				break;
		}
		if (panel_hidden(popup_panel))
		{
			print_waves(output_window, &waves);
		}
	}
	
	// Free resources
	while (waves.first != NULL)
	{
		delete_wave(&waves);
	}
	free_form_struct(wave_form);
	free(popup_panel);
	free(output_panel);

	endwin();
	
    return 1;
}	
