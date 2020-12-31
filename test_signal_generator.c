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
#include "ASCII_parser.h"

#include<errno.h>
#include<math.h>
#include<stdlib.h>

#include<menu.h>
#include<form.h>
#include<panel.h>

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
	curs_set(0);	// Hide cursor

	MENU *main_menu;
	ITEM **main_menu_items;
	WINDOW * menu_window = newwin(
		MAIN_MENU_WINDOW_LINES,
		MAIN_MENU_WINDOW_COLUMNS,
		2,
		(COLS - MAIN_MENU_WINDOW_COLUMNS) / 2
	);
	const char * main_menu_options[] = MAIN_MENU_OPTIONS;
	const int main_menu_item_count = sizeof(main_menu_options) / sizeof(main_menu_options[0]);;
	

	main_menu_items = (ITEM **)calloc(main_menu_item_count + 1, sizeof(ITEM *));
	for(int i = 0; i < main_menu_item_count; ++i)
	{
		main_menu_items[i] = new_item(main_menu_options[i], "");
	}
	main_menu_items[main_menu_item_count] = (ITEM *)NULL;

	main_menu = new_menu((ITEM **)main_menu_items);
	set_menu_win(main_menu, menu_window);
	set_menu_sub(main_menu, derwin(menu_window, 0, 0, 1, 1));
	set_menu_mark(main_menu, " * ");
	
	post_menu(main_menu);
	box(menu_window, 0, 0);
	wrefresh(menu_window);

	WINDOW * output_window = newwin(
			EDIT_WINDOW_LINES, 
			EDIT_WINDOW_COLUMNS,
		       	4 + MAIN_MENU_WINDOW_LINES, 
		       	(COLS - MAIN_MENU_WINDOW_COLUMNS) / 2);
	box(output_window, 0, 0);	
	wrefresh(output_window);

	// Debug
	mvwprintw(output_window, 1, 2, "Sampling frequency: 555 kHz");
	mvwprintw(output_window, 2, 2, "Sample count:       1000");
	mvwprintw(output_window, 4, 2, "Shape: Amplitude: Phase: Freq: Duty: Mode:");
	wnoutrefresh(output_window);
	
	int c;
	keypad(menu_window, TRUE);
	while((c = wgetch(menu_window)) != 'q')
	{
		switch(c)
		{
			case KEY_DOWN :
				menu_driver(main_menu, REQ_DOWN_ITEM);
				break;
			case KEY_UP :
				menu_driver(main_menu, REQ_UP_ITEM);
				break;
			case '\n' :
				break;
		}
		// Debug
		mvwprintw(output_window, 5, 2, "Main menu size: %d", main_menu_item_count);
		mvwprintw(output_window, 6, 2, "Selected option: %d", item_index(current_item(main_menu)));
		wnoutrefresh(output_window);
		doupdate();

	}

	unpost_menu(main_menu);
	free_menu(main_menu);
	for(int i =0; i < main_menu_item_count; i++)
		free_item(main_menu_items[i]);
	endwin();
	return 1;
}	
