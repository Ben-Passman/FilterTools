#ifndef MAIN_SETTINGS_FORM
#define MAIN_SETTINGS_FORM

/************************************************************************************************
 * FilterTools/main_settings_form.h
 * 
 * Author	: Ben Passman
 * Created	: 24/1/2021
 * 
 * Description	: Ncurses form layout and access functions for main settings
 *
 * Revision History:
 * Date         Author      Rev     Notes
 * 24/1/2021     Ben P       1.0     File created
 *
 ************************************************************************************************ */

#include "waveforms.h"
#include "form_handler.h"

struct Form main_settings_setup (WINDOW *window);
void set_main_settings_fields(const struct Form *main_settings, const struct WaveList *list);
void get_main_settings_fields(const struct Form *main_settings, struct WaveList *list);

#endif 
