#
# Makefile for the New Arcade Graphics Editor (AGE 2 / New Age / Turaco)
#
# Project by:
#        Scott "Jerry" Lawrence (jerry@mail.csh.rit.edu)
#               Ivan Mackintosh (ivan@rcp.co.uk)
#


# if you're using DEGUI, this next line should read:
#   LIBS = -ldegui -lalleg
# if you don't want to use DEGUI, the next line should read:
#   LIBS = -lalleg

LIBS = -lalleg 
LIBS = `allegro-config --libs`  # GN: allegro-4.2.3

# also if you're using degui, the following line should read:
#  DEGUI = -DUSE_DEGUI
# otherwise remove the line

DEGUI = 

# misc other options.
# currently available:
#    NO_FADE             disables exit fadeout
OTHER_OPTIONS = -DNO_FADE

# the name of the source .zip files
SOURCE_ZIP = t113_bak.zip
DRIVER_ZIP = d113_bak.zip

OLD_SOURCE_ZIP = t113_old.zip
OLD_DRIVER_ZIP = d113_old.zip

# distributions
DISTZIP = trco_113.zip

DRIVZIP = driv_113.zip


#
# Compiler flags
#

# first the flags that are the same for all configurations...
SFLAGS =  -s -Wall -Iinclude

# now the specific ones... (uncomment your favorite)
# 
CFLAGS = $(SFLAGS)
#  yields big executable (and compiles the fastest!)
#
#CFLAGS = $(SFLAGS) -O3 -finline-functions -funroll-all-loops -fomit-frame-pointer 
#  yields smaller executable
#
#CFLAGS = $(SFLAGS) -O3 
#  yields even smaller executable
#
#CFLAGS = $(SFLAGS) -O2 -fomit-frame-pointer
#  yields still yet smaller executable
#
# -O2 is great for size reduction, -O3 turns on -finline-functions
#  which chews up lotsa space for the executable.  
#  Since it doesn't need to be superfast, -O2 should be enough


OBJS = \
	gui/animview.o \
	gui/bitmaped.o \
	gui/sprtplte.o \
	gui/guipal.o \
	gui/button.o \
	gui/drivsel.o \
	gui/bmpdisp.o \
	gui/util.o \
	src/main.o \
	src/font.o \
	src/list.o \
	src/dlg.o \
	src/config.o \
	src/snap.o \
	src/editmenu.o \
	src/toolmenu.o \
	src/editmode.o \
	src/help.o \
	src/sprite.o \
	src/fileio.o \
	src/palette.o \
	src/texted.o \
	src/maped.o \
	src/inidriv.o \
	src/iload.o \
	src/gamedesc.o \
	src/coding.o \
	src/life.o \
	src/animate.o \
	maped/pacman.o


all: turaco.exe

# build the main executable:
turaco.exe: $(OBJS) $(MAPED) 
	gcc -g -o turaco.exe $(OBJS) $(LIBS)
	
%.o:   %.c 
	gcc $(CFLAGS) $(DEGUI) $(OTHER_OPTIONS) -c -o $@ $<


# now the targets for building the driver zip file, source distribution 
# zipfile, and binary distribution zipfile.  (as well as a few extra
# utility functions)

# build up the drivers
driver_file_sort:
	mkdir drivers
	cd drivers
	pkunzip -o -d ../drvsrc/mamedump.zip
	pkunzip -o -d ../drvsrc/t_100.zip
	pkunzip -o -d ../drvsrc/age_dump.zip
	call ..\txt\drivers.bat
	cp ../txt/gameboy.ini .
	cp ../txt/drivers.txt .
	cp ../txt/inidriv.txt .
	cd ..

# generate the driver distribution zip file
driverzip: driver_file_sort
	pkzip -ex -rP $(DRIVZIP) drivers\*.*

cleandrivers:
	rm -rf drivers

# binary distribution zip file
distrib: turaco.exe
	strip turaco.exe
	pkzip -ex $(DISTZIP) turaco.exe
	pkzip -ex $(DISTZIP) turaco.ini
	pkzip -ex $(DISTZIP) txt\whatsnew*.txt
	pkzip -ex $(DISTZIP) txt\readme*.txt
	pkzip -ex $(DISTZIP) txt\inidriv*.txt

# clean target
clean:
	rm -f gui/*.o # if exist gui\*.o del gui\*.o
	rm -f src/*.o # if exist src\*.o del src\*.o
	rm -f maped/*.o # if exist maped\*.o del maped\*.o
	rm -f *.pcx # if exist *.pcx del *.pcx
	rm -f turaco.exe # if exist turaco.exe del turaco.exe
	# if exist $(DISTZIP) rm $(DISTZIP)
	# if exist $(DRIVZIP) rm $(DRIVZIP)
	rm -rf snapshot

# if you don't have the unix-style commands: rm, mv, cp, you need to 
# get them from the File Utils package available at any DJGPP/GCC 
# distribution site..   ya need them for this part...
backup: clean
	if exist \zip\$(OLD_SOURCE_ZIP) del \zip\$(OLD_SOURCE_ZIP)
	if exist \zip\$(SOURCE_ZIP) copy \zip\$(SOURCE_ZIP) \zip\$(OLD_SOURCE_ZIP)
	if exist \zip\$(SOURCE_ZIP) del \zip\$(SOURCE_ZIP)
	pkzip -rP -ex \zip\$(SOURCE_ZIP) include\*.*
	pkzip -rP -ex \zip\$(SOURCE_ZIP) gui\*.*
	pkzip -rP -ex \zip\$(SOURCE_ZIP) src\*.*
	pkzip -rP -ex \zip\$(SOURCE_ZIP) maped\*.*
	pkzip -rP -ex \zip\$(SOURCE_ZIP) expdriv\*.*
	pkzip -rP -ex \zip\$(SOURCE_ZIP) testbin\*.*
	pkzip -rP -ex \zip\$(SOURCE_ZIP) txt\*.*
	pkzip -ex \zip\$(SOURCE_ZIP) make*
	pkzip -ex \zip\$(SOURCE_ZIP) turaco*.ini

foobackup:
	if exist \zip\$(OLD_DRIVER_ZIP) del \zip\$(OLD_DRIVER_ZIP)
	if exist \zip\$(DRIVER_ZIP) copy \zip\$(DRIVER_ZIP) \zip\$(OLD_DRIVER_ZIP)
	if exist \zip\$(DRIVER_ZIP) del \zip\$(DRIVER_ZIP)
	pkzip -rP -e0 \zip\$(DRIVER_ZIP) drvsrc\*.*

# make a backup onto a floppy disk 
#   -- good for develpment on multiple machines
floppy: backup
	if exist a:\$(SOURCE_ZIP) del a:\$(SOURCE_ZIP)
	cp \\zip\\$(SOURCE_ZIP) a:\\
#if exist a:\$(DRIVER_ZIP) del a:\$(DRIVER_ZIP)
#cp \\zip\\$(DRIVER_ZIP) a:\\


# and another two so I can start it going as I get a bite
# to eat from the kitchen...

# clean, backup, then regenerate the executable
lunch: backup turaco.exe

# clean, backup, backup to floppy, then regenerate the executable
dinner: floppy turaco.exe

# clean, backup, backup to floppy, driver, executable, distrib 
wholeshebang: floppy driverzip distrib
	if exist a:\$(DISTZIP) del a:\$(DISTZIP)
	if exist a:\$(DRIVZIP) del a:\$(DRIVZIP)
	mv $(DISTZIP) $(DRIVZIP) a:

