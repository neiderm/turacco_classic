# turaco
The final version of Turaco? (1.1.3)
http://umlautllama.com/projects/turaco/
http://umlautllama.com/projects/turaco/r/T113_BAK.ZIP
Ported to Linux.
Builds with allegro-4.4.2 


Not sure how useful it will be to anyone these days, but I thought it 
looked interesting and it kept me amused during a long plane ride for a 
business trip a while back.

You may want to check out the command-line version of turaco, which
allows graphics to be exported to an image file to be editted in your 
favorite graphics program and then dumped back to ROM format:

http://umlautllama.com/projects/turacocl/

... which I have imported from Google Code:

code.google.com/p/bleu-romtools


If you are interested in running the Turaco GUI:

You will first need to install the Allegro library:

http://cdn.allegro.cc/file/library/allegro/4.4.2/allegro-4.4.2.tar.gz

With Allegro installed, hopefully turaco will build by simply invoking 
the Makefile.

Once the program is compiled, there are just a few steps to set it up.


The turaco.ini will be created automatically when the program is first run. 
Be sure to set ROMPath (no ROMs included with this source-code prooject).

Turaco depends on "game drivers" that describe the geometry of the graphics 
encoded in the ROM file.  On my installation, I have copied e.g. 
"./drivers/galaganm.ini" from the turaco_cl project 
("bleu-romtools/turaco-drivers/NAMCO/GALAGANM.INI")

Driver name may be specified on the command line e.g.
  "LD_LIBRARY_PATH=/usr/local/lib/ ./turaco.exe galaganm

enjoy!
-GN
