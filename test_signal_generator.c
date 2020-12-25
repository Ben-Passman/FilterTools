/****************************************************************************************************************
 * FilterTools/test_signal_generator.c
 * 
 * Author	: Ben Passman
 * Created	: 22/12/2020
 * 
 * Description	: Generator functions for creating test waveforms. 
 *
 * Revision History:
 * Date		Author		Ref	Revision
 * 22/12/2020	Ben P		1.0	Sine, saw, triangle and square wave generators implemented.
 * */

#include "test_signal_generator.h"

#include<errno.h>
#include<limits.h>
#include<math.h>
#include<stdlib.h>
#include<stdio.h>
#include<ncurses.h>

long double string_to_long_double(char * string)
{
	errno = 0;
	char *endptr;
	long double conv = strtod(string, &endptr);

	if (errno != 0 || *endptr != '\0')
	{
		printf("Failed to parse string, error code %d\n", errno);
		return errno;
	}

	return conv;
}

long double saw_wave(const long double t, const double freq, const double amplitude, const double phase, const double duty)
{
	long double delay = phase / (2.0L * acosl(-1) * freq);
	long double output_point = 2.0L * amplitude * freq * (t - delay) + amplitude;
	output_point = fmodl(output_point, 2.0L * amplitude) - amplitude;
	if (output_point < -amplitude)  // Correction for -ve time
	{
		output_point += 2.0L * amplitude;
	}

	return output_point;
}

long double triangle_wave(const long double t, const double freq, const double amplitude, const double phase, const double duty)
{
	long double delay = phase / (2.0L * acosl(-1) * freq);
	long double output_point = 4.0L * amplitude * freq * (t - delay);
	output_point = fmodl(output_point, 4.0L * amplitude);
	if (output_point < 0)	// Correction for -ve time
	{
		output_point += 4.0L * amplitude;
	}
	if (output_point > amplitude)	
	{
		output_point = 2.0L * amplitude - output_point;
	}
	if (output_point < -amplitude)
	{
		output_point = -output_point - 2.0L * amplitude;
	}

	return output_point;
}

long double square_wave(const long double t, const double freq, const double amplitude, const double phase, const double duty)
{
	long double T = 1 / freq;
	long double delay = phase / (2.0L * acosl(-1) * freq);
	long double local_time = fmodl(t - delay, T);
	if (local_time < 0) // Correction for -ve time
	{
		local_time = T + local_time;
	}

	long double output_point = amplitude;
	if (local_time > duty * T) 
	{
		output_point = -amplitude;
	}

	return output_point;
}

long double sine_wave(const long double t, const double freq, const double amplitude, const double phase, const double duty)
{
	long double w = (t * 2.0L * acosl(-1) * freq) - phase;
	long double outputPoint = amplitude * sinl(w);

	return outputPoint;
}

void generate_signal(const int number_of_samples, const long double sample_rate)
{
	FILE *fp;
	fp = fopen("FFT Test Data.csv", "w");

	if(fp != NULL) {
		long double T = 1.0/sample_rate;	// Sampling period
		long double pi = acosl(-1);		// Use long double for Pi
	
		for(long double i = -number_of_samples * T; i <= number_of_samples * T; i+=T)
		{
			long double w = i * 2.0 * pi;
			long double test_wave = sinl(5.0*w) + 
						sinl(10.0*w) + 
						sinl(50.0*w) + 
						sinl(55.0*w) + 
						sinl(300.0*w) +
						sinl(450.0*w) + 
						//sinl(7000.0*w) + 
						//sinl(10000.0*w) + 
						triangle_wave(i, 7000, 1.2, 0, 0) +
						saw_wave(i, 10000, 1.4, 1.5, 0) +
						square_wave(i, 20000, 1.0, 0.75, 0.3) +
						//sinl(12000.0*w) + 
						//sinl(25000.0*w);
						sine_wave(i, 25000, 1.0, 1.5, 0);
			fprintf(fp, "%Lf, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf\n",
				i, 
				sinl(5.0*w), 
				sinl(10.0*w), 
				sinl(50.0*w), 
				sinl(55.0*w), 
				sinl(300.0*w),
				sinl(450.0*w), 
				//sinl(7000.0*w), 
				//sinl(10000.0*w), 
				triangle_wave(i, 2000, 1.0, 0.75, 0),
				saw_wave(i, 1000, 1.4, 1.5, 0),
				square_wave(i, 2000, 1.0, 3.14, 0.3),
				sine_wave(i, 25000, 1.0, 1.5, 0),
				//sinl(12000.0*w), 
				//sinl(25000.0*w),
				test_wave);
		}
	}
	else
	{
		printf("Could not open file for editing.\n");
		exit(1);
	}
	fclose(fp);
}

void draw_menu(WINDOW * menu_window, const char ** menu_items, const int menu_size, const int active_line)
{
	for (int i = 0; i < menu_size; i++)
	{
		if (i == active_line)
		{
			wattron(menu_window, A_REVERSE);
			mvwprintw(menu_window, 1 + i, 2, menu_items[i]);
			wattroff(menu_window, A_REVERSE);
		}
		else
		{
			mvwprintw(menu_window, 1 + i, 2, menu_items[i]);
		}
	}
	wrefresh(menu_window);
}

int get_input(WINDOW * win)
{
	int command = -1;
	keypad(win, true); // For arrow key presses
	int c = wgetch(win); // refreshes window as well
	switch(c)
	{	
		case KEY_UP :
			command = SELECT_PREVIOUS_OPTION;
			break;
		case KEY_DOWN :
			command = SELECT_NEXT_OPTION;
			break;
		case '\n' :
			command = EXECUTE_SELECTED_OPTION;
			break;
		case 'q' :
			command =  QUIT;
			break;
	}
	return command;
}

int main(int argc, char** argv)
{
/*	if (argc > 2)
	{
		long double sampleCount = string_to_long_double(argv[1]);
		long double sampleFreq = string_to_long_double(argv[2]);
		// Tested 100 samples at 1MHz
		generate_signal(sampleCount, sampleFreq);
	}
	else
	{
		printf("Missing function arguments\n");
	}*/

	initscr();	// Init screen, setup memory and clear screen
	cbreak();	// Unbuffered input, enables Ctrl + C to exit ncurses (default)
	noecho();	// User input is not echoed

	const char * edit_menu_items[] = EDIT_MENU_LIST;
	const int edit_menu_length = sizeof(edit_menu_items) / sizeof(edit_menu_items[0]);
	int row_max, column_max; // Functions to get line/column coords
	getmaxyx(stdscr, row_max, column_max);
	if(row_max < 0 || column_max < 0)
	{
		// PLACEHOLDER - TEST FOR MINIMUM SCREEN SIZE
		printf("Insufficient screen space in current console");
	}

	WINDOW * menu_win = newwin(
			MENU_WINDOW_LINES, 
			MENU_WINDOW_COLUMNS, 
			4,
			(column_max - MENU_WINDOW_COLUMNS) / 2); // height/width line/column
	WINDOW * edit_win = newwin(
			EDIT_WINDOW_LINES, 
			EDIT_WINDOW_COLUMNS,
		       	4 + MENU_WINDOW_LINES, 
		       	(column_max - MENU_WINDOW_COLUMNS) / 2);
	box(menu_win, 0, 0);
	box(edit_win, 0, 0);
	refresh(); // Screen level refresh
	curs_set(0); // Hide cursor

	wprintw(edit_win, "Edit window");
	mvwprintw(edit_win, 1, 1, "Line %d", 1);
	mvwprintw(edit_win, 2, 1, "Line %d", 2);
	wrefresh(edit_win);

	int selected_menu_option = 1;
	draw_menu(menu_win, edit_menu_items, edit_menu_length, selected_menu_option);
	while(1) {
		int next_state = get_input(menu_win);
		mvwprintw(menu_win, edit_menu_length + 1, 2, "                    ");
		switch(next_state)
		{
			case SELECT_PREVIOUS_OPTION :
				if (selected_menu_option > 0)
				{
					selected_menu_option--; 
				}
				draw_menu(menu_win, edit_menu_items, edit_menu_length, selected_menu_option);
				break;
			case SELECT_NEXT_OPTION :
				if (selected_menu_option < edit_menu_length - 1)
				{
					selected_menu_option++;
				}
				draw_menu(menu_win, edit_menu_items, edit_menu_length, selected_menu_option);
				break;
			case EXECUTE_SELECTED_OPTION :
				wmove(menu_win, edit_menu_length + 1, 2);
				wprintw(menu_win, "Pressed enter.");
				break;
			case QUIT :
				mvwprintw(menu_win, edit_menu_length + 1, 2, "Quitting...");
				endwin(); // Deallocate memory and end ncurses
				return 0;
				break;
			default :
				return -1;
				break;

		}
		wrefresh(menu_win);
	}
}	
