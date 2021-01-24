#ifndef WAVE_SETTINGS_FORM
#define WAVE_SETTINGS_FORM

/************************************************************************************************
 * FilterTools/wave_settings_form.h
 * 
 * Author	: Ben Passman
 * Created	: 16/1/2021
 * 
 * Description	: Ncurses form layout and access functions for wave settings
 *
 * Revision History:
 * Date         Author      Rev     Notes
 * 16/1/2021     Ben P       1.0     File created
 *
 ************************************************************************************************ */

#include "waveforms.h"
#include "form_handler.h"

struct Form wave_settings_setup (WINDOW *window);
void set_wave_fields(const struct Form *wave_settings, const struct WaveForm *wave);
void get_wave_fields(const struct Form *form, struct WaveForm *wave);
void print_waves(WINDOW *window, struct WaveList *wlist);

#endif 
