// DLG.c
//
//  the main dialog file
//
//   Main editor dialog, menus & valve functions 
//
//  September, 1998
//  jerry@mail.csh.rit.edu

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "allegro.h"
#include "general.h"
#include "snap.h"
#include "util.h"
#include "bitmaped.h"
#include "animview.h"
#include "guipal.h"
#include "button.h"
#include "sprtplte.h"
#include "palette.h"
#include "sprite.h"
#include "editmenu.h"
#include "toolmenu.h"
#include "editmode.h"
#include "inidriv.h"
#include "gamedesc.h"
#include "fileio.h"
#include "maped.h"
#include "texted.h"
#include "help.h"
#include "drivsel.h" // for driver dirs
#include "config.h" // for driver dirs
#include "life.h"
#include "animate.h"

#define  SCANCODE_TO_KEY scancode_to_name // GN:

BITMAP *current_sprite = NULL;


// extern SPRITE_PALETTE * GfxBanks;
// extern int NumGfxBanks;
//SPRITE_PALETTE * sprite_bank = NULL; // the sprite being used in the display


int stub(void) { return D_O_K; }


//
///  The Menus...
//

MENU file_menu[] =
{
    {"&Change Game", 	file_game, NULL, 0, NULL},
    {"", 		NULL, NULL, 0, NULL},
    {"&Save Graphics" ,	file_save_gfx, NULL, 0, NULL},
    {"&Revert ",	file_revert, NULL, 0, NULL},
    {"", 		NULL, NULL, 0, NULL},
    {"&Generate Patch", file_genpatch, NULL, 0, NULL},
    {"&Apply Patch",	file_applypatch, NULL, 0, NULL},
    {"", 		NULL, NULL, 0, NULL},
    {"Sav&e C Source" ,	file_save_c_source, NULL, 0, NULL},
    {"", 		NULL, NULL, 0, NULL},
    {"E&xit", 		file_exit, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL},
};

MENU edit_menu[] =
{
//    {"&Undo", 		edit_undo, NULL, D_DISABLED, NULL},
//    {"&Redo", 		edit_redo, NULL, D_DISABLED, NULL},
//    {"", 		NULL, NULL, 0, NULL},
    {"&Copy sprite", 	edit_copy_sprite, NULL, 0, NULL},
    {"&Paste sprite", 	edit_paste_sprite, NULL, 0, NULL},
    {"", 		NULL, NULL, 0, NULL},
    {"&Save Bank PCX", 	edit_save_pcx, NULL, 0, NULL},
    {"&Load From PCX", 	edit_load_pcx, NULL, 0, NULL},
    {"", 		NULL, NULL, 0, NULL},
//    {"&Map Editor", 	editors_map, NULL, D_DISABLED, NULL},
//    {"&Text Editor", 	editors_text, NULL, D_DISABLED, NULL},
//    {"", 		NULL, NULL, 0, NULL},
    {"&Add Palette", 	palette_add_new, NULL, 0, NULL},
    {"&Edit Palette", 	palette_edit, NULL, 0, NULL},
    {"", 		NULL, NULL, 0, NULL},
    {"Pre&ferences", 	edit_preferences, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL},
};

MENU mode_menu[] =
{
    {"&p Paint",         edit_mode_paint, NULL, 0, NULL},
    {"&f Flood Fill",    edit_mode_flood, NULL, 0, NULL},
    {"&e Eyedrop", 	 edit_mode_eye, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL},
};


MENU help_menu[] =
{
    {"&Readme",	      help_general, NULL, 0, NULL},
    {"&What's New",   help_new, NULL, 0, NULL},
    {"&About...",     help_about, NULL, 0, NULL},
    {NULL, NULL, NULL, 0, NULL},
};

MENU main_menus[] =
{
    {"&File", 		NULL, file_menu,  0, NULL},
    {"&Edit", 		NULL, edit_menu,  0, NULL},
    {"&Mode",		NULL, mode_menu,  0, NULL},
    {"&Sprite",		NULL, tools_menu, 0, NULL},
    {"&Help", 		NULL, help_menu,  0, NULL},
    {NULL, NULL, NULL, 0, NULL},
};


////////////////////////////////////////////////////////////////////////////////
//
// "We're sorry we misinterpreted your running away as an attack."
//
////////////////////////////////////////////////////////////////////////////////
void DisplayGameDescription(int x, int y)
{
    char TruncatedName[50];

    // does text fit in area provided - if not then truncate it and add elipses
    if (strlen(GameDescription) > 22)
    {
	strncpy(TruncatedName, GameDescription, 20);
	TruncatedName[20] = '\0';
	strcat(TruncatedName, "...");
	textout(screen, font, TruncatedName, x, y, GUI_FORE);
    } else {
	// otherwise just display it.
	textout(screen, font, GameDescription, x, y, GUI_FORE);
    }
    // thanks ivan.. i missed that one.  :]
}

extern int timer_ticks;
int last_timer = 0;


int screen_text(int msg, DIALOG *d, int c)
{
    static int mouse_over_old = -1;
    static int cursor_over_old = -1;

    if (msg == MSG_IDLE)
    {
	// for lack of a better place to put this for now... 
	// don't like this? eat it.
	if (timer_ticks != last_timer)
	{
	    last_timer = timer_ticks;
	    animate_step();
	}

	if (GameDriverLoaded && NumGfxBanks)
	    if (GfxBanks[currentGfxBank].mouse_over != mouse_over_old
	       || GfxBanks[currentGfxBank].last_selected != cursor_over_old)
	    {
		cursor_over_old = GfxBanks[currentGfxBank].last_selected;
		mouse_over_old = GfxBanks[currentGfxBank].mouse_over;
		show_mouse(NULL);
		(void)screen_text(MSG_DRAW, d, c);

		main_dialog[11].proc(MSG_DRAW, &main_dialog[11], c);

		show_mouse(screen);
	    }

    }

    if (msg == MSG_DRAW)
    {
	// draw up all of the text onto the screen...
	text_mode(GUI_BACK);

	// number of banks
	if(GameDriverLoaded && NumGfxBanks)
	    textprintf_centre(screen, font, 265, 129, GUI_FORE, 
				"%02d/%02d", currentGfxBank+1, NumGfxBanks);
	else
	    textout_centre(screen, font, "--/--", 265, 129, GUI_FORE);

	border_3d(screen, 240, 125, 50, 15, 0);

        // number of palettes
        if (GameDriverLoaded && GfxBankExtraInfo)
        {
	    textprintf_centre(screen, font, 189, 74, GUI_FORE,
				"%d", current_palette_number +1);
	    textprintf_centre(screen, font, 189, 94, GUI_FORE, "%d", 
		NumColPalettes[GfxBankExtraInfo[currentGfxBank].planes] );
	} else {
	    textout_centre(screen, font, "--", 189, 74, GUI_FORE);
	    textout_centre(screen, font, "--", 189, 94, GUI_FORE);
	}
	line(screen, 181, 87, 196, 87, GUI_FORE);
	border_3d(screen, 179, 70, 20, 34, 0);

	// current sprite number
	if (GameDriverLoaded && NumGfxBanks)
	{
	    if (GfxBanks[currentGfxBank].mouse_over < 
		    GfxBanks[currentGfxBank].n_total)
	        textprintf_centre(screen, font, 290, 175, GUI_FORE,
			     "[0x%03x]",GfxBanks[currentGfxBank].mouse_over);
	    else if (GfxBanks[currentGfxBank].last_selected >= 0)
	        textprintf_centre(screen, font, 290, 175, GUI_FORE,
			     " 0x%03x ",GfxBanks[currentGfxBank].last_selected);
	    else
		textout_centre(screen, font, " 0x??? ", 290, 175, GUI_FORE);

	    textprintf_centre(screen, font, 290, 195, GUI_FORE,
			 "0x%03x",GfxBanks[currentGfxBank].n_total-1);

	} else {
	    textout_centre(screen, font, "0x---", 290, 175, GUI_FORE);
	    textout_centre(screen, font, "0x---", 290, 195, GUI_FORE);
	}
	line(screen, 269, 187, 310, 187, GUI_FORE);


	if (strlen(GameDescription))
	    DisplayGameDescription(4, 129);
	else
	    textout(screen, font, "No game loaded.", 4, 129, GUI_FORE);
	border_3d(screen, 0, 125, 205, 15, 0);
    }

    return D_O_K;
}

void try_loading_cmd_line_driver(void)
{
    char tstring[255];
    char * filename;
    int pos;
    int old_drv_subdirs;

    // try loading the game as described on the command line 
    // if there was one
    if (strlen(command_line_driver))
    {
        // strip the path
        filename = get_filename(command_line_driver);

        // strip the extension
        for (pos=0 ; pos < strlen(filename) ; pos++)
	    if (filename[pos] == '.') filename[pos] = '\0';
       
	sprintf(tstring, "expdriv/%s.ini", filename);

	if (exists(tstring))
	{
	    try_loading_the_driver(tstring);
	} else {
	    sprintf(tstring, "drivers/%s.ini", filename);
	    if (exists(tstring))
	    {
		try_loading_the_driver(tstring);
            } else {
		// now we'll try to check other subdirectories, 
		// without checking the ini file settings for subdirs.
		// that switch is just for the file selector...
                old_drv_subdirs = drv_subdirs;
                drv_subdirs = 1;
		create_dir_list();
		for (pos = 0 ; pos < ndirs ; pos++)
		{
		    sprintf(tstring, "DRIVERS/%s/%s.ini", dirlist[pos], filename);
		    if (exists(tstring))
		    {
			try_loading_the_driver(tstring);
			break;
		    }
		}
		destroy_dir_list();
                drv_subdirs = old_drv_subdirs;
            }
	}
    }
}


int oneshot(int msg, DIALOG *d, int c)
{
    static BOOL first_time = TRUE; // need it to be static...

    // this is a kludge and a half, but we want this to be done
    // the first time available once the dialog has been drawn for 
    // the first time.

    // That means that we need to do it at the first idle time...

    if (msg == MSG_IDLE)
    {
        if (first_time)
        {
	    first_time = FALSE;

            // put all the stuff that needs to be done only once in here...

	    /*
	    alert("EXPERIMENTAL VERSION!", "USE AT YOUR OWN RISK!", "", 
		  "Got it!", NULL, 0, 0);
	    */

	    busy();
	    try_loading_cmd_line_driver();
	    not_busy();
	    if (GameDriverLoaded)
		return D_REDRAW;
	}
    }

    return D_O_K;
}

int my_kyb(int msg, DIALOG *d, int c)
{

    if (msg == MSG_START)
    {
	// create a dummy bitmap for the display...
	current_sprite = create_bitmap(ED_DEF_SIZE, ED_DEF_SIZE);
	clear_to_color(current_sprite, FIRST_USER_COLOR);
	/*
	line(current_sprite, 0, 0, 
	     current_sprite->w-1, current_sprite->h-1, 
	     FIRST_USER_COLOR+1);
	line(current_sprite, 0, 
	     current_sprite->h-1, current_sprite->w-1, 0, 
	     FIRST_USER_COLOR+1);
	*/

    }


    if (msg == MSG_END)
    {
	destroy_bitmap(current_sprite);
    }

    if (msg == MSG_XCHAR)
    {
	// sponges
	if ((c&0xff) == 27) { // escape
	    return D_USED_CHAR;
	} 

	if((SCANCODE_TO_KEY(KEY_F5)) == c)
	    return editors_map();

	if((SCANCODE_TO_KEY(KEY_F6)) == c)
	    return editors_text();

	if((SCANCODE_TO_KEY(KEY_F7)) == c)
	    return animate_force();

	if((SCANCODE_TO_KEY(KEY_F8)) == c)
	    return animate_toggle();

	if((SCANCODE_TO_KEY(KEY_F9)) == c)
	    return edit_save_pcx();

	if((SCANCODE_TO_KEY(KEY_F10)) == c)
	    return edit_load_pcx();

	if((SCANCODE_TO_KEY(KEY_F11)) == c)
	{
	    life_counter=0l;

	    show_mouse(NULL);
	    text_mode(GUI_BACK);
	    textprintf(screen, font, 261, 4, GUI_FORE, 
		       "%07ld ", life_counter);
	    show_mouse(screen);
	}

	if((SCANCODE_TO_KEY(KEY_F12)) == c)
	{
	    if (do_life() == D_REDRAW)
	    {
		show_mouse(NULL);
		main_dialog[2].proc(MSG_DRAW, &main_dialog[2], 0);

		text_mode(GUI_BACK);
		textprintf(screen, font, 261, 4, GUI_FORE, 
			   "%07ld ", life_counter);

		show_mouse(screen);
	    }
	}

	if((SCANCODE_TO_KEY(KEY_PGDN)) == c)
	    return sprite_pal_minus(NULL);

	if((SCANCODE_TO_KEY(KEY_PGUP)) == c)
	    return sprite_pal_plus(NULL);

	if((SCANCODE_TO_KEY(KEY_HOME)) == c)
	    return sprite_pal_home(NULL);

	// other keys
	if (c>>8 == KEY_P) { // alt-p
	    screen_snap();
	}

	if (c>>8 == KEY_A)  // alt-a
	    return animate_set_point();

	if (c>>8 == KEY_Z)  // alt-z
	    return animate_change_playmode();

    }
    return D_O_K;
}

////////////////////////////////////////////////////////////////////////////////
//
// "The cops in this adventure hang out at Dungeon Doughnuts."
//
////////////////////////////////////////////////////////////////////////////////

int slidcall(void *dp3, int d2)
{
    char te[64];

    sprintf(te, "%3d", d2);
    textout(screen, font, te, 200, 100, 2);
    return D_O_K;
}

void bmed_callback(DIALOG * d, int x, int y, int mb)
{
    BITMAP ** temp1;
    BITMAP * temp2;

    temp1 = d->dp;  // pointer to the bitmap to draw
    temp2 = *temp1; // this is the bitmap to draw

    if (temp2 == NULL) return;

    switch (edit_mode)
    {
    case(MODE_PAINT):
	if(mb&MOUSE_FLAG_LEFT_DOWN)
	    do_tool(TOOLS_PAINT_BG, temp2, x, y);
	else
	    do_tool(TOOLS_PAINT_FG, temp2, x, y);
	break;

    case(MODE_FLOODFILL):
	if(mb&MOUSE_FLAG_LEFT_DOWN)
	    do_tool(TOOLS_FLOOD_FILL_BG, temp2, x, y);
	else
	    do_tool(TOOLS_FLOOD_FILL_FG, temp2, x, y);
	break;

    case(MODE_EYEDROP):
	if(mb&MOUSE_FLAG_LEFT_DOWN)
	    do_tool(TOOLS_EYEDROPPER_BG, temp2, x, y);
	else
	    do_tool(TOOLS_EYEDROPPER_FG, temp2, x, y);
	set_mouse_edit_mode(edit_mode_old);

// why is this here?
//	show_mouse(NULL);
//	main_dialog[3].proc(MSG_DRAW, &main_dialog[3], 0);
//	show_mouse(screen);
	break;
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// "It's hot."  "Not for pizza."  "No. Not for pizza."
//
////////////////////////////////////////////////////////////////////////////////

// d1 - range, d2-value , dp - bitmap, dp2 -callback
DIALOG main_dialog[] =
{
   /* (dialog proc)     (x)   (y)   (w)   (h)   (fg)  (bg)  
   		(key) (flags)  (d1)  (d2)  (dp) */

   /// base dialog stuff 
   { d_clear_proc,      0, 0, 0, 0, GUI_FORE, GUI_BACK,    
   		0, 0, 0, 0, NULL },
   { d_menu_proc,	0, 0, 0, 0, GUI_FORE, GUI_BACK, 
   		0, 0, 0, 0, main_menus }, 

   /// bitmap editing stuff
   { bitmap_editor_proc, 6, 19, 80+4, 80+4, GUI_FORE, GUI_BACK, 
   		0, 0, 95, 19, &current_sprite, bmed_callback }, 

   /// sprite put routine...
   /*
   { button_dp2_proc,    6, 106, 84, 16, GUI_FORE, GUI_BACK,
   		0, D_EXIT,  0, 0, "\\/ Back \\/",  sprite_return },
   
   { button_dp2_proc,    6, 106, 41, 16, GUI_FORE, GUI_BACK,
   		0, D_EXIT,  0, 0, "Back",  sprite_return },
   { button_dp2_proc,    48, 106, 42, 16, GUI_FORE, GUI_BACK,
   		0, D_EXIT,  0, 0, "Edit",  sprite_capture },
   */
   { button_dp2_proc,    6, 105, 41, 16, GUI_FORE, GUI_BACK,
   		0, D_EXIT,  0, 0, "Edit",  sprite_capture },
   { button_dp2_proc,    48, 107, 42, 16, GUI_FORE, GUI_BACK,
   		0, D_EXIT,  0, 0, "Back",  sprite_return },

   /// palette display and selection
   { pal_display_proc, 93, 55+10, 20, (4*15)+4-20, GUI_FORE, GUI_BACK, 
   		0, 0, 0, 0, &pal_fg_color, &pal_bg_color},
   { pal_select_proc,  115, 55, (4*15)+4, (4*15)+4, GUI_FORE, GUI_BACK, 
   		0, 0, 0, 0, &psel, psel_callback }, 
   { button_dp2_proc,    115+(4*15)+4, 55, 20, 15,
   		GUI_FORE, GUI_BACK,  0, D_EXIT,  0, 0, "+",  pal_plus },
   { button_dp2_proc,    115+(4*15)+4, 55+((4*15)+4)-15, 20, 15,
   		GUI_FORE, GUI_BACK,  0, D_EXIT,  0, 0, "-",  pal_minus },

   /// select bank for the sprite palette...
   { button_dp2_proc,    210, 125, 30, 15, GUI_FORE, GUI_BACK,
   		0, D_EXIT,  0, 0, "<<",  sprite_bank_minus },
   { button_dp2_proc,    290, 125, 30, 15, GUI_FORE, GUI_BACK,
   		0, D_EXIT,  0, 0, ">>",  sprite_bank_plus },

   /// sprite selector
   { sprite_palette_proc, 0, 140, (8*32)+4, 100, GUI_FORE, GUI_BACK, 
   		0, 0, 0, 0, &sprite_bank, main_sprtplte_callback },
   { button_dp2_proc,     260, 140,  15, 15,  GUI_FORE, GUI_BACK,  
   		0, D_EXIT,  0, 0, "^", sprite_pal_plus },
   { button_dp2_proc,     260, 225,  15, 15,  GUI_FORE, GUI_BACK,  
   		0, D_EXIT,  0, 0, "v", sprite_pal_minus },

#if 0 // GN: broken
    /// bitmap animation stuff
    { animview_proc, 320-6-84, 19, 80+2, 80+2, GUI_FORE, GUI_BACK, 
   		0, 0, -1, 0}, 
#endif
    { button_dp2_proc,    320-6-84-15-1, 19+80+1-30,  
                          15, 15, GUI_FORE, GUI_BACK,  
   		          0, D_EXIT,  0, 0, "P", animate_change_playmode },
    { button_dp2_proc,    320-6-84-15-1, 19+80+1-15,  
                          15, 15, GUI_FORE, GUI_BACK,  
   		          0, D_EXIT,  0, 0, "A", animate_set_point },

    // now some keyboard shortcuts...
    { d_keyboard_proc, 0, 0, 0, 0, 0, 0,  '.', 0, 0, 0, edit_mode_paint }, 
    { d_keyboard_proc, 0, 0, 0, 0, 0, 0,  'f', 0, 0, 0, edit_mode_flood }, 
    { d_keyboard_proc, 0, 0, 0, 0, 0, 0,  ',', 0, 0, 0, edit_mode_eye }, 

    { d_keyboard_proc, 0, 0, 0, 0, 0, 0,  '<', 0, 0, 0, palette_dec }, 
    { d_keyboard_proc, 0, 0, 0, 0, 0, 0,  '>', 0, 0, 0, palette_inc }, 
    { d_keyboard_proc, 0, 0, 0, 0, 0, 0,  '[', 0, 0, 0, bank_dec }, 
    { d_keyboard_proc, 0, 0, 0, 0, 0, 0,  ']', 0, 0, 0, bank_inc }, 

    { my_kyb, 0, 0, 0, 0, GUI_FORE, GUI_BACK, 0, 0, 0, 0, NULL },
    { screen_text, 0, 0, 0, 0, GUI_FORE, GUI_BACK, 0, 0, 0, 0, NULL },
    { oneshot, 0, 0, 0, 0, GUI_FORE, GUI_BACK, 0, 0, 0, 0, NULL },
    { NULL, 0, 0, 0, 0, GUI_FORE, GUI_BACK,  0, 0, 0, 0, NULL }
};

