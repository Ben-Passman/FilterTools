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

#define NUMERIC_FIELD_REGEX "-?([0-9]*[.|,])?[0-9]+(([eE][+-]?[0-9]+)|[kMGTPEZY])?"

static FIELDTYPE *FIELD_CUSTOM = NULL;

static bool is_valid_number_field(FIELD *field, const void *p)
{
	char * contents = field_buffer(field, 0);
	char * temp = (char *) calloc(strlen(contents), sizeof(char));

	strip_whitespace(contents, temp);
	format_number_string(temp, NUMERIC_FIELD_REGEX, temp);
	set_field_buffer(field, 0, temp);

	free(temp);
	return true;
}

static bool custom_char_validation(int c, const void *p)
{
    return ((c > '2') && (c < '9')) ? true : false;
}

void init_form_handler(void)
{
    //FIELD_CUSTOM = new_fieldtype(&is_valid_number_field, &custom_char_validation);
    FIELD_CUSTOM = new_fieldtype(&is_valid_number_field, NULL);
}

struct Form form_setup(WINDOW *form_window, struct FormTemplate *field_list, int size)
{
	struct Form this_form;
	this_form.field_count = size;
	
	for (int i = 0; i < size; i++)
	{
		struct FormTemplate *f = (field_list + i);
		if (f->type == LABEL_FIELD)
		{
			mvwprintw(form_window, f->row, f->column, f->text);
			this_form.field_count--;
		}
	}
	
	this_form.fields = (FIELD **) calloc(this_form.field_count + 1, sizeof(FIELD *));
	this_form.field_types = (enum FieldType *) calloc(this_form.field_count + 1, sizeof(enum FieldType));

	int field_index = 0;
	for (int i = 0; i < size; i++)
	{
		struct FormTemplate *f = (field_list + i);
		*(this_form.field_types + field_index) = f->type;
		if (f->type != LABEL_FIELD)
		{
 			this_form.fields[field_index] = new_field(f->row_size, f->column_size, f->row, f->column, 0, 0);
			field_opts_off(this_form.fields[field_index], O_AUTOSKIP | O_BLANK);
			set_field_buffer(this_form.fields[field_index], 0, f->text);
			
			switch (f->type)
			{
				case NUMBER_FIELD :
					set_field_type(this_form.fields[field_index], FIELD_CUSTOM);
				case PATH_FIELD :
					field_opts_off(this_form.fields[field_index], O_STATIC);
					set_field_back(this_form.fields[field_index], A_UNDERLINE);
					set_max_field(this_form.fields[field_index], 1024);	
					break;
				case LIST_FIELD :
					break;
				case OK_FIELD :
				case CANCEL_FIELD :
					break;
				default :
					break;			
			}
			
			field_index++;
		}

	}
	this_form.fields[field_index] = NULL;
	
	int rows = 8;
	int cols = 28;
	this_form.form = new_form(this_form.fields);
	scale_form(this_form.form, &rows, &cols);
	set_form_win(this_form.form, form_window);
	set_form_sub(this_form.form, derwin(form_window, rows, cols, 2, 15));
	form_opts_off(this_form.form, O_BS_OVERLOAD); // Prevents backspace moving into previous field
	post_form(this_form.form);

	return this_form;
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
	//FIELD * active_field = current_field(form);
	int c = 0;
	curs_set(1);
	
	while((c= wgetch(window)) != '\n')
	{
		switch(c)
		{
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
				case LIST_FIELD :
                			form_driver(menu->form, REQ_END_LINE);
					update_field_text(window, menu->form);
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
	free(form_struct.field_types);
    free_fieldtype(FIELD_CUSTOM);
}
