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
	
	this_form.fields = calloc(this_form.field_count + 1, sizeof(FIELD *));
	this_form.field_types = calloc(this_form.field_count + 1, sizeof(enum FieldType));

	int field_index = 0;
	int list_index;
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
					set_field_type(this_form.fields[field_index], FIELD_SCIENTIFIC);
				case PATH_FIELD :
					field_opts_off(this_form.fields[field_index], O_STATIC);
					set_field_back(this_form.fields[field_index], A_UNDERLINE);
					set_max_field(this_form.fields[field_index], 1024);	
					break;
				case LIST_FIELD :
					list_index = (int) (*f->text - '0');
					set_field_userptr(this_form.fields[field_index], (void *) &dropdown_lists[list_index]);
					set_field_buffer(this_form.fields[field_index], 0, *dropdown_lists[list_index].item_list);
					break;
				case INDEX_FIELD :
					set_field_type(this_form.fields[field_index], FIELD_INDEX);
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
//	post_form(this_form.form);

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
	free(form_struct.field_types);
	free_fieldtype(FIELD_SCIENTIFIC);
	free_fieldtype(FIELD_INDEX);
}
