/************************************************************************************************
 * FilterTools/main_settings_form.c
 * 
 * Author	: Ben Passman
 * Created	: 24/1/2021
 * 
 * Description	: Ncurses form layout and access functions for main settings
 *
 * Revision History:
 * Date         Author      Rev     Notes
 * 24/1/2021    Ben P       1.0     File created
 *
 ************************************************************************************************ */

#include "wave_settings_form.h"
#include "input_validation.h"
#include <stdlib.h>
#include <string.h>

struct Form main_settings_setup (WINDOW *window)
{
	mvwprintw(window, 2, 4, "Export Path:");
	mvwprintw(window, 4, 4, "Sampling Frequency:");
	mvwprintw(window, 4, 36, "Hz");
    mvwprintw(window, 6, 4, "Number of Samples:");
	
	static enum FieldType types[] = { 
		PATH_FIELD, 
		NUMBER_FIELD, 
		INDEX_FIELD,
        OK_FIELD,
        CANCEL_FIELD
	};
	
	struct Form settings;
	settings.field_count = 6;
	settings.fields = calloc(settings.field_count + 1, sizeof(FIELD *));
	settings.field_types = &types[0];
        
	settings.fields[0] = new_field(1, 20, 0, 0, 0, 0);
	settings.fields[1] = new_field(1, 20, 2, 0, 0, 0);
	settings.fields[2] = new_field(1, 20, 4, 0, 0, 0);
	settings.fields[3] = new_field(1, 8, 6, 0, 0, 0);
	settings.fields[4] = new_field(1, 8, 6, 10, 0, 0);
	settings.fields[5] = NULL;
    
	set_field_buffer(settings.fields[3], 0, "   Ok   ");
	set_field_buffer(settings.fields[4], 0, " Cancel ");
    	
	field_setup(settings.fields, types, settings.field_count);
	settings.form = form_setup(window, settings.fields, 8, 28);

	return settings;
}

void set_main_settings_fields(const struct Form *main_settings, const struct WaveList *list)
{   
    char temp[21];
    set_field_buffer(main_settings->fields[0], 0, list->export_path);
    sprintf(temp, "%e", list->sample_frequency);
    set_field_buffer(main_settings->fields[1], 0, temp);
    sprintf(temp, "%d", list->sample_count);
    set_field_buffer(main_settings->fields[2], 0, temp);
}

void get_main_settings_fields(const struct Form *main_settings, struct WaveList *list)
{
    char temp[21];
    strcpy(list->export_path, field_buffer(main_settings->fields[0], 0));
    strip_whitespace(field_buffer(main_settings->fields[1], 0), temp);
    list->sample_frequency = ASCII_string_to_double(temp);
    strip_whitespace(field_buffer(main_settings->fields[2], 0), temp);
    list->sample_count = ASCII_string_to_double(temp);
}
