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

enum field_type {
	PATH_FIELD,
	NUMBER_FIELD,
	LIST_FIELD,
	OK_FIELD,
	CANCEL_FIELD
};

void validate_numeric(FORM *form)
{
	form_driver(form, REQ_VALIDATION); // Force validation to update buffer contents
	
	char *buffer_contents = field_buffer(current_field(form), 0);
	char *temp = (char *) calloc(strlen(buffer_contents), sizeof(char));				
	
	strip_whitespace(buffer_contents, temp);
	format_number_string(temp, NUMERIC_FIELD_REGEX, temp);
	set_field_buffer(current_field(form), 0, temp);
	
	free(temp);
}

struct Form form_setup(WINDOW *form_window)
{
	struct Form this_form;
	this_form.field_count = 5;

	this_form.fields = (FIELD **)calloc(this_form.field_count + 1, sizeof(FIELD *));

	this_form.fields[0] = new_field(1, 20, 0, 0, 0, 0);
	set_field_back(this_form.fields[0], A_UNDERLINE);
	field_opts_off(this_form.fields[0], O_STATIC);
	set_max_field(this_form.fields[0], 1024);

	this_form.fields[1] = dup_field(this_form.fields[0], 2, 0);
//	set_field_type(this_form.fields[1], TYPE_NUMERIC);

	this_form.fields[2] = new_field(1, 20, 4, 0, 0, 0);

	this_form.fields[3] = new_field(1, 8, 6, 0, 0, 0);
	
	this_form.fields[4] = dup_field(this_form.fields[3], 8, 0);
	this_form.fields[5] = NULL;

	for (int i = 0; i < this_form.field_count; i++)
	{
		field_opts_off(this_form.fields[i], O_AUTOSKIP | O_BLANK);
	}
	
	set_field_buffer(this_form.fields[2], 0, "Option 1");
	set_field_buffer(this_form.fields[3], 0, "   Ok   ");
	set_field_buffer(this_form.fields[4], 0, " Cancel ");

	int rows = 8;
	int cols = 28;
	this_form.form = new_form(this_form.fields);
	scale_form(this_form.form, &rows, &cols);
	set_form_win(this_form.form, form_window);
	set_form_sub(this_form.form, derwin(form_window, rows, cols, 2, 15));
	form_opts_off(this_form.form, O_BS_OVERLOAD); // Prevents backspace moving into previous field
	post_form(this_form.form);

	mvwprintw(form_window, 2, 4, "Path:"); // Highlight on select, cursor on active
	mvwprintw(form_window, 4, 4, "Number:"); // Highlight on select, cursor on active
	mvwprintw(form_window, 6, 4, "List:"); // Highlight on select
	mvwprintw(form_window, 8, 4, "Button:"); // Dim, highlight on select

// label, type, position

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
	curs_set(0);
}

void form_menu_driver(WINDOW* window, FORM *form, int c)
{
	int index;

	switch(c)
	{
		case KEY_DOWN :
			form_driver(form, REQ_NEXT_FIELD);
			form_highlight_active(form);
			break;
		case KEY_UP :
			form_driver(form, REQ_PREV_FIELD);
			form_highlight_active(form);
			break;
		case '\n' :
            index = field_index(current_field(form));
			switch(index)
			{
				case PATH_FIELD :
					form_driver(form, REQ_END_LINE);
                    update_field_text(window, form);
					break;
                case NUMBER_FIELD :
					form_driver(form, REQ_END_LINE);
                    update_field_text(window, form);
                    validate_numeric(form);
					break;
				case LIST_FIELD :
                    form_driver(form, REQ_END_LINE);
					update_field_text(window, form);
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
}
