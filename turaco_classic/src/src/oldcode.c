// coding.c
//
//  Functions for converting between the sprite palette bitmaps
//  and the format of the memory rom images.
//
//  November, 1998
//   Ivan Mackintosh
//
//  Speedup hacks/rewrite December 1998 Jerry

#include <stdio.h>
#include "allegro.h"
#include "sprtplte.h"
#include "general.h"
#include "gamedesc.h"
#include "config.h"
#include "coding.h"
#include "toolmenu.h"


int GetBit(long startbit)
{
   long nbyte = startbit>>3;
   int nbit = startbit%8;
   int mask = 0x80 >> nbit;

   return ((GfxRomData[nbyte] & mask) == mask);
}

#define ___getsprite___()\
    result = 0;                                               \
    if (gbe->planes == 1 && plane1_hack)                      \
    {                                                         \
	for (plane = 0; plane < gbe->planes; plane++)         \
	if (y==4)                                             \
	{                                                     \
	    if (GetBit( offs1 +                               \
		    gbe->xoffset[(x+4)%sp->sprite_w] +        \
		    gbe->yoffset[y] +                         \
		    gbe->planeoffsets[0]))                    \
	        result |= 1;                                  \
	} else {                                              \
	    if (GetBit(offs1 +                                \
		       gbe->xoffset[x] +                      \
		       gbe->yoffset[y] +                      \
		       gbe->planeoffsets[plane]))             \
		    result |= 1;                              \
	}                                                     \
    } else {                                                  \
	for (plane = 0; plane < gbe->planes; plane++)         \
	{                                                     \
	    if (GetBit(offs1 +                                \
		       gbe->xoffset[x] +                      \
		       gbe->yoffset[y] +                      \
		       gbe->planeoffsets[plane]))             \
		result |= (1 << (gbe->planes - 1 - plane));   \
	}                                                     \
    }

void Decode_Normal(int CurrentBank)
{
    GFXBANKEXTRA * gbe = &GfxBankExtraInfo[CurrentBank];
    SPRITE_PALETTE * sp = &GfxBanks[CurrentBank];
    int x, y, spriteno, plane;
    long offs1;
    BYTE result = 0;
    long StartBit = gbe->startaddress * 8;
    BITMAP * tbmp = create_bitmap(sp->sprite_w,sp->sprite_h);

    clear_to_color(tbmp, FIRST_USER_COLOR);

    for (spriteno = 0; spriteno < sp->n_total; spriteno++)
    {
	offs1 = (gbe->charincrement * spriteno) + StartBit;
	// copy the current character into a bitmap
	for (y = 0; y < sp->sprite_h; y++)
	{
	    for (x = 0; x < sp->sprite_w; x++)
	    {
		___getsprite___();
		putpixel(tbmp, x, y, result + FIRST_USER_COLOR);
	    }
	}

	// now copy it to the appropriate place in the master gfxbank bitmap
	blit(tbmp, sp->bmp, 0, 0, spriteno * sp->sprite_w, 
	     0, sp->sprite_w, sp->sprite_h);
    }

    destroy_bitmap(tbmp);
}

void Decode_Flip_X(int CurrentBank)
{
    GFXBANKEXTRA * gbe = &GfxBankExtraInfo[CurrentBank];
    SPRITE_PALETTE * sp = &GfxBanks[CurrentBank];
    int x, y, spriteno, plane;
    int rx;
    long offs1;
    BYTE result = 0;
    long StartBit = gbe->startaddress * 8;
    BITMAP * tbmp = create_bitmap(sp->sprite_w,sp->sprite_h);

    clear_to_color(tbmp, FIRST_USER_COLOR);

    for (spriteno = 0; spriteno < sp->n_total; spriteno++)
    {
	offs1 = (gbe->charincrement * spriteno) + StartBit;
	// copy the current character into a bitmap
	for (y = 0; y < sp->sprite_h; y++)
	{
	    for (x = 0; x < sp->sprite_w; x++)
	    {
		rx = sp->sprite_w-1-x;

		___getsprite___();

		putpixel(tbmp, rx, y, result + FIRST_USER_COLOR);
	    }
	}

	// now copy it to the appropriate place in the master gfxbank bitmap
	blit(tbmp, sp->bmp, 0, 0, spriteno * sp->sprite_w, 
	     0, sp->sprite_w, sp->sprite_h);
    }

    destroy_bitmap(tbmp);
}

void Decode_Flip_Y(int CurrentBank)
{
    GFXBANKEXTRA * gbe = &GfxBankExtraInfo[CurrentBank];
    SPRITE_PALETTE * sp = &GfxBanks[CurrentBank];
    int x, y, spriteno, plane;
    int ry;
    long offs1;
    BYTE result = 0;
    long StartBit = gbe->startaddress * 8;
    BITMAP * tbmp = create_bitmap(sp->sprite_w,sp->sprite_h);

    clear_to_color(tbmp, FIRST_USER_COLOR);

    for (spriteno = 0; spriteno < sp->n_total; spriteno++)
    {
	offs1 = (gbe->charincrement * spriteno) + StartBit;
	// copy the current character into a bitmap
	for (y = 0; y < sp->sprite_h; y++)
	{
	    ry = sp->sprite_h-1-y;
	    for (x = 0; x < sp->sprite_w; x++)
	    {
		___getsprite___();

		putpixel(tbmp, x, ry, result + FIRST_USER_COLOR);
	    }
	}

	// now copy it to the appropriate place in the master gfxbank bitmap
	blit(tbmp, sp->bmp, 0, 0, spriteno * sp->sprite_w, 
	     0, sp->sprite_w, sp->sprite_h);
    }

    destroy_bitmap(tbmp);
}

void Decode_Rotate_90(int CurrentBank)
{
    GFXBANKEXTRA * gbe = &GfxBankExtraInfo[CurrentBank];
    SPRITE_PALETTE * sp = &GfxBanks[CurrentBank];
    int x, y, spriteno, plane;
    int ry;
    long offs1;
    BYTE result = 0;
    long StartBit = gbe->startaddress * 8;
    BITMAP * tbmp = create_bitmap(sp->sprite_w,sp->sprite_h);

    clear_to_color(tbmp, FIRST_USER_COLOR);

    for (spriteno = 0; spriteno < sp->n_total; spriteno++)
    {
	offs1 = (gbe->charincrement * spriteno) + StartBit;
	// copy the current character into a bitmap
	for (y = 0; y < sp->sprite_h; y++)
	{
	    ry = sp->sprite_h-1-y;
	    for (x = 0; x < sp->sprite_w; x++)
	    {
		___getsprite___();

		putpixel(tbmp, ry, x, result + FIRST_USER_COLOR);
	    }
	}

	// now copy it to the appropriate place in the master gfxbank bitmap
	blit(tbmp, sp->bmp, 0, 0, spriteno * sp->sprite_w, 
	     0, sp->sprite_w, sp->sprite_h);
    }

    destroy_bitmap(tbmp);
}

void Decode_Rotate_180(int CurrentBank)
{
    GFXBANKEXTRA * gbe = &GfxBankExtraInfo[CurrentBank];
    SPRITE_PALETTE * sp = &GfxBanks[CurrentBank];
    int x, y, spriteno, plane;
    int rx,ry;
    long offs1;
    BYTE result = 0;
    long StartBit = gbe->startaddress * 8;
    BITMAP * tbmp = create_bitmap(sp->sprite_w,sp->sprite_h);

    clear_to_color(tbmp, FIRST_USER_COLOR);

    for (spriteno = 0; spriteno < sp->n_total; spriteno++)
    {
	offs1 = (gbe->charincrement * spriteno) + StartBit;
	// copy the current character into a bitmap
	for (y = 0; y < sp->sprite_h; y++)
	{
	    ry = sp->sprite_h-1-y;
	    for (x = 0; x < sp->sprite_w; x++)
	    {
		rx = sp->sprite_w-1-x;
		___getsprite___();

		putpixel(tbmp, rx, ry, result + FIRST_USER_COLOR);
	    }
	}

	// now copy it to the appropriate place in the master gfxbank bitmap
	blit(tbmp, sp->bmp, 0, 0, spriteno * sp->sprite_w, 
	     0, sp->sprite_w, sp->sprite_h);
    }

    destroy_bitmap(tbmp);
}

void Decode_Rotate_270(int CurrentBank)
{
    GFXBANKEXTRA * gbe = &GfxBankExtraInfo[CurrentBank];
    SPRITE_PALETTE * sp = &GfxBanks[CurrentBank];
    int x, y, spriteno, plane;
    int rx,ry;
    long offs1;
    BYTE result = 0;
    long StartBit = gbe->startaddress * 8;
    BITMAP * tbmp = create_bitmap(sp->sprite_w,sp->sprite_h);

    clear_to_color(tbmp, FIRST_USER_COLOR);

    for (spriteno = 0; spriteno < sp->n_total; spriteno++)
    {
	offs1 = (gbe->charincrement * spriteno) + StartBit;
	// copy the current character into a bitmap
	for (y = 0; y < sp->sprite_h; y++)
	{
	    ry = sp->sprite_h-1-y;
	    for (x = 0; x < sp->sprite_w; x++)
	    {
		rx = sp->sprite_w-1-x;
		___getsprite___();

		putpixel(tbmp, y, rx, result + FIRST_USER_COLOR);
	    }
	}

	// now copy it to the appropriate place in the master gfxbank bitmap
	blit(tbmp, sp->bmp, 0, 0, spriteno * sp->sprite_w, 
	     0, sp->sprite_w, sp->sprite_h);
    }

    destroy_bitmap(tbmp);
}



void Decode(int CurrentBank)
{
    switch(Orientation)
    {
    case ORIENTATION_FLIP_X:
	Decode_Flip_X(CurrentBank);
	break;
    case ORIENTATION_FLIP_Y:
	Decode_Flip_Y(CurrentBank);
	break;
    case ORIENTATION_ROTATE_180:
    case ORIENTATION_SWAP_XY:
	Decode_Rotate_180(CurrentBank);
	break;
    case ORIENTATION_ROTATE_90:
	Decode_Rotate_90(CurrentBank);
	break;
    case ORIENTATION_ROTATE_270:
	Decode_Rotate_270(CurrentBank);
	break;
    default:
	Decode_Normal(CurrentBank);
	break;
    }
}



void PutBit(long startbit, int value)
{
    long nbyte = startbit>>3;
    int nbit = startbit%8;
    int mask = 0x80 >> nbit;

    GfxRomData[nbyte] &= ~mask;

    if (value)
	GfxRomData[nbyte] |= mask;
}



#define ___putsprite___(xxx,yyy)\
    if (gbe->planes == 1 && plane1_hack)                              \
    {                                                                 \
	if (y==4)                                                     \
	    PutBit(offs1 +                                            \
		gbe->xoffset[(x+4)%sp->sprite_w] +                    \
		gbe->yoffset[y] +                                     \
		gbe->planeoffsets[0],                                 \
		(getpixel(tbmp, (xxx), (yyy)) - FIRST_USER_COLOR)     \
		& 1 );                                                \
	else                                                          \
	    PutBit(offs1 +                                            \
		    gbe->xoffset[x] +                                 \
		    gbe->yoffset[y] +                                 \
		    gbe->planeoffsets[0],                             \
		    (getpixel(tbmp, (xxx), (yyy)) - FIRST_USER_COLOR) \
		    & 1 );                                            \
    } else {                                                          \
	for (plane = 0; plane < gbe->planes; plane++)                 \
	{                                                             \
	    PutBit(offs1 +                                            \
		    gbe->xoffset[x] +                                 \
		    gbe->yoffset[y] +                                 \
		    gbe->planeoffsets[plane],                         \
		    (getpixel(tbmp, (xxx), (yyy)) - FIRST_USER_COLOR) \
		    & (1 << (gbe->planes - 1 - plane)));              \
	}                                                             \
    }


void Encode_Normal(CurrentBank)
{
    GFXBANKEXTRA * gbe = &GfxBankExtraInfo[CurrentBank];
    SPRITE_PALETTE * sp = &GfxBanks[CurrentBank];
    int x, y, spriteno, plane;
    long StartBit = gbe->startaddress * 8;
    BITMAP * tbmp = create_bitmap(sp->sprite_w,sp->sprite_h);
    int offs1;

    for (spriteno = 0; spriteno < sp->n_total; spriteno++)
    {
	offs1 = (gbe->charincrement * spriteno) + StartBit;

	// copy the current char into the temp bitmap for rotation
	blit(sp->bmp, tbmp, spriteno * sp->sprite_w, 0, 
	      0, 0, sp->sprite_w, sp->sprite_h);

	for (y = 0; y < sp->sprite_h; y++)
	{
	    for (x = 0; x < sp->sprite_w; x++)
	    {
		___putsprite___(x,y);
	    }
	}
    }
    destroy_bitmap(tbmp);
}

void Encode_Flip_X(CurrentBank)
{
    GFXBANKEXTRA * gbe = &GfxBankExtraInfo[CurrentBank];
    SPRITE_PALETTE * sp = &GfxBanks[CurrentBank];
    int x, y, spriteno, plane;
    int rx;
    long StartBit = gbe->startaddress * 8;
    BITMAP * tbmp = create_bitmap(sp->sprite_w,sp->sprite_h);
    int offs1;

    for (spriteno = 0; spriteno < sp->n_total; spriteno++)
    {
	offs1 = (gbe->charincrement * spriteno) + StartBit;

	// copy the current char into the temp bitmap for rotation
	blit(sp->bmp, tbmp, spriteno * sp->sprite_w, 0, 
	      0, 0, sp->sprite_w, sp->sprite_h);

	for (y = 0; y < sp->sprite_h; y++)
	{
	    for (x = 0; x < sp->sprite_w; x++)
	    {
		rx = sp->sprite_w-1-x;

		___putsprite___(rx,y);
	    }
	}
    }
    destroy_bitmap(tbmp);
}

void Encode_Flip_Y(CurrentBank)
{
    GFXBANKEXTRA * gbe = &GfxBankExtraInfo[CurrentBank];
    SPRITE_PALETTE * sp = &GfxBanks[CurrentBank];
    int x, y, spriteno, plane;
    int ry;
    long StartBit = gbe->startaddress * 8;
    BITMAP * tbmp = create_bitmap(sp->sprite_w,sp->sprite_h);
    int offs1;

    for (spriteno = 0; spriteno < sp->n_total; spriteno++)
    {
	offs1 = (gbe->charincrement * spriteno) + StartBit;

	// copy the current char into the temp bitmap for rotation
	blit(sp->bmp, tbmp, spriteno * sp->sprite_w, 0, 
	      0, 0, sp->sprite_w, sp->sprite_h);

	for (y = 0; y < sp->sprite_h; y++)
	{
	    ry = sp->sprite_h-1-y;
	    for (x = 0; x < sp->sprite_w; x++)
	    {
		___putsprite___(x,ry);
	    }
	}
    }
    destroy_bitmap(tbmp);
}

void Encode_Rotate_90(CurrentBank)
{
    GFXBANKEXTRA * gbe = &GfxBankExtraInfo[CurrentBank];
    SPRITE_PALETTE * sp = &GfxBanks[CurrentBank];
    int x, y, spriteno, plane;
    int ry;
    long StartBit = gbe->startaddress * 8;
    BITMAP * tbmp = create_bitmap(sp->sprite_w,sp->sprite_h);
    int offs1;

    for (spriteno = 0; spriteno < sp->n_total; spriteno++)
    {
	offs1 = (gbe->charincrement * spriteno) + StartBit;

	// copy the current char into the temp bitmap for rotation
	blit(sp->bmp, tbmp, spriteno * sp->sprite_w, 0, 
	      0, 0, sp->sprite_w, sp->sprite_h);

	for (y = 0; y < sp->sprite_h; y++)
	{
	    ry = sp->sprite_h-1-y;
	    for (x = 0; x < sp->sprite_w; x++)
	    {
		___putsprite___(ry,x);
	    }
	}
    }
    destroy_bitmap(tbmp);
}

void Encode_Rotate_180(CurrentBank)
{
    GFXBANKEXTRA * gbe = &GfxBankExtraInfo[CurrentBank];
    SPRITE_PALETTE * sp = &GfxBanks[CurrentBank];
    int x, y, spriteno, plane;
    int rx,ry;
    long StartBit = gbe->startaddress * 8;
    BITMAP * tbmp = create_bitmap(sp->sprite_w,sp->sprite_h);
    int offs1;

    for (spriteno = 0; spriteno < sp->n_total; spriteno++)
    {
	offs1 = (gbe->charincrement * spriteno) + StartBit;

	// copy the current char into the temp bitmap for rotation
	blit(sp->bmp, tbmp, spriteno * sp->sprite_w, 0, 
	      0, 0, sp->sprite_w, sp->sprite_h);

	for (y = 0; y < sp->sprite_h; y++)
	{
	    ry = sp->sprite_h-1-y;
	    for (x = 0; x < sp->sprite_w; x++)
	    {
		rx = sp->sprite_w-1-x;

		___putsprite___(rx,ry);
	    }
	}
    }
    destroy_bitmap(tbmp);
}

void Encode_Rotate_270(CurrentBank)
{
    GFXBANKEXTRA * gbe = &GfxBankExtraInfo[CurrentBank];
    SPRITE_PALETTE * sp = &GfxBanks[CurrentBank];
    int x, y, spriteno, plane;
    int rx;
    long StartBit = gbe->startaddress * 8;
    BITMAP * tbmp = create_bitmap(sp->sprite_w,sp->sprite_h);
    int offs1;

    for (spriteno = 0; spriteno < sp->n_total; spriteno++)
    {
	offs1 = (gbe->charincrement * spriteno) + StartBit;

	// copy the current char into the temp bitmap for rotation
	blit(sp->bmp, tbmp, spriteno * sp->sprite_w, 0, 
	      0, 0, sp->sprite_w, sp->sprite_h);

	for (y = 0; y < sp->sprite_h; y++)
	{
	    for (x = 0; x < sp->sprite_w; x++)
	    {
		rx = sp->sprite_w-1-x;

		___putsprite___(y,rx);
	    }
	}
    }
    destroy_bitmap(tbmp);
}



void Encode(int CurrentBank)
{
    switch(Orientation)
    {
    case ORIENTATION_FLIP_X:
	Encode_Flip_X(CurrentBank);
	break;
    case ORIENTATION_FLIP_Y:
	Encode_Flip_Y(CurrentBank);
	break;
    case ORIENTATION_ROTATE_180:
    case ORIENTATION_SWAP_XY:
	Encode_Rotate_180(CurrentBank);
	break;
    case ORIENTATION_ROTATE_90:
	Encode_Rotate_90(CurrentBank);
	break;
    case ORIENTATION_ROTATE_270:
	Encode_Rotate_270(CurrentBank);
	break;
    default:
	Encode_Normal(CurrentBank);
	break;
    }
}


// If and old bank is specified (-1 for no old bank) then
// the bitmap for that bank is encoded into the memory roms image
// Once this is complete the bitmap for the new bank is then
// decode from the memory rom image.
void SwitchGraphicsBank(int oldbank, int newbank)
{
    // anything to encode?
    if (oldbank != -1)
	Encode(oldbank);

    // now decode the new one
    Decode(newbank);
}
