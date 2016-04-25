// drivsel.h
//
//  The file selector functions
//
//  November, 1998
//  jerry@mail.csh.rit.edu


extern char selected_filename[];

void game_driver_selector(void);



// for the driver directories...

extern char ** dirlist;
extern int ndirs;
void create_dir_list(void);
void destroy_dir_list(void);

