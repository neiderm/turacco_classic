// texted.c
//
//  the text editor 
//
//  November, 1998
//  jerry@mail.csh.rit.edu

#include "allegro.h"
#include "general.h"
#include "texted.h"

int editors_text(void)
{
    alert("Sorry, The text editor", 
          "is not yet supported.", 
          NULL,
          "&Ok", NULL, 0,0);

    return D_O_K;
}
