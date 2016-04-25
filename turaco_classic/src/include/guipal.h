// GUIpal.h
//
//  Palette UI module for allegro
//
//  September, 1998
//  jerry@mail.csh.rit.edu


typedef struct palette_select{
    RGB * palette;  // palette for graphics - color 0 thru color N
    int ncolors;    // number of colors - MUST be a multiple of 2
    int basecolor;  // first palette color to base things on.
} PALETTE_SELECT;

/* custom dialog procedure for the Sprite Palette object */
int pal_display_proc(int msg, DIALOG *d, int c);
// dp  is a pointer to an int containing the current foreground color
// dp2 is a pointer to an int containing the current background color

/* custom dialog procedure for the Palette selector object */
int pal_select_proc(int msg, DIALOG *d, int c);
// dp is a pointer to a palette_select structure containing the palette 
//    properly filled in. 
//
// dp2 is a pointer to a callback function with a prototype of:
//     int func(DIALOG *d, int color, int mouse);
//     which gets passed the color value [0..ncolors] of the mouse click, and
//     a flag stating whether the left or right mouse button was clicked
//     it should return D_REDRAW if the dialog is to be redrawn or D_O_K
