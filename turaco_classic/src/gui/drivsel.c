// drivsel.c
//
//  The file selector functions
//
//  October, 1998
//  jerry@mail.csh.rit.edu

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <dir.h> // for file attributes.

#include "allegro.h"
#include "general.h"
#include "config.h"
#include "util.h"
#include "snap.h"
#include "editmode.h"
#include "button.h"


#define FDS_FILENAME (127)
#define FDS_GN_SHORT (16)
#define FDS_GN_LONG  (32)

// structure to store all of the file info into...
typedef struct fdlg_st {
    char filename[FDS_FILENAME];
    char gamename_short[FDS_GN_SHORT];
    char gamename_long[FDS_GN_LONG];
    int  n_banks;
    int  n_palettes;
    int  dir;
    long planes;
} FDS;


FDS * fds = NULL;
int fds_total = 0;
int fds_count = 0;
int fds_skipped = 0;

char selected_filename[255];
int selected_item = 0;

char ** dirlist = NULL;
int ndirs = 0;


void progress_box(void)
{
    text_mode(GUI_BACK);
    rectfill(screen, 40, 80, 280, 180, GUI_BACK); 
    border_3d(screen, 40, 80, 241, 101, 1);

    // put first line of text at 160, 100
    //    second line of text at 160, 120
}

void fds_callback(char * fname, int attrib, int param)
{
    char * pBuffer = NULL;
    char sect[127];
    int count;
    int curr;

    if (!fds_total) // stupidness -- look at fds_total, not fds_count
    {
	show_mouse(NULL);
        progress_box();
	textout_centre(screen, font, "Counting...", 160, 100, GUI_FORE);
	show_mouse(screen);
    }

    if (param)
    {
	if (keypressed())
	{
	    if (!fds_skipped)
	        fds_skipped = fds_count;

	    sprintf(fds[fds_count].gamename_long, "Skipped.");
	    sprintf(fds[fds_count].gamename_short, "???");
	    fds[fds_count].n_banks = 0;
	    fds[fds_count].n_palettes = 0;
	    fds[fds_count].planes = 0l;
	    fds[fds_count].dir = -3;
	} else {
	    // then the fds pointer is set up, otherwise, it's just counting

	    sprintf(fds[fds_count].filename, fname);

	    if (param > 0)
		fds[fds_count].dir = param-1;
	    else
		fds[fds_count].dir = param;
	    
	    push_config_state();
	    set_config_file(fname);

	    pBuffer = get_config_string("General", "Description", "None");
	    if (strcmp(pBuffer, "None"))
	    {
		strncpy(fds[fds_count].gamename_long, pBuffer, FDS_GN_LONG);

		pBuffer = get_filename(fname);
		strncpy(fds[fds_count].gamename_short, pBuffer, FDS_GN_SHORT);

		count = 0;
		do{
		    sprintf(sect, "Decode%d", count+1);
		    pBuffer = get_config_string(sect, "start", "----");
		    count++;
		} while (strcmp(pBuffer, "----"));
		fds[fds_count].n_banks = count-1;

		count = 0;
		do{
		    sprintf(sect, "Palette%d", count+1);
		    pBuffer = get_config_string("Palette", sect , "----");
		    count++;
		} while (strcmp(pBuffer, "----"));
		fds[fds_count].n_palettes = count-1;

		// number of bitplanes... see if this works...
		fds[fds_count].planes = 0l;
		for (count = 0; count < fds[fds_count].n_banks ; count++)
		{
		    sprintf(sect, "Decode%d", count+1);
		    curr = get_config_int(sect, "Planes", 0);

		    if (curr)
			fds[fds_count].planes |= (1 << (curr-1));

		}

	    } else {
		sprintf(fds[fds_count].gamename_long, "Badly formatted file.");
		sprintf(fds[fds_count].gamename_short, "???");
		fds[fds_count].n_banks = 0;
		fds[fds_count].n_palettes = 0;
		fds[fds_count].planes = 0l;
	    }
	    pop_config_state();
	}

        fds_count++;

        if (!fds_skipped || !(fds_count%25))
	{
	    show_mouse(NULL);
#ifdef USE_DEGUI
	    // draw a pretty progress bar -- borrow a hidden function from degui
	    _draw_progressbar(60, 123, 200, 20, 
			      1, 
			      (float)fds_count, 
			      (float)fds_total, 
			      0,
			      GUI_FORE,
			      GUI_SELECT,
			      GUI_DESELECT,
			      GUI_DISABLE,
			      GUI_L_SHAD,
			      GUI_D_SHAD);

#else
	    // nothing pretty here .. just the plain 'ol original display...
	    if (fds_count != 0) 
	    textprintf_centre(screen, font, 158, 130, GUI_FORE, "%d/%d",
		fds_count, fds_total);

#endif
	    text_mode(GUI_BACK);
	    if (!fds_skipped)
	    {
		textprintf_centre(screen, font, 158, 151, GUI_FORE, 
				"    %s    ", 
				(param == -1)?"DRIVERS":
				(param == -2)?"EXPDRIV": 
				(param == -3)?"-------": dirlist[param-1]);
		textprintf_centre(screen, font, 158, 165, GUI_FORE, 
			      "    %s    ", get_filename(fname));
	    } else {
		textprintf_centre(screen, font, 158, 151, GUI_FORE, 
			      "                   ");
		textprintf_centre(screen, font, 158, 165, GUI_FORE, 
			      "                   ");
	    }

	    show_mouse(screen);
	}
    } 
}



// for the qsort function...
int fds_cmp(const void *e1, const void *e2)
{
    FDS *fds1 = (FDS *)e1;
    FDS *fds2 = (FDS *)e2;

    if (e1 == NULL || e2 == NULL)  return 0;

    return ( strcmp(fds1->gamename_long, fds2->gamename_long) );
}


// for the gui object...
char* fullname_getter(int index, int *list_size)
{
    if(index < 0)
    {
	// return the count
	*list_size = fds_total;
	return NULL;
    } else {
        // return the string at index
        return fds[index].gamename_long;
    }
}

void fds_dir_callback(char * fname, int attrib, int param)
{
    if (attrib == FA_DIREC)
	if (strcmp(fname, "drivers\\.")  && 
	    strcmp(fname, "drivers\\..") )  // skip these
	    {
		ndirs++;
		dirlist = realloc(dirlist, sizeof(char*)*ndirs );
		dirlist[ndirs-1] = malloc(16);//strlen(entry));
		strcpy(dirlist[ndirs-1], fname+8);
	    }
}

void destroy_dir_list(void)
{
    int temp;

    if (dirlist)
    {
	for(temp=0; temp < ndirs ; temp++)
	    if (dirlist[temp])  // good to check this here..
		free(dirlist[temp]);
	free(dirlist);
	dirlist = NULL;
    }
    ndirs = 0;
}

void create_dir_list(void)
{
    if (dirlist) destroy_dir_list();

    if (drv_subdirs)
	(void) for_each_file("drivers\\*.*", FA_DIREC, fds_dir_callback, 0);
}



int drv_sel_ok(DIALOG *d);
extern DIALOG driver_selector[];

int d_my_list_proc(int msg, DIALOG *d, int c)
{
    int retval;
    int fdsc;
    char tdir[127];
    int cdir;
    int aret;

    selected_item = d->d1;

    if (msg == MSG_START)
    {
        busy();

        create_dir_list();

        // construct the file list...

	fds_total = 0;

	// first count the experimental drivers
	if (file_exists("expdriv", FA_DIREC, &aret)) 
	    fds_total += for_each_file("expdriv\\*.ini", 0, fds_callback, 0);

	// then count the regular drivers
	if (file_exists("drivers", FA_DIREC, &aret)) 
	{
	    fds_total += for_each_file("drivers\\*.ini", 0, fds_callback, 0);

	    for (cdir = 0; cdir < ndirs ; cdir++)
	    {
		sprintf(tdir, "drivers\\%s\\*.ini", dirlist[cdir]);
		fds_total += for_each_file(tdir, 0, fds_callback, 0);
	    }
	}

	if (fds_total) // make sure we have some drivers in the list...
	{

	    //if (fds)  free(fds);
	    fds = (FDS *) malloc(sizeof(FDS) * fds_total);
	    if (fds)
	    {
		textprintf_centre(screen, font, 160, 100, GUI_FORE, 
			"Scanning %d drivers", fds_total );

		fds_count = 0;
		fds_skipped = 0;

		// first scan the experimental drivers
		if (file_exists("expdriv", FA_DIREC, &aret)) 
		    (void) for_each_file("expdriv\\*.ini", 0, fds_callback, -2);

		// then scan the regular drivers
		if (file_exists("drivers", FA_DIREC, &aret)) 
		{
		    (void) for_each_file("drivers\\*.ini", 0, fds_callback, -1);
		    for (cdir = 0; cdir < ndirs ; cdir++)
		    {
			sprintf(tdir, "drivers\\%s\\*.ini", dirlist[cdir]);
			(void) for_each_file(tdir, 0, fds_callback, cdir+1);
		    }
		}


		// checking for user skip.
		if(keypressed())
		    (void) readkey();
		if(fds_skipped)
		    fds_total = fds_skipped;

		// now sort the list...
		// 
		//  it's quick enough (even on my 486) that we don't need to 
		//  change the temporary display to note that we're doing it...

		qsort(fds, fds_total, sizeof(FDS), fds_cmp); // in stdlib  :D
	    }
	} else {
	    not_busy();

	    alert("No drivers found or", "driver directories not found", "",
	          "Bummin!", NULL, 0,0);
	    return D_EXIT;
	}

	not_busy();
    }

    if (msg == MSG_CHAR)
    {
	if (isalnum(c))
	{
	    c &= 0x00ff;
	    c = toupper(c);

	    if (fds)
	    {
	        // if we're on the letter, skip to the next one

		fdsc = d->d1;
	        if (toupper(fds[d->d1].gamename_long[0]) == c &&
	            d->d1 < fds_total-1)
	        {
	            if(toupper(fds[d->d1+1].gamename_long[0]) == c)
		    {
	                d->d1++;
	                selected_item = d->d1;
	                if ( (d->d1 - d->d2) >13)
			    d->d2 = d->d1;
			else if (d->d1 < d->d2)
			    d->d2 = d->d1;
			if (fds_total > 13 && (d->d2 > fds_total-13) )
			    d->d2 = fds_total-14;
			return D_REDRAW;
		    }
	        } else {
		    // find the first occurrence
		    for (fdsc=0 ; fdsc < fds_total ; fdsc++)
		    {
			if (toupper(fds[fdsc].gamename_long[0]) == c)
			{
			    d->d1 = fdsc;
			    if ( (d->d1 - d->d2) >13)
				d->d2 = fdsc;
			    else if (d->d1 < d->d2)
				d->d2 = d->d1;
			    if (fds_total > 13 && (d->d2 > fds_total-13) )
				d->d2 = fds_total-14;
			    selected_item = fdsc;
			    return D_REDRAW;
			}
		    }
	        }

	        // otherwise, go to that letter if it exists...
		for (fdsc=0 ; fdsc < fds_total ; fdsc++)
		{
		    if (toupper(fds[fdsc].gamename_long[0]) == c)
		    {
			d->d1 = fdsc;
			if ( (d->d1 - d->d2) >13)
			    d->d2 = fdsc;
			else if (d->d1 < d->d2)
			    d->d2 = d->d1;
			if (fds_total > 13 && (d->d2 > fds_total-13) )
			    d->d2 = fds_total-14;
			selected_item = fdsc;
			return D_REDRAW;
		    }
		}
		// phew!
	    }
	    return D_USED_CHAR;

	} else if ((c & 0x0ff) == ' '  ||
	           (c & 0x0ff) == 13) {

	    // move the control over to the OK button

	    simulate_keypress(0x0f09);
	    return D_USED_CHAR;

	} else if (c>>8 == KEY_P) {
	    screen_snap();
	} 
    }

    if (msg == MSG_DCLICK)
    {
	// best to not do anything...
	//simulate_keypress(0x0f09);
	//return D_O_K;
	busy();
	strcpy(selected_filename, fds[selected_item].filename);
	return D_EXIT;
    }

    retval = d_list_proc(msg, d, c);

    if (msg == MSG_END) {
        // destroy the file list....
        if (fds)
            free(fds);
        destroy_dir_list();
        return D_REDRAW;
    } 

    if (msg != MSG_IDLE) {
        int text_x = d->x+d->w+4;
        int text_y = d->y;

	border_3d(screen, text_x, text_y, 101, 15, 0);

	border_3d(screen, text_x, text_y+16, 101, 15, 0);

	border_3d(screen, text_x, text_y+36, 101, 36, 0);

	text_y+=4;
	text_x+=4;

        // redraw the text information
        text_mode(GUI_BACK);
        if (fds)
        {
	    textprintf(screen, font, text_x, text_y, GUI_FORE, "%-12s", 
	        (fds[d->d1].dir == -1)?"DRIVERS":
	        (fds[d->d1].dir == -2)?"EXPDRIV": 
	        (fds[d->d1].dir == -3)?"--------": dirlist[fds[d->d1].dir]);

	    textprintf(screen, font, text_x, text_y+16, GUI_FORE, "%-12s", 
		    fds[d->d1].gamename_short);

	    textprintf(screen, font, text_x, text_y+36, GUI_FORE, 
		    "%d bank%c  ", fds[d->d1].n_banks,
		    (fds[d->d1].n_banks != 1)? 's' : ' ');

	    textprintf(screen, font, text_x, text_y+46, GUI_FORE, 
		    " %s %s %s %s %s", 
		    (fds[d->d1].planes & 0x01) ?  "2":" ",
		    (fds[d->d1].planes & 0x02) ?  "4":" ",
		    (fds[d->d1].planes & 0x04) ?  "8":" ",
		    (fds[d->d1].planes & 0x08) ? "16":"  ",
		    (fds[d->d1].planes & 0x10) ? "32":"  ");

	    textprintf(screen, font, text_x, text_y+56, GUI_FORE, 
		    "%d palette%c ", fds[d->d1].n_palettes,
		    (fds[d->d1].n_palettes != 1 )? 's' : ' ');

	}
    }

    return (retval);
}

int drv_sel_ok(DIALOG *d)
{
    busy();
    strcpy(selected_filename, fds[selected_item].filename);
    return D_EXIT;
}

int drv_sel_cancel(DIALOG *d)
{
    selected_filename[0] = '\0';
    return D_EXIT;
}

static DIALOG driver_selector[] =
{
   /* (dialog proc)     (x)   (y)   (w)   (h)   (fg)  (bg)  (key) (flags)  (d1)  (d2)  (dp) */
   { d_shadow_box_proc, 0,    0,    314,  160,  0,    0,    0,    0,       0,    0,    NULL },
   { d_ctext_proc,      152,  8,    1,    1,    0,    0,    0,    0,       0,    0,   "Select a Driver..." },
   { button_dp2_proc,   220,  107,  80,   16,   0,    0,    0,    D_EXIT,  0,    0,    "OK", drv_sel_ok },
   { button_dp2_proc,   220,  129,  80,   16,   0,    0,    27,   D_EXIT,  0,    0,    "Cancel", drv_sel_cancel },
   { d_my_list_proc,    8,    26,   196,  119,  0,    0,    0,    D_EXIT,  0,    0,    fullname_getter },
   { NULL }
};


// now the function that puts it all together.  the filename selected is 
// stored in selected_filename.  if CANCEL was chosen, [0] is null.
void game_driver_selector(void)
{
    int aret;

    if (!file_exists("drivers", FA_DIREC, &aret) && 
        !file_exists("expdriv", FA_DIREC, &aret))
    {
	alert("ERROR:", "DRIVERS directory not found!", "",
	      "Continue", NULL, 0, 0);
    } else {
	centre_dialog(driver_selector);
	set_dialog_color(driver_selector, GUI_FORE, GUI_BACK);
	do_dialog(driver_selector, 4);
    }
}

