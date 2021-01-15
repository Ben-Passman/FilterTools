/************************************************************************************************
 * FilterTools/ui_forms.c
 * 
 * Author	: Ben Passman
 * Created	: 16/1/2021
 * 
 * Description	: Form layouts and access functions
 *
 * Revision History:
 * Date         Author      Rev     Notes
 * 16/1/2021    Ben P       1.0     File created
 *
 ************************************************************************************************ */

#include "ui_forms.h"
#include <stdlib.h>

struct Form wave_settings_setup (WINDOW *window)
{
	static const char *wave_types[] = { "Sine", "Cosine", "Sawtooth", "Triangle", "Square" };
	static const char *modes[] = { "Add", "Subtract", "Multiply", "Divide", "AM", "FM" };
	static struct Dropdown settings_dropdowns[] = { 
		{ 0, &wave_types[0], sizeof wave_types / sizeof wave_types[0] },
		{ 0, &modes[0], sizeof modes / sizeof modes[0] }
	}; 
	
	mvwprintw(window, 2, 4, "Shape:");
	mvwprintw(window, 4, 4, "Amplitude:");
	mvwprintw(window, 6, 4, "Frequency:");
	mvwprintw(window, 6, 36, "Hz");
	mvwprintw(window, 8, 4, "Phase:");
	mvwprintw(window, 10, 4, "Duty:");
	mvwprintw(window, 12, 4, "Mode:");
	mvwprintw(window, 14, 4, "DC Offset:");
	
	static enum FieldType types[] = { 
		LIST_FIELD, 
		NUMBER_FIELD, 
		NUMBER_FIELD, 
		NUMBER_FIELD, 
		NUMBER_FIELD, 
		LIST_FIELD, 
		NUMBER_FIELD, 
		OK_FIELD, 
		CANCEL_FIELD 
	};
	
	struct Form settings;
	settings.field_count = 9;
	settings.fields = calloc(settings.field_count + 1, sizeof(FIELD *));
	settings.field_types = &types[0];
        
	settings.fields[0] = new_field(1, 20, 0, 0, 0, 0);
	settings.fields[1] = new_field(1, 20, 2, 0, 0, 0);
	settings.fields[2] = new_field(1, 20, 4, 0, 0, 0);
	settings.fields[3] = new_field(1, 20, 6, 0, 0, 0);
	settings.fields[4] = new_field(1, 20, 8, 0, 0, 0);
	settings.fields[5] = new_field(1, 20, 10, 0, 0, 0);
	settings.fields[6] = new_field(1, 20, 12, 0, 0, 0);
	settings.fields[7] = new_field(1, 8, 14, 0, 0, 0);
	settings.fields[8] = new_field(1, 8, 14, 10, 0, 0);
	settings.fields[9] = NULL;
    
	set_field_userptr(settings.fields[0], (void *) &settings_dropdowns[0]);
	set_field_userptr(settings.fields[5], (void *) &settings_dropdowns[1]);
	set_field_buffer(settings.fields[7], 0, "   Ok   ");
	set_field_buffer(settings.fields[8], 0, " Cancel ");
    	
	field_setup(settings.fields, types, settings.field_count);
	settings.form = form_setup(window, settings.fields, 8, 28);

	return settings;
}
