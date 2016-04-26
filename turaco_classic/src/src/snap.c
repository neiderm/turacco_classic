// ====================================================================
// snapshot utility
//
// January, 1998 jerry@mail.csh.rit.edu
//   http://www.csh.rit.edu/~jerry
// ====================================================================
//
// snap.c
//
//

#include <stdio.h>
#include <allegro.h>  
#include "snap.h"	// hey, we need these!
#ifdef SNAP_JPEG
#include <jpeg.h>
#endif
#include <dirent.h>	// for DIR
//#include <sys\stat.h>	// for mkdir


void snap(BITMAP *bmp, RGB * pal)
{
    DIR * dd;
    FILE * fp;
    char filename[80];
    int counter = 0;
    int done = 0;

    // first check if the directory exists.
    dd = opendir(SNAPDIR);
    if (dd)
	closedir(dd); // yup
    else 
	mkdir(SNAPDIR, S_IRUSR | S_IWUSR);

    while (!done && counter <= 9999)
    {
#ifdef SNAP_JPEG
	sprintf(filename, "%s/%s%04d.jpg", SNAPDIR, SNAPBASE, counter);
#else
	sprintf(filename, "%s/%s%04d.pcx", SNAPDIR, SNAPBASE, counter);
#endif
	fp = fopen(filename, "rb");
	if (fp)
	{
	    fclose(fp);
	    counter++;
	} else {
	    done = 1;
	}
    }

    if (counter <= 9999) // just in case...
    {
#ifdef SNAP_JPEG
	save_jpeg(filename, bmp, pal);
#else
done = // GN: returns -1
	save_pcx(filename, bmp, pal);
if (done != 0)
  return;
#endif
    }
}

void screen_snap(void)
{
    BITMAP * bmp = create_bitmap(SCREEN_W, SCREEN_H);
// GN: idfk
PALETTE _current_pallete;
get_pallete(_current_pallete);
    if (bmp == NULL) return;

    blit(screen, bmp, 0,0, 0,0, SCREEN_W, SCREEN_H);
#if 1 // GN:      // GN: wtf src/snap.c|71|error: ‘_current_pallete’ undeclared 
    snap(bmp, _current_pallete);
#endif
    destroy_bitmap(bmp);
}

