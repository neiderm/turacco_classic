// main.c

//
//  The main application file..
//
//  September, 1998
//  jerry@mail.csh.rit.edu

char title_text[]   = "Turaco v1.1.3";
char title_date[]   = "07 Oct 1999";

#include <stdlib.h>
#include <stdio.h>
#include <string.h> // for strncpy
#include <time.h>
#include <conio.h>

#include "allegro.h"
#include "font.h"
#include "general.h"
#include "sprtplte.h"
#include "guipal.h"
#include "palette.h"
#include "config.h"
#include "editmenu.h"
#include "gamedesc.h"
#include "inidriv.h"
#include "sprite.h"
#include "list.h"
#include "editmode.h"

char command_line_driver[127]; // if a driver was named on the command line...

int gfx_hres = 320;
int gfx_vres = 240; // 480 also works well..

volatile int timer_ticks = 0;

void usage(void)
{
    printf("\nUsage:\n");
    printf("    TURACO.EXE [ game | -help | -v ]\n");
    printf("\t game     \tload the specified game\n");
    printf("\t -help    \tdisplay this information\n");
    printf("\t -v       \tdisplay version information\n");
    printf("\n");
    printf("    TURACO.EXE [ -list | -listhtml | -listfull | -listroms ]\n");
    printf("\t -list    \tlist all of the available drivers\n");
    printf("\t -listhtml\tlist all of the available drivers in html format\n");
    printf("\t -listfull\tlist all of the drivers with extra info\n");
    printf("\t -listroms\tlist all of the drivers' required roms\n");
}

void version_info(void)
{
    printf("\n");
    printf("%s  (%s)\n", title_text, title_date);
    //printf("\tTuraco.ini version %d.%d\n", 
    //		ini_version_major, ini_version_minor);
    printf("\tBuilt: %s, %s using:\n", __DATE__, __TIME__);
    printf("\t       Allegro v%s (%s)\n", ALLEGRO_VERSION_STR, ALLEGRO_DATE_STR);
#ifdef USE_DEGUI
    printf("\t       DEGUI v%s (%d)\n", DEGUI_VERSION_STR, DEGUI_DATE);
#endif
    printf("\t       DJGPP v%s\n", __VERSION__);
}


#define RET_DOIT     (0)
#define RET_VERSION  (1)
#define RET_USAGE    (2)
#define RET_LIST     (3)
#define RET_LISTFULL (4)
#define RET_LISTHTML (5)
#define RET_LISTROMS (6)

int parse_command_line(int argc, char** argv)
{
    // return 0 to go into graphics mode, 
    // return 1 to exit immediately

    if (argc == 1)  return RET_DOIT;
    if (argc > 2)   return RET_USAGE; 

    command_line_driver[0] = '\0';

    if (argv[1][0] == '-')   // it wants to be a command line option
    {
	if (!strcmp(argv[1], "--version"))  return RET_VERSION;
	if (!strcmp(argv[1], "-v"))         return RET_VERSION;

	if (!strcmp(argv[1], "-help"))  return RET_USAGE;
	if (!strcmp(argv[1], "-h"))     return RET_USAGE;
	if (!strcmp(argv[1], "-?"))     return RET_USAGE;

	if (!strcmp(argv[1], "-list"))      return RET_LIST;
	if (!strcmp(argv[1], "-listfull"))  return RET_LISTFULL;
	if (!strcmp(argv[1], "-listhtml"))  return RET_LISTHTML;
	if (!strcmp(argv[1], "-listroms"))  return RET_LISTROMS;

	return RET_USAGE;
    } else 
        strncpy(command_line_driver, argv[1], 126);

    return RET_DOIT;
}

void end_blurb(void)
{
    printf("%s  (%s)\n\n", title_text, title_date);
    printf("A re-write of \"AGE\", The Arcade Games Editor\n\n");
    printf("Written by:\n");
    printf("\tScott \"Jerry\" Lawrence  jerry@mail.csh.rit.edu\n");
    printf("\tIvan Mackintosh         ivan@rcp.co.uk\n");

    printf("\nSpecial thanks to:\n");
    printf("\tChris \"Zwaxy\" Moore, M.C. Silvius, David Caldwell\n");

    printf("\tAll of those involved with MAME, and other emulators,\n");
    printf("\tAll of those who wrote the original games.\n");
    printf("\n\t\t\t   THANKS!\n");
    printf("\n");
    printf("Need help?  Have suggestions?  Want the latest version?  Want\n");
    printf("to ask us something?  Go to the message board, and homepage at:\n");
    printf("\n");
    printf("\thttp://www.csh.rit.edu/~jerry/turaco/\n");
    printf("\n");
}


void setup_palette(void)
{
#ifndef USE_DEGUI
    RGB cf  = {32/4,32/4,32/4};
    RGB cm  = {128/4,128/4,128/4};
    RGB cb  = {192/4,192/4,192/4};
    RGB cl  = {255/4,255/4,255/4};
    RGB cd  = {92/4,92/4,92/4};
    RGB cs  = {128/4,192/4,128/4};
    RGB cds = {220/4,220/4,220/4};
    RGB cdi = {128/4,128/4,128/4};
#endif
    RGB tcd, tcm, tcl, tc;

    set_pallete(desktop_pallete);
#ifdef USE_DEGUI
    set_default_object_colors ( UI_COLOR_0, UI_COLOR_0+1, UI_COLOR_0+2,
      UI_COLOR_0+3, UI_COLOR_0+4, UI_COLOR_0+5, UI_COLOR_0+6, UI_COLOR_0+7);
#else 
    // Set the default colors
    set_color(UI_COLOR_0 + 0,&cf);
    set_color(UI_COLOR_0 + 1,&cm);
    set_color(UI_COLOR_0 + 2,&cb);
    set_color(UI_COLOR_0 + 3,&cl);
    set_color(UI_COLOR_0 + 4,&cd);
    set_color(UI_COLOR_0 + 5,&cs);
    set_color(UI_COLOR_0 + 6,&cds);
    set_color(UI_COLOR_0 + 7,&cdi);
#endif

#ifdef AQUA_CURSOR_COLORS
    tcd.r = 5;  tcd.g = 20; tcd.b = 20;
    tcm.r = 10; tcm.g = 40; tcm.b = 40;
    tcl.r = 34; tcl.g = 56; tcl.b = 56;
#endif

#ifdef RED_CURSOR_COLORS
    tcd.r = 20; tcd.g = 0; tcd.b = 0;
    tcm.r = 40; tcm.g = 0; tcm.b = 0;
    tcl.r = 60; tcl.g = 0; tcl.b = 0;
#endif

#ifdef AMIGA_CURSOR_COLORS
    tcd.r =  0; tcd.g =  0; tcd.b =  0;
    tcm.r = 56; tcm.g = 16; tcm.b = 16;
    tcl.r = 56; tcl.g = 56; tcl.b = 48;
#endif


    set_color(MOUSE_COLOR_DARK,  &tcd);
    set_color(MOUSE_COLOR_MID,   &tcm);
    set_color(MOUSE_COLOR_LIGHT, &tcl);

    tc.r = 0;  tc.g = 0;  tc.b = 0;
    set_color(0, &tc);

    tcl.r = 63; tcl.g = 63; tcl.b = 30;
    set_color(COLOR_HILITE, &tcl);

    tcd.r = 30; tcd.g = 30; tcd.b = 63;
    set_color(COLOR_LOLITE, &tcd);

#ifndef USE_DEGUI
    // in case we're not using the degui functions, 
    // setup the allegro versions...
    gui_fg_color = GUI_FORE;
    gui_bg_color = GUI_BACK;
    gui_mg_color = GUI_DISABLE;
#endif

#define AUX_INTENSITY (40)
    //auxiliary colors for the palette selector...
    tc.r = AUX_INTENSITY;  tc.g = 0;  tc.b = 0;
    set_color(COLOR_RED, &tc);
    tc.r = 0;  tc.g = AUX_INTENSITY;  tc.b = 0;
    set_color(COLOR_GREEN, &tc);
    tc.r = 0;  tc.g = 0;  tc.b = AUX_INTENSITY;
    set_color(COLOR_BLUE, &tc);
}

void timer_handler(void)
{
    timer_ticks++;
}
END_OF_FUNCTION(timer_handler);

// initialize all of the non-allegro, TURACO subsystems
void Init_Subsystems(void)
{
    original_font = font; 
    font = &my_new_font; 
    Init_INI();   
    Init_Palette();
    InitialiseGameDesc();
    Init_Cursors();

    // now some timer stuff...
    i_love_bill = TRUE;
    install_timer();
    install_int(&timer_handler, 100);

    LOCK_VARIABLE(timer_ticks);
    LOCK_FUNCTION(timer_handler);
}

// de-initialize all of the non-allegro, TURACO subsystems
void DeInit_Subsystems(void)
{
    Save_INI();   
    DeInit_INI();   
    FreeDriver();
    DeInit_Palette();
    DeInit_Cursors();

    if (clipboard != NULL) destroy_bitmap(clipboard);
}


int setup_gfx_mode(void)
{
    // try to set the resolution to the mode as selected in the 
    // ini file.  if it fails, fall back on 320x240

    // check for minimum resolution (320x240)
    if (gfx_hres < 320  ||  gfx_vres < 240)
    {
	printf("Video resolution must be at least 320x240!\n");
	gfx_hres = 320;
	gfx_vres = 240;
    }

    if (set_gfx_mode(GFX_AUTODETECT, gfx_hres, gfx_vres, 0, 0) < 0)
    {
	// ACK! it failed... fall back on 320x240 (known working on all systems)
	gfx_hres = 320;
	gfx_vres = 240;
	if (set_gfx_mode(GFX_AUTODETECT, gfx_hres, gfx_vres, 0, 0) < 0)
	    return 1; // major VGA failure... can't do anything...
    }
    return 0;
}

int main(int argc, char ** argv)
{
    int ret;

    allegro_init();

    ret = parse_command_line(argc, argv);
    if ( ret == RET_DOIT )
    {
	install_keyboard(); 
	install_mouse();
	Init_Subsystems();

	if (setup_gfx_mode())
	{
	    // unable to select gfx mode
	    printf("Unable to select graphics mode\n");
	} else {
	    REGULAR_CURSOR();
	    setup_palette();

	    do_dialog(main_dialog, -1);
	    set_gfx_mode(GFX_TEXT,80,25,0,0);
	}
	DeInit_Subsystems();

	allegro_exit();
	end_blurb();

    } else {

	allegro_exit();
	if (ret < RET_USAGE)
	    end_blurb();

	if (ret == RET_USAGE)    usage();
	if (ret == RET_VERSION)  version_info();
	if (ret == RET_LIST)     list_games();
	if (ret == RET_LISTFULL) list_full();
	if (ret == RET_LISTHTML) list_html();
	if (ret == RET_LISTROMS) list_roms();
    }

    return 0;
}

/*
void foo(void)
{
    // dump out the font to a file.
    int fpos;
    FILE * fp;
    fp = fopen("font.bin", "w");
    if (fp)
    {
	fwrite (font->dat.dat_8x8, 1, 224*8, fp);
	fclose(fp);
    }
}
*/
