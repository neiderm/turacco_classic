// bitmaped.h
//
//  bitmap editor UI module for allegro
//
//  September, 1998
//  jerry@mail.csh.rit.edu



/* custom dialog procedure for the bitmap editor object */
int bitmap_editor_proc(int msg, DIALOG *d, int c);
// dp is a pointer to a pointer to a bitmap.
//    the bitmap should be a multiple of 8 square.
//    the destination size should be a multiple of the source bitmap square
//    the source bitmap is used to determine the display scaling, so the
//    bitmap should be the same size as the amount of space to be edited.
//    (there should be no unused, dead space.)
//
// dp2 is a pointer to a callback function with a prototype of:
//     void func(DIALOG *d, int x, int y, int mouse);
//     which gets passed the x & y coordinate of the mouse click, and
//     a flag stating whether the left or right mouse button was clicked
//
// d1,d2 is the position of the regular size image (-1,-1 if none)

// new messages 
#define MSG_SIZE_CHANGE (MSG_USER+1)  /* for when the bitmap size changes */
