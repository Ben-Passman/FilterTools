#ifndef MAIN_MENU
#define MAIN_MENU

/********************************************************************************************************************************
 * FilterTools/main_menu.h
 *
 * Author	: Ben Passman
 * Created	: 25/1/2021
 *
 * Description	: Main window controls
 *
 * Revision History:
 * Date		Author		Rev	Notes
 * 25/1/2021	Ben P		1.0	Created header file.
 *
 * */

extern const char *const wave_types[];
extern const char *const wave_modes[];

#define OUTPUT_WINDOW_SIZE 20, 80
#define OUTPUT_WINDOW_LOCATION 13, (COLS - 80) / 2

#define POPUP_WINDOW_SIZE 20, 60
#define POPUP_WINDOW_LOCATION 10, (COLS - 60) / 2

void initialise_ncurses();
void initialise_windows();
void initialise_panels();
void initialise_forms();
void main_menu_refresh();
int main_menu_driver();
void free_ncurses();

#endif
