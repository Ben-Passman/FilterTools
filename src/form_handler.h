#ifndef FORM_HANDLER
#define FORM_HANDLER

/************************************************************************************************
 * FilterTools/form_handler.h
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

#include <form.h>

struct Form {
	FORM *form;
	FIELD **fields;
	int field_count;
};

void validate_numeric(FORM *form);
struct Form form_setup(WINDOW *form_window);
void form_highlight_active(FORM *form);
void update_field_text(WINDOW *window, FORM *form);
void form_menu_driver(WINDOW *window, FORM *form, int c);
void free_form_struct(struct Form form_struct);

#endif
