/************************************************************************************************
 * FilterTools/menu_handler.c
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

#include "menu_handler.h"

#include <stdlib.h>

#define MAIN_MENU_OPTIONS { \
	"Edit settings", \
	"Add new wave", \
	"Modify existing wave", \
	"Delete wave", \
	"Load wave set", \
	"Save wave set", \
	"Export combined waveform" \
}

struct Menu main_menu_setup(WINDOW *menu_window)
{
	struct Menu main_menu;
	const char *main_menu_options[] = MAIN_MENU_OPTIONS;
	main_menu.item_count = sizeof(main_menu_options) / sizeof(main_menu_options[0]);

	main_menu.items = (ITEM **) calloc(main_menu.item_count + 1, sizeof(ITEM *));

	for(int i = 0; i < main_menu.item_count; ++i)
	{
		main_menu.items[i] = new_item(main_menu_options[i], "");
	}
	main_menu.items[main_menu.item_count] = (ITEM *)NULL;

	main_menu.menu = new_menu((ITEM **) main_menu.items);
	set_menu_win(main_menu.menu, menu_window);
	set_menu_sub(main_menu.menu, derwin(menu_window, 0, 0, 1, 1));
	set_menu_mark(main_menu.menu, " * ");
	
	post_menu(main_menu.menu);
	
	return main_menu;
}

void main_menu_driver(MENU *menu, int c)
{
    switch(c)
	{
		case KEY_DOWN :
			menu_driver(menu, REQ_DOWN_ITEM);
			break;
		case KEY_UP :
			menu_driver(menu, REQ_UP_ITEM);
			break;
		case '\n' :
			
			break;
	}
}

void free_menu_struct(struct Menu menu_struct)
{
	unpost_menu(menu_struct.menu);
	free_menu(menu_struct.menu);
	for(int i =0; i < menu_struct.item_count + 1; i++)
	{
		free_item(menu_struct.items[i]);
	}
	free(menu_struct.items);
}
