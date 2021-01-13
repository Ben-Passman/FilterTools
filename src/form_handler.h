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

enum FieldType { LABEL_FIELD, PATH_FIELD, INDEX_FIELD, NUMBER_FIELD, LIST_FIELD, OK_FIELD, CANCEL_FIELD };

struct Form {
	FORM *form;
	FIELD **fields;
    enum FieldType *field_types;
	int field_count;
};

struct FormTemplate {
	enum FieldType type;
	int row_size;
	int column_size;
	int row;
	int column;
	char * text;
};

void validate_numeric(FORM *form);
void init_form_handler(void);
struct Form wave_settings_setup (WINDOW *window);
void form_highlight_active(FORM *form);
void update_field_text(WINDOW *window, FORM *form);
void form_menu_driver(WINDOW *window, struct Form *form, int c);
void free_form_struct(struct Form form_struct);

#endif
