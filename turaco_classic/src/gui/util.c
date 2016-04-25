// util.c
//
//  bitmap editor UI module for allegro
//
//  September, 1998
//  jerry@mail.csh.rit.edu

#include <stdio.h>
#include <allegro.h>
#include "general.h" // for the color definitions
#include "util.h"


void position_dialog(DIALOG *d, int x, int y)
{
    int c=0;

    if (d==NULL) return;
    if (d[c].x != 0) return;

    while(d[c].proc != NULL)
    {
	d[c].x+= x;
	d[c].y+= y;
	c++;
    }
}


void my_draw_dotted_rect(BITMAP *bmp,int x1,int y1,int x2,int y2,int c)
{
    int x, y;

    for(x=x1; x<=x2; x+=2)
	putpixel(bmp,x,y1,c);

    for(x=x1+1; x<=x2; x+=2)
	putpixel(bmp,x,y2,c);

    for(y=y1; y<=y2; y+=2)
	putpixel(bmp,x1,y,c);

    for(y=y1+1; y<=y2; y+=2)
	putpixel(bmp,x2,y,c);
}


#ifndef USE_DEGUI
void draw_dotted_rect(BITMAP *bmp,int x1,int y1,int x2,int y2,int c)
{
    my_draw_dotted_rect(bmp, x1, y1, x2, y2, c);
}
#endif

void border_3d(BITMAP * bmp, int x, int y, int w, int h, int up)
{
    int ca,cb;

    if (up)
    {
	ca = GUI_L_SHAD;
	cb = GUI_D_SHAD;
    } else {
	ca = GUI_D_SHAD;
	cb = GUI_L_SHAD;
    }

    line(bmp, x, y, x+w-1, y, ca);
    line(bmp, x, y, x, y+h-1, ca);
    line(bmp, x+1, y+h-1, x+w-1, y+h-1, cb);
    line(bmp, x+w-1, y+1, x+w-1, y+h-1, cb);
}

void box_3d(BITMAP * bmp, 
	    int w, int h, 
	    int up, int selected)
{
    int ca,cb;

    if (up)
    {
	ca = GUI_L_SHAD;
	cb = GUI_D_SHAD;
    } else {
	ca = GUI_D_SHAD;
	cb = GUI_L_SHAD;
    }

    rect(bmp, 0, 0, w-1, h-1, GUI_FORE);
    line(bmp, 1, 1, w-2, 1, cb);
    line(bmp, 1, 1, 1, h-2, cb);
    line(bmp, 2, h-2, w-2, h-2, ca);
    line(bmp, w-2, 2, w-2, h-2, ca);

//    rect(bmp, 1, 1, w-2, h-2, (selected)?COLOR_HILITE:COLOR_LOLITE);

    if (selected)
    {
	rect(bmp, 0, 0,  w-1, h-1, GUI_BACK);
	draw_dotted_rect(bmp, 0, 0, w-1, h-1, GUI_FORE);
    } else {
	rect(bmp, 0, 0,  w-1, h-1, GUI_FORE);
    }
}


