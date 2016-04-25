// config.h
//
//  user configuration stuff
//
//  October, 1998


// NOTE: if any other functions need to use any other .ini files,
//       be sure to use the push_config_state() and pop_config_state()
//       function calls!!!


extern int ini_version_major;
extern int ini_version_minor;

extern char InitGameName[];       // as used by load selector

#define ROM_PATH_LEN (1024)
extern char ROMPath[];

extern int drv_subdirs;
extern int troll_magic;

extern FONT * original_font;

void Init_INI(void);   
// initialize the ini system

void DeInit_INI(void);
// deinitialize the ini system

void Load_INI(void);
// load in all internal bits from the .ini file!

void Save_INI(void);
// save out all internal bits to be stored
