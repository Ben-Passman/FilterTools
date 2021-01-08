#ifndef MENU_HANDLER
#define MENU_HANDLER

/************************************************************************************************
 * FilterTools/menu_handler.h
 * 
 * Author	: Ben Passman
 * Created	: 8/1/2021
 * 
 * Description	: Routines for handling ncurses menus
 *
 * Revision History:
 * Date         Author      Rev     Notes
 * 8/1/2021     Ben P       1.0     File created
 *
 ************************************************************************************************ */

#include <menu.h>

struct Menu {
	MENU *menu;
	ITEM **items;
	int item_count;
};

struct Menu main_menu_setup(WINDOW *menu_window);
void main_menu_driver(MENU *menu, int c);
void free_menu_struct(struct Menu menu_struct);

#endif
