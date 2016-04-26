// Editmenu.c
//
//  edit menu functions
//
//  September, 1998
//  jerry@mail.csh.rit.edu

#include <string.h>  // strncpy
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> 

#include "allegro.h"
#include "button.h"
#include "general.h"
#include "config.h"
#include "coding.h" // commit graphics banks
#include "font.h"
#include "sprtplte.h"
#include "sprite.h"
#include "gamedesc.h"
#include "bmpdisp.h"

BITMAP * clipboard = NULL;


// UNDO / REDO

int edit_undo(void)
{
    return D_O_K;
}

int edit_redo(void)
{
    return D_O_K;
}


// COPY / PASTE (SNARF/BARF)

int edit_copy_sprite(void)
{
    if (current_sprite == NULL)   return D_O_K;

    if (clipboard != NULL) destroy_bitmap(clipboard);

    clipboard = create_bitmap(current_sprite->w, current_sprite->h);

    blit(current_sprite, clipboard, 0, 0, 0, 0, 
         current_sprite->w, current_sprite->h);

    return D_O_K;
}

int edit_paste_sprite(void)
{
    if (current_sprite == NULL)   return D_O_K;

    if (clipboard == NULL)        return D_O_K;

    blit(clipboard, current_sprite, 0, 0, 0, 0, 
         current_sprite->w, current_sprite->h);

    return D_O_K;
}


// RIP FUNCTIONS

char nsprites_wide[16];
char final_resolution[64];
int old = -1;
static /*extern */ DIALOG save_pcx_dialog[];

int nd_edit_proc(int msg, DIALOG *d, int c)
{
    int new;
    if (msg == MSG_IDLE)
    {
	new = atoi(nsprites_wide);
	if (new != old)
	{
	    old = new;

	    if (new != 0)
	    {
		sprintf(final_resolution, "%d x %d        ", 
		    (new * current_sprite->w),
		    GfxBanks[currentGfxBank].n_total
		    / new * current_sprite->h );
	    } else {
		sprintf(final_resolution, "?            ");
	    }
	    final_resolution[15] = '\0';

	    show_mouse(NULL);
	    save_pcx_dialog[7].proc(MSG_DRAW, &save_pcx_dialog[7], 0);
	    show_mouse(screen);
	}

    }

    if (msg == MSG_CHAR)
    {
	if (isprint(c))
	{
	    if (isdigit(c))
		return( d_edit_proc(msg, d, c) );
	    else 
		return D_USED_CHAR;
	} else
	    return( d_edit_proc(msg, d, c) );

    } else
	return( d_edit_proc(msg, d, c) );

    return D_O_K;
}

static DIALOG save_pcx_dialog[] =
{
   /* (dialog proc)     (x)   (y)   (w)   (h)   (fg)  (bg)  (key) (flags)  (d1)  (d2)  (dp) */
   { d_shadow_box_proc, 0,    0,    304,  160,  0,    0,    0,    0,       0,    0,    NULL },
   { d_ctext_proc,      152,  8,    1,    1,    0,    0,    0,    0,       0,    0,    "Image save settings..." },
   { d_button_proc,     208,  107,  80,   16,   0,    0,    'o',  D_EXIT,  0,    0,    "&OK" },
   { d_button_proc,     208,  129,  80,   16,   0,    0,    'c',  D_EXIT,  0,    0,    "&Cancel" },

   { d_text_proc,      40,  38,     1,   1,    0,    0,    0,    0,       0,    0,    "No. Sprites Wide:"},
   { nd_edit_proc,     180, 38,     50,  8,    0,    0,    0,    0,       4,    0,    nsprites_wide },

   { d_text_proc,      40,  60,     1,   1,    0,    0,    0,    0,       0,    0,    "Final Resolution:"},
   { d_text_proc,      180, 60,     1,   1,    0,    0,    0,    0,       0,    0,    final_resolution},
   { NULL }
};

#define SPD_OK     (2)
#define SPD_CANCEL (3)

int edit_save_pcx(void)
{
    char the_path[255];
    char newname[32];
    char numbre[32];
    char *pos;
    BITMAP * tb;
    BITMAP * save_me;
    RGB pal[256];
    int count;
    int x,y;
    int sx,sy;
    int wide;
PALETTE _current_pallete; // GN: editmenu.c|228|error: ‘_current_pallete’ undeclared 
get_pallete(_current_pallete);

    if (!GameDriverLoaded)
    {
	alert("Cannot save pcx!", "No romdata loaded!", "Sorry.", 
		"&Okay", NULL, 'O', 0);
	return D_REDRAW;
    }

    // get user preferences
    strncpy(nsprites_wide, get_config_string("PCX_Rip", "NumPerRow", "32"), 16);

    centre_dialog(save_pcx_dialog);
    set_dialog_color(save_pcx_dialog, GUI_FORE, GUI_BACK);
    if (do_dialog(save_pcx_dialog, 2) == SPD_OK)
    {
	set_config_string("PCX_Rip", "NumPerRow", nsprites_wide);
	wide = atoi(nsprites_wide);

	strncpy(the_path, get_config_string("PCX_Rip", "Path", "."), 255);
	put_backslash(the_path);

	// generate a possible name for the image...
	// in the form:  pacman bank 4:  pacm04.pcx
	strcpy(newname, ROMDirName);
	newname[6] = '\0';
	sprintf(numbre, "%02d", currentGfxBank+1);
	strcat(newname, numbre);
	strcat(the_path, newname);
	strcat(the_path, ".PCX");

	if (file_select("Enter A PCX Image filename:", the_path, "PCX") )
	{
	    Commit_Graphics_Bank();

	    // save it out now 
	    tb = create_bitmap(current_sprite->w, current_sprite->h);
	    sx = wide * (current_sprite->w);
	    sy = GfxBanks[currentGfxBank].n_total
		 / wide  * (current_sprite->h);

	    save_me = create_bitmap(sx, sy);

	    if (tb && save_me)
	    {
		// generate the bitmap to be saved
		x = y = 0;
		for (count = 0 ; 
		     count < GfxBanks[currentGfxBank].n_total ;
		     count ++)
		{
		    if (x >= wide)
		    {
			x = 0;
			y++;
		    }

		    // get a sprite
		    sprite_get(count, tb, sprite_bank);

		    // color shift it
		    for (sx=0 ; sx < tb->w ; sx++)
		        for (sy=0 ; sy < tb->h ; sy++)
			    putpixel(tb, sx, sy, 
			             getpixel(tb, sx, sy) - FIRST_USER_COLOR);
		    // copy it to the save bitmap
		    blit(tb, save_me, 0, 0,
			 x*tb->w, y*tb->h, tb->w, tb->h);

		    x++;
		}

		// generate the palette to be saved
		for (count = FIRST_USER_COLOR ; count < 256 ; count++)
		{
		    pal[count - FIRST_USER_COLOR].r = _current_pallete[count].r;
		    pal[count - FIRST_USER_COLOR].g = _current_pallete[count].g;
		    pal[count - FIRST_USER_COLOR].b = _current_pallete[count].b;
		}
		// let the last colors just be whatever they are -- they're 
		// not important anyway...

		// and finally ... save it out!
		save_pcx(the_path, save_me, pal);
	    }

	    if (tb)       destroy_bitmap(tb);
	    if (save_me)  destroy_bitmap(save_me);

	    pos = get_filename(the_path);
	    *pos = '\0';
	    set_config_string("PCX_Rip", "Path", the_path);

	}
    }
    return(D_REDRAW);
}

BITMAP * elpd_bmp = NULL;
BITMAP_DISPLAY_STRUCT * bds = NULL;

int home_callback(DIALOG * d, int ic)
{
    if (bds)
    {
	bds->x = 0;
	bds->y = 0;
	return (D_REDRAW);
    }
    return (D_O_K);
}

int grid_callback(DIALOG * d, int ic)
{
    return (D_REDRAW);
}


int elpd_callback(DIALOG * d, int x, int y, int mb)
{
//    blit(s,d, sx,sy, dx,dy, w,h)

    blit(elpd_bmp, current_sprite, x,y, 0,0, 
         current_sprite->w, current_sprite->h);

    return D_EXIT;
}

DIALOG edit_load_PCX_dialog[] =
{
   /* (dialog proc)     (x)   (y)   (w)   (h)   (fg)  (bg)  
   		(key) (flags)  (d1)  (d2)  (dp) */

    /// base dialog stuff 
    { d_shadow_box_proc, 0,    0,    314,  235,  0,    0,    
                0,    0,       0,    0,    NULL },
    { d_ctext_proc,      160,  7,    1,    1,    0,    0,    
                0,    0,       0,    0,   "Select Sprite Region"},

    { bitmap_display_proc,      5, 20, 304, 155, 0, 0, 
                0, 0, 0, 0, &elpd_bmp, &bds, elpd_callback },
    
    { button_dp2_proc,   5, 180, 80, 16, 0, 0,
   		'h', D_EXIT,  0, 0, "&Home",  home_callback },

    { button_dp2_proc,   225, 190, 80, 16, 0, 0,
   		'g', D_EXIT,  0, 0, "&Grid",  grid_callback },

    { d_button_proc,   225,  210,  80,   16,   0,    0,    
                'c',   D_EXIT,  0,    0,    "&Close", NULL },

    { NULL },
};
#define ELPD_BMP_DISPLAY (2)


void adjust_bitmap(BITMAP * bmp, int colors)
{
    int x, y, c;

    if (bmp == NULL)
	return;

    for (x=0 ; x< bmp->w ; x++)
	for (y=0 ; y< bmp->h ; y++)
	{
	    c = (getpixel(bmp, x, y))%colors;
	    c += FIRST_USER_COLOR;
	    putpixel(bmp, x, y, c);
	}

}

int edit_load_pcx(void)
{
    RGB a_palette[256];
    char the_path[255];
    char *pos;

    if (!GameDriverLoaded)
    {
	alert("Cannot oade pcx!", "No romdata loaded!", "Sorry.", 
		"&Okay", NULL, 'O', 0);
	return D_REDRAW;
    }

    strncpy(the_path, get_config_string("PCX_Rip", "Path", "."), 255);
    put_backslash(the_path);

    if (file_select("Select A PCX Image filename:", the_path, "PCX") )
    {
	elpd_bmp = load_pcx(the_path, a_palette);

	if (elpd_bmp)
	{
	    // perhaps in this place here, we should put in a selector
	    // to remap the colors... but for now, we will just modulo
	    // with the number of colors available in this bank, and add
	    // on FIRST_USER_COLOR
	    adjust_bitmap(elpd_bmp, 
	                  (1 << GfxBankExtraInfo[currentGfxBank].planes)
	                 );

	    /*
	    */ bds = NULL;
	    bds = (BITMAP_DISPLAY_STRUCT *) 
		      malloc(sizeof(BITMAP_DISPLAY_STRUCT));

	    bds->x = bds->y = 0;
	    bds->size = current_sprite->w;
	    bds->grid = FALSE;
	    bds->sel_w = current_sprite->w;
	    bds->sel_h = current_sprite->h;

	    centre_dialog(edit_load_PCX_dialog);
	    set_dialog_color(edit_load_PCX_dialog, GUI_FORE, GUI_BACK);
	    do_dialog(edit_load_PCX_dialog, -1);

	    free(bds);
	} else {
	    alert("", "Unable to load the bitmap", "", 
	          "&Bummin", NULL, 'b', 0);
	}

	pos = get_filename(the_path);
	*pos = '\0';
	set_config_string("PCX_Rip", "Path", the_path);
    }
    return(D_REDRAW);
}

// Quote from "The fairly decent three":
// How many of you are there?
//   um.... counting the blind man and the gimp...  three.
//   but you really can't count the gimp.  he can't pull a trigger.

// MISC PREFERENCES 
extern DIALOG edit_prefs_dialog[];

int do_settings(int msg, DIALOG *d, int c);

///// resolution stuff

int r320x240(void) { gfx_hres=320;  gfx_vres=240; return D_REDRAW; }
int r320x400(void) { gfx_hres=320;  gfx_vres=400; return D_REDRAW; }
int r320x480(void) { gfx_hres=320;  gfx_vres=480; return D_REDRAW; }
int r640x480(void) { gfx_hres=640;  gfx_vres=480; return D_REDRAW; }
int r800x600(void) { gfx_hres=800;  gfx_vres=600; return D_REDRAW; }
int r1024x768(void){ gfx_hres=1024; gfx_vres=768; return D_REDRAW; }

MENU resolution_popup[] =
{
    {"320x240",  r320x240, NULL, 0, NULL},
    {"320x400",  r320x400, NULL, 0, NULL},
    {"320x480",  r320x480, NULL, 0, NULL},
    {"640x480",  r640x480, NULL, 0, NULL},
    {"800x600",  r800x600, NULL, 0, NULL},
    {"1024x768", r1024x768, NULL, 0, NULL},
    {NULL}
};


int resolution_callback(DIALOG * d, int ic)
{
    int sel = do_menu(resolution_popup, d->x+10, d->y+4);

    if (sel >= 0)
#if 0 // GN: wy can't sprintf to  edit_prefs_dialog[DE_RESOLUTION].dp
	strcpy(edit_prefs_dialog[8].dp, resolution_popup[sel].text);
#endif
    return(D_REDRAW);
}
///// end resolution stuff

// GN: tmp
#define DE_USE_SUBDIRS (2)
#define DE_NEW_FONT (3)
#define DE_RESOLUTION (8)
#define DE_TROLL_MAGIC (4)


DIALOG edit_prefs_dialog[] =
{
   /* (dialog proc)     (x)   (y)   (w)   (h)   (fg)  (bg)  
   		(key) (flags)  (d1)  (d2)  (dp) */

    /// base dialog stuff 
    { d_shadow_box_proc, 0,    0,    314,  235,  0,    0,    
                0,    0,       0,    0,    NULL },
    { d_ctext_proc,      160,  7,    1,    1,    0,    0,    
                0,    0,       0,    0,   "Turaco User Preferences"},

//#define DE_USE_SUBDIRS (2)
    { d_check_proc,      6, 40, 290, 16, 0, 0, 
                'd', 0, 0, 0, "Use &Driver Subdirectories" },

//#define DE_NEW_FONT (3)
    { d_check_proc,    6, 54, 290, 16, 0, 0, 
                'n', 0, 0, 0, "Use &New Font" },

//#define DE_TROLL_MAGIC (4)
    { d_check_proc,    6, 68, 290, 16, 0, 0, 
                't', 0, 0, 0, "&Troll Magic" },

    { d_text_proc,      10,  90,    1,    1,    0,    0,    
                0,    0,       0,    0,   "Rom Paths:"},
    { d_edit_proc,       10, 100, 285, 16, 0, 0, 
                0, 0, ROM_PATH_LEN, 1, ROMPath }, 

    { d_text_proc,      10,  130,    1,    1,    0,    0,    
                0, 0,   0,    0,   "Screen Resolution:"},

//#define DE_RESOLUTION (8)
    { button_dp2_proc,   10, 140, 100, 16, GUI_FORE, GUI_BACK,
   		0, D_EXIT,  0, 0, "1                        27",  
   		resolution_callback },

    { d_button_proc,   225,  203,  80,   16,   0,    0,    
                'c',   D_EXIT,  0,    0,    "&Close", NULL },

    { do_settings, 0, 0, 314, 0 },
    { NULL, 0, 0, 0, 0, GUI_FORE, GUI_BACK,  0, 0, 0, 0, NULL }
};


int do_settings(int msg, DIALOG *d, int c)
{
    if (msg == MSG_START)
    {
	if (drv_subdirs)  edit_prefs_dialog[DE_USE_SUBDIRS].flags |= D_SELECTED;
	else              edit_prefs_dialog[DE_USE_SUBDIRS].flags &= ~D_SELECTED;
	if (font == &my_new_font) 
	      edit_prefs_dialog[DE_NEW_FONT].flags |= D_SELECTED;
	else  edit_prefs_dialog[DE_NEW_FONT].flags &= ~D_SELECTED;

#if 0 // GN: wy can't sprintt to  edit_prefs_dialog[DE_RESOLUTION].dp
	sprintf(edit_prefs_dialog[DE_RESOLUTION].dp, "%dx%d", gfx_hres, gfx_vres);
#endif
	d->d1 = gfx_hres;
	d->d2 = gfx_vres;

	if (troll_magic)  edit_prefs_dialog[DE_TROLL_MAGIC].flags |= D_SELECTED;
	else              edit_prefs_dialog[DE_TROLL_MAGIC].flags &= ~D_SELECTED;

	return D_REDRAW;
    }

    if (msg == MSG_DRAW)
    {
        line(screen, d->x+10, d->y+20, d->x+d->w-10, d->y+20, GUI_FORE);
        line(screen, d->x+10, d->y+21, d->x+d->w-10, d->y+21, GUI_MID);
    }

    if (msg == MSG_END)
    {
        if (edit_prefs_dialog[DE_TROLL_MAGIC].flags & D_SELECTED)
	    troll_magic = 1;
	else 
	    troll_magic = 0;

        if (edit_prefs_dialog[DE_USE_SUBDIRS].flags & D_SELECTED)
	    drv_subdirs = 1;
	else 
	    drv_subdirs = 0;

        if (edit_prefs_dialog[DE_NEW_FONT].flags & D_SELECTED)
	    font = &my_new_font;
	else
	    font = original_font;

	if (gfx_hres != d->d1 || gfx_vres != d->d2)
	    alert("The new screen resolution will", 
	          "be active the next time TURACO",
	          "is restarted", 
	      "Okay", NULL, 0, 0);

    }
    return D_O_K;
}

int edit_preferences(void)
{
    /// all of the bitmap crap in here is a kludge to temporarily fix
    ///  bug 09.  Without this stuff here, the current_sprite gets
    ///  rotated by 45 degrees clockwise.  odd.

    BITMAP * bmp = create_bitmap(current_sprite->w, current_sprite->h);

    if(bmp)
    {
	blit(current_sprite, bmp, 0, 0, 0, 0, 
	     current_sprite->w, current_sprite->h);
    }

    centre_dialog(edit_prefs_dialog);
    set_dialog_color(edit_prefs_dialog, GUI_FORE, GUI_BACK);
    do_dialog(edit_prefs_dialog, -1);

    if(bmp)
    {
	blit(bmp, current_sprite, 0, 0, 0, 0, 
	     current_sprite->w, current_sprite->h);
	destroy_bitmap(bmp);
    }

    return D_REDRAW;
}

