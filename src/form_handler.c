/************************************************************************************************
 * FilterTools/form_handler.c
 * 
 * Author	: Ben Passman
 * Created	: 8/1/2021
 * 
 * Description	: Routines for handling ncurses forms
 *
 * Revision History:
 * Date         Author      Rev     Notes
 * 8/1/2021     Ben P       1.0     File created
 *
 ************************************************************************************************ */

#include "form_handler.h"
#include "input_validation.h"

#include <stdlib.h>
#include <string.h>

#define SCIENTIFIC_FIELD_REGEX "-?([0-9]*[.|,])?[0-9]+(([eE][+-]?[0-9]+)|[kMGTPEZY])?"

struct Dropdown
{
	int selected_item_index;
	const char **item_list;
	const int size;
};

static FIELDTYPE *FIELD_SCIENTIFIC = NULL;
static FIELDTYPE *FIELD_INDEX = NULL;

static const char *wave_types[] = { "Sine", "Cosine", "Sawtooth", "Triangle", "Square" };
static const char *modes[] = { "Add", "Subtract", "Multiply", "Divide", "AM", "FM" };
static struct Dropdown dropdown_lists[] = { 
	{ 0, &wave_types[0], sizeof wave_types / sizeof wave_types[0] },
	{ 0, &modes[0], sizeof modes / sizeof modes[0] }
}; 

static bool is_valid_number_field(FIELD *field, const void *p)
{
	char * contents = field_buffer(field, 0);
	char * temp = calloc(strlen(contents), sizeof(char));

	strip_whitespace(contents, temp);
	format_number_string(temp, SCIENTIFIC_FIELD_REGEX, temp);
	set_field_buffer(field, 0, temp);

	free(temp);
	return true;
}

static bool is_integer_char(int c, const void *p)
{
    return ((c >= '0') && (c <= '9')) ? true : false;
}

void init_form_handler(void)
{
    FIELD_SCIENTIFIC = new_fieldtype(&is_valid_number_field, NULL);
    FIELD_INDEX = new_fieldtype(NULL, &is_integer_char);
}

static void field_setup(FIELD **fields, enum FieldType *types, int size)
{
    for (int i = 0; i < size; i++)
	{
		field_opts_off(*(fields + i), O_AUTOSKIP | O_BLANK);
		switch (*(types + i))
		{
			case NUMBER_FIELD :
				set_field_type(*(fields + i), FIELD_SCIENTIFIC);
			case PATH_FIELD :
				field_opts_off(*(fields + i), O_STATIC);
				set_field_back(*(fields + i), A_UNDERLINE);
				set_max_field(*(fields + i), 1024);	
				break;
			case INDEX_FIELD :
				set_field_type(*(fields + i), FIELD_INDEX);
				break;
			default :
				break;			
		}
	}
}

static FORM *form_setup(WINDOW *window, FIELD **fields, int rows, int columns)
{
    FORM *form = new_form(fields);
	scale_form(form, &rows, &columns);
	set_form_win(form, window);
	set_form_sub(form, derwin(window, rows, columns, 2, 15));
	form_opts_off(form, O_BS_OVERLOAD); // Prevents backspace moving into previous field

	return form;
}

struct Form wave_settings_setup (WINDOW *window)
{
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
    
	set_field_userptr(settings.fields[0], (void *) &dropdown_lists[0]);
	set_field_userptr(settings.fields[5], (void *) &dropdown_lists[1]);
	set_field_buffer(settings.fields[7], 0, "   Ok   ");
	set_field_buffer(settings.fields[8], 0, " Cancel ");
    	
	field_setup(settings.fields, types, settings.field_count);
	settings.form = form_setup(window, settings.fields, 8, 28);

	return settings;
}

void form_highlight_active(FORM *form)
{
	for(int i = 0; i < form->maxfield; i++)
	{
		set_field_back(form->field[i], A_NORMAL);
	}
	set_field_back(current_field(form), A_REVERSE);
}

void update_field_text(WINDOW *window, FORM *form)
{
	int c = 0;
	curs_set(1);
	
	while((c= wgetch(window)) != '\n')
	{
		switch(c)
		{
			case KEY_HOME :
				form_driver(form, REQ_BEG_LINE);
				break;
			case KEY_END :
				form_driver(form, REQ_END_LINE);
				break;
			case KEY_LEFT :
				form_driver(form, REQ_PREV_CHAR);
				break;
			case KEY_RIGHT :
				form_driver(form, REQ_NEXT_CHAR);
				break;
			case KEY_BACKSPACE :
			case 127 :
				form_driver(form, REQ_DEL_PREV);
				break;
			case KEY_DC :
				form_driver(form, REQ_DEL_CHAR);
				break;
			default :
				form_driver(form, c);
				break;		
		}
	}
	form_driver(form, REQ_VALIDATION);
	curs_set(0);
}

void form_menu_driver(WINDOW* window, struct Form *menu, int c)
{
	int index;

	switch(c)
	{
		case KEY_LEFT :
			index = *(menu->field_types + field_index(current_field(menu->form)));
			if (index == LIST_FIELD)
			{
				struct Dropdown *list_contents = (struct Dropdown *) field_userptr(current_field(menu->form));
				if (list_contents->selected_item_index < 1)
				{
					list_contents->selected_item_index = list_contents->size;
				}
				list_contents->selected_item_index--;
				set_field_buffer(current_field(menu->form), 0, *(list_contents->item_list + list_contents->selected_item_index));
			}
			else if (index == CANCEL_FIELD)
			{
				form_driver(menu->form, REQ_PREV_FIELD);
				form_highlight_active(menu->form);
			}
			break;
		case KEY_RIGHT :
			index = *(menu->field_types + field_index(current_field(menu->form)));
			if (index == LIST_FIELD)
			{
				struct Dropdown *list_contents = (struct Dropdown *) field_userptr(current_field(menu->form));
				list_contents->selected_item_index++;
				if (list_contents->selected_item_index >= list_contents->size)
				{
					list_contents->selected_item_index = 0;
				}
				set_field_buffer(current_field(menu->form), 0, *(list_contents->item_list + list_contents->selected_item_index));
			}
			else if (index == OK_FIELD)
			{
				form_driver(menu->form, REQ_NEXT_FIELD);
				form_highlight_active(menu->form);
			}
			break;
		case KEY_DOWN :
			form_driver(menu->form, REQ_NEXT_FIELD);
			form_highlight_active(menu->form);
			break;
		case KEY_UP :
			form_driver(menu->form, REQ_PREV_FIELD);
			form_highlight_active(menu->form);
			break;
		case '\n' :
        		index = *(menu->field_types + field_index(current_field(menu->form)));
			switch(index)
			{
				case PATH_FIELD :
					form_driver(menu->form, REQ_END_LINE);
                			update_field_text(window, menu->form);
					break;
        		        case NUMBER_FIELD :
					form_driver(menu->form, REQ_END_LINE);
                			update_field_text(window, menu->form);
					break;
				case INDEX_FIELD :
					form_driver(menu->form, REQ_END_LINE);
					update_field_text(window, menu->form);
					break;
				case LIST_FIELD :
					break;
				case OK_FIELD :
				case CANCEL_FIELD :
					c = 'q';
					break;
			}
			break;
		default :
			break;
	}
}

void free_form_struct(struct Form form_struct)
{    
	unpost_form(form_struct.form);
	free_form(form_struct.form);
	for(int i = 0; i < form_struct.field_count + 1; i++)
	{
		free_field(form_struct.fields[i]);
	}
	free(form_struct.fields);
	free_fieldtype(FIELD_SCIENTIFIC);
	free_fieldtype(FIELD_INDEX);
}
