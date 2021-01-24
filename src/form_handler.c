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

#define SCIENTIFIC_FIELD_REGEX "-?([0-9]*[.|,])?[0-9]+(([eE][+-]?[0-9]+)|[yzafpnumkMGTPEZY])?"

// Custom types for field validation
static FIELDTYPE *FIELD_SCIENTIFIC = NULL;
static FIELDTYPE *FIELD_INDEX = NULL;

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

void init_custom_fields(void)
{
    FIELD_SCIENTIFIC = new_fieldtype(&is_valid_number_field, NULL);
    FIELD_INDEX = new_fieldtype(NULL, &is_integer_char);
}

// Setup functions
void field_setup(FIELD **fields, enum FieldType *types, int size)
{
    for (int i = 0; i < size; i++)
	{
		field_opts_off(*(fields + i), O_AUTOSKIP | O_BLANK);
		switch (*(types + i))
		{
            case PATH_FIELD :
                field_opts_off(*(fields + i), O_STATIC);
                set_max_field(*(fields + i), FORM_PATH_MAX);
                break;
			case NUMBER_FIELD :
				set_field_type(*(fields + i), FIELD_SCIENTIFIC);
				break;
			case INDEX_FIELD :
				set_field_type(*(fields + i), FIELD_INDEX);
				break;
			default :
				break;			
		}
	}
}

FORM *form_setup(WINDOW *window, FIELD **fields, int rows, int columns)
{
    FORM *form = new_form(fields);
	scale_form(form, &rows, &columns);
	set_form_win(form, window);
	set_form_sub(form, derwin(window, rows, columns, 2, 25));
	form_opts_off(form, O_BS_OVERLOAD); // Prevents backspace moving into previous field

	return form;
}

// Interface functions
static void update_field_text(WINDOW *window, FORM *form)
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

void form_highlight_active(FORM *form)
{
	for(int i = 0; i < form->maxfield; i++)
	{
		set_field_back(form->field[i], A_NORMAL);
	}
	set_field_back(current_field(form), A_REVERSE);
}

int form_menu_driver(WINDOW* window, struct Form *menu)
{
	int c;
	int field_type;
	while (1)
	{
		c = wgetch(window);
		switch(c)
		{
			case KEY_LEFT :
				field_type = *(menu->field_types + field_index(current_field(menu->form)));
				if (field_type == LIST_FIELD)
				{
					struct Dropdown *list = (struct Dropdown *) field_userptr(current_field(menu->form));
					if (list->index < 1)
					{
						list->index = list->size;
					}
					list->index--;
					set_field_buffer(current_field(menu->form), 0, *(list->item_list + list->index));
				}
				else if (field_type == CANCEL_FIELD)
				{
					form_driver(menu->form, REQ_PREV_FIELD);
					form_highlight_active(menu->form);
				}
				break;
			case KEY_RIGHT :
				field_type = *(menu->field_types + field_index(current_field(menu->form)));
				if (field_type == LIST_FIELD)
				{
					struct Dropdown *list = (struct Dropdown *) field_userptr(current_field(menu->form));
					list->index++;
					if (list->index >= list->size)
					{
						list->index = 0;
					}
					set_field_buffer(current_field(menu->form), 0, *(list->item_list + list->index));
				}
				else if (field_type == OK_FIELD)
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
            	field_type = *(menu->field_types + field_index(current_field(menu->form)));
				switch(field_type)
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
						return 1;
					case CANCEL_FIELD :
						return 0;
				}
				break;
			default :
				break;
		}
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
