// bitmaped.c
//
//  bitmap editor UI module for allegro
//
//  September, 1998
//  jerry@mail.csh.rit.edu
//
//  Re-written 5-7 March 1999
//  jerry@absynth.com

// see the .h file for what data is to be displayed in what fields

#include <stdio.h>
#include <allegro.h>
#include "general.h"
#include "toolmenu.h"
#include "util.h"
#include "bitmaped.h"


struct _bitmap_editor_internals {
    int hilite;
    int grid;
    int * xscal;
    int * yscal;
    int old_w;
    int old_h;
};


void bitmaped_free_tables(struct _bitmap_editor_internals *bi)
{
    if (bi->xscal)
	free(bi->xscal);
    if (bi->yscal)
	free(bi->yscal);
    bi->xscal = NULL;
    bi->yscal = NULL;
}


void bitmaped_generate_tables(BITMAP * bmp, 
                              DIALOG *d, 
                              struct _bitmap_editor_internals *bi)
{
    int c;
    float tf;

    if (bi == NULL) return;

    bitmaped_free_tables(bi);

    bi->xscal = (int *)malloc(sizeof(int) * d->w);
    bi->yscal = (int *)malloc(sizeof(int) * d->h);

    if (bi->xscal == NULL || bi->yscal == NULL)
    {
	bitmaped_free_tables(bi);
	return;
    }

    memset(bi->xscal, 0, (sizeof(int) * d->w));
    memset(bi->yscal, 0, (sizeof(int) * d->h));

    tf = (float)(d->w-4)/(float)(bmp->w);
    for (c = 0 ; c < d->w-4 ; c++)
    {
	bi->xscal[c] = (int) ( c / tf );
    }

    tf = (float)(d->h-4)/(float)(bmp->h);
    for (c = 0 ; c < d->h-4 ; c++)
    {
	bi->yscal[c] = (int) ( c / tf );
    }
}


void bitmaped_draw_grid(BITMAP *bmp, struct _bitmap_editor_internals *bi)
{
    int c;
    int last = -2;

    for (c=0 ; c<(bmp->h)-4 ; c++)
    {
	if (bi->xscal[c] != last)
	{
	    last = bi->xscal[c];
	    line(bmp, c+2, 2, c+2, bmp->w-3, GUI_FORE);
	}
    }

    last = -2;
    for (c=0 ; c<(bmp->w)-4 ; c++)
    {
	if (bi->yscal[c] != last)
	{
	    last = bi->yscal[c];
	    line(bmp, 2, c+2, bmp->w-3, c+2, GUI_FORE);
	}
    }
}


void bitmaped_draw_bitmap(BITMAP *src, BITMAP *dest, 
                          struct _bitmap_editor_internals *bi)
{
    int x,y;

    // in case the scaling tables aren't around, use the old method
    // to draw the bitmap -- this will probably never be called.
    // besides -- if it can't allocate ~160 ints, there are some serious
    // memory problems, and the whole system probably won't work right.
    if (bi == NULL || bi->xscal == NULL || bi->yscal == NULL)
    {
	stretch_blit(src, dest, 0, 0, src->w, src->h,
		    2, 2, dest->w-4, dest->h-4);
	return;
    }

    // for now, the first version will be brute-force... nothing quick here...
    // 
    // this seems to be quite quick on my 486sx33, so this will probably not
    // be changed anytime soon.  I was thinking of perhaps:
    // 1) drawing filled rectangles for each pixel
    // 2) drawing down a row of lines, then re-blitting that for the rows
    // but neither speedup seems necessary now that this seems as quick as
    // it is...
    for (x=2; x<dest->w-2 ; x++)
	for (y=2; y<dest->h-2 ; y++)
	{
	    putpixel(dest, x, y, 
	             getpixel(src, bi->xscal[x-2], bi->yscal[y-2]) );
	}
}


/* custom dialog procedure for the bitmap editor object */
int bitmap_editor_proc(int msg, DIALOG *d, int c)
{
    BITMAP * temp;
    BITMAP ** temp1;
    BITMAP * temp2;
    BITMAP * temp3;
    struct _bitmap_editor_internals *bi;
    int x,y;
    int lastx = -1, lasty = -1;
    int ret = D_O_K;
    void (*proc)(DIALOG *dl, int x_pos, int y_pos, int mouse);

    /* process the message */
    switch (msg) {

    /* initialise when we get a start message */
    case MSG_START:
	d->dp3 = malloc(sizeof(struct _bitmap_editor_internals));
	bi = d->dp3;
	bi->hilite = COLOR_LOLITE;
	bi->grid = 1;
	bi->xscal = NULL;
	bi->yscal = NULL;
	bi->old_w = -2;
	bi->old_h = -2;
	break;

    /* shutdown when we get an end message */
    case MSG_END:
	bi = d->dp3;
	if(bi)
	{
	    bitmaped_free_tables(bi);
	    free(bi);
	    d->dp3 = NULL;
	}
	break;

    /* draw in response to draw messages */
    case MSG_DRAW:
	bi = d->dp3;

	temp = create_bitmap(d->w, d->h);
	clear_to_color(temp, 3); //d->bg);

	if (d->dp == NULL)
	{
	    text_mode (-1);
	    textout_centre(temp, font, "??", (d->w)/2, (d->h)/2, GUI_FORE);
	} else {
	    temp1 = d->dp;  // pointer to the bitmap to draw
	    temp2 = *temp1; // this is the bitmap to draw
	    if (temp2 == NULL)
	    {
		text_mode (-1);
		textout_centre(temp, font, "No Gfx!", temp->w/2, temp->h/2, GUI_FORE);
	    } else {
		// if it's a different size than before, update the tables...
		if ( (bi->old_w != temp2->w) ||
		     (bi->old_h != temp2->h) )
		{
		    bi->old_w = temp2->w;
		    bi->old_h = temp2->h;
		    (void) SEND_MESSAGE(d, MSG_SIZE_CHANGE, 0);
		    bi = d->dp3;
		}

		// draw the bitmap here...
		bitmaped_draw_bitmap(temp2, temp, bi);

		// and the grid if necessary
		if (bi->grid)
		    bitmaped_draw_grid(temp, bi);

		// now draw out the small version of the bitmap...
		if (d->d1 != -1 && d->d2 != -1)
		{
		    temp3 = create_bitmap(temp2->w+2, temp2->h+2);
		    if (temp3)
		    {
			clear_to_color(temp3, GUI_MID);
			blit(temp2, temp3, 0, 0, 1, 1, temp2->w, temp2->h);
			border_3d(temp3, 0, 0, temp3->w, temp3->h, 0);
			blit(temp3, screen, 0,0, d->d1, d->d2, temp3->w, temp3->h);
			destroy_bitmap(temp3);
		    }
		}
	    }
	}

	// the highlight/selection boxen
	box_3d(temp, temp->w, temp->h, 
		(bi->hilite == COLOR_HILITE)?1:0,
		(bi->hilite == COLOR_HILITE)?1:0
		);

	// and copy it to the screen
	blit(temp, screen, 0, 0, d->x, d->y, d->w, d->h);
	destroy_bitmap(temp);
	break; 

    case MSG_CLICK:
	temp1 = d->dp;  // pointer to the bitmap to draw
	temp2 = *temp1; // this is the bitmap to draw
	bi = d->dp3;

	// take control of the mouse and handle drawing

	// constrain the mouse (fixed 6 Mar 99)
	set_mouse_range(d->x+2, d->y+2, d->x+d->w-3, d->y+d->h-3);

	while(mouse_b) // while a button is down...
	{
	    // figure out the target position
	    /* -- new method -- */
	    // a table lookup -- the old method used a math function.
	    // this is quicker, not that the old one was slow.
	    x = bi->xscal[(mouse_x - d->x - 2)];
	    y = bi->xscal[(mouse_y - d->y - 2)];

	    // if the new position is different...
	    if (lastx != x || lasty != y)
	    {
		/* call callback function here */
		if (d->dp2) {
		    proc = d->dp2;
		    (*proc)(d, x, y, c);
		}

		// now re-draw it all...
		show_mouse(NULL);
		SEND_MESSAGE(d, MSG_DRAW, 0);
		show_mouse(screen);

		lastx=x;
		lasty=y;
	    }
	}

	// unconstrain the mouse
	set_mouse_range(0, 0, SCREEN_W-1, SCREEN_H-1);
	break; 

    case MSG_DCLICK:
	break; 

    case MSG_GOTMOUSE:
	bi = d->dp3;
	bi->hilite = COLOR_HILITE;
	show_mouse(NULL);
	SEND_MESSAGE(d, MSG_DRAW, 0);
	show_mouse(screen);
	break; 

    case MSG_LOSTMOUSE:
	bi = d->dp3;
	bi->hilite = COLOR_LOLITE;
	show_mouse(NULL);
	SEND_MESSAGE(d, MSG_DRAW, 0);
	show_mouse(screen);
	break; 

    case MSG_GOTFOCUS:
	bi = d->dp3;
	bi->hilite = COLOR_HILITE;
	break; 

    case MSG_LOSTFOCUS:
	bi = d->dp3;
	bi->hilite = COLOR_LOLITE;
	break; 

    case MSG_WANTFOCUS:
	return D_WANTFOCUS;
	break; 

    case MSG_CHAR:
	temp1 = d->dp;  // pointer to the bitmap to draw
	temp2 = *temp1; // this is the bitmap to draw

	bi = d->dp3;
	if (c == (KEY_X << 8) ){
	    do_tool(TOOLS_HORIZ_FLIP, temp2, 0, 0);
	    ret = D_USED_CHAR;
	} else if (c == (KEY_Y << 8) ){
	    do_tool(TOOLS_VERT_FLIP, temp2, 0, 0);
	    ret = D_USED_CHAR;

	} else if (c == (KEY_Z << 8) ){
	    do_tool(TOOLS_CW_ROTATE, temp2, 0, 0);
	    ret = D_USED_CHAR;
	} else if (c == (KEY_W << 8) ){
	    do_tool(TOOLS_CW_ROTATE_45, temp2, 0, 0);
	    ret = D_USED_CHAR;

	} else if (c == (KEY_UP << 8)){ // alt+letter
	    do_tool(TOOLS_WRAP_UP, temp2, 0, 0);
	    ret = D_USED_CHAR;
	} else if (c == (KEY_DOWN << 8)){ // alt+letter
	    do_tool(TOOLS_WRAP_DOWN, temp2, 0, 0);
	    ret = D_USED_CHAR;
	} else if (c == (KEY_LEFT << 8)){ // alt+letter
	    do_tool(TOOLS_WRAP_LEFT, temp2, 0, 0);
	    ret = D_USED_CHAR;
	} else if (c == (KEY_RIGHT << 8)){ // alt+letter
	    do_tool(TOOLS_WRAP_RIGHT, temp2, 0, 0);
	    ret = D_USED_CHAR;

	} else if (c == (KEY_K << 8)){ // alt+letter
	    do_tool(TOOLS_CLEAR_BITMAP, temp2, 0, 0);
	    ret = D_USED_CHAR;

	} else if (c == (KEY_G << 8)){         // alt+letter
	    bi->grid ^= 1;
	    ret = D_USED_CHAR;
	}
	show_mouse(NULL);
	SEND_MESSAGE(d, MSG_DRAW, 0);
	show_mouse(screen);
	break; 

    case MSG_SIZE_CHANGE:
	temp1 = d->dp;  // pointer to the bitmap to draw
	temp2 = *temp1; // this is the bitmap to draw
	bi = d->dp3;

	if ( (bi != NULL) && (temp2 != NULL) && (d != NULL) )
	{
	    bitmaped_generate_tables(temp2, d, bi);
	}
	break; 
    }


    /* always return OK status, since we don't ever need to close 
    * the dialog or get the input focus.
    */
    return ret;
}
