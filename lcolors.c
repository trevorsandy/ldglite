/*
 *  LDLITE, a program for viewing *.dat files.
 *  Copyright (C) 1998  Paul J. Gyugyi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#include "ldliteVR.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "string.h"

#undef GREYSCALE  // Define to force everything to 2-bit 4-level grey for Gameboy & WinCE devices

int zcolor_unalias(int index, char *name);
int zcolor_alias(int index, char *name);

ZCOLOR_DEF_TABLE_ENTRY zcolor_table_default[ZCOLOR_TABLE_DEFAULT_SIZE] = {
	{"Black",             8,{0x22,0x22,0x22,0xff},{0x22,0x22,0x22,0xff}}, // 0
	{"Blue",              9,{0x00,0x33,0xb2,0xff},{0x00,0x33,0xb2,0xff}}, // 1
	{"Green",            10,{0x00,0x7f,0x33,0xff},{0x00,0x7f,0x33,0xff}}, // 2
	{"Dark-Cyan",        11,{0x00,0xaa,0xaa,0xff},{0x00,0xaa,0xaa,0xff}}, // 3
	{"Red",              12,{0xcc,0x00,0x00,0xff},{0xcc,0x00,0x00,0xff}}, // 4
	{"Magenta",          13,{0xff,0x33,0x99,0xff},{0xff,0x33,0x99,0xff}}, // 5
	{"Brown",             0,{0x66,0x33,0x00,0xff},{0x66,0x33,0x00,0xff}}, // 6
	{"Light-Gray",        8,{0xaa,0xaa,0xaa,0xff},{0xaa,0xaa,0xaa,0xff}}, // 7
	{"Dark-Gray",         0,{0x66,0x66,0x58,0xff},{0x66,0x66,0x58,0xff}}, // 8
	{"Light-Blue",        1,{0x00,0x80,0xff,0xff},{0x00,0x80,0xff,0xff}}, // 9
	{"Light-Green",       2,{0x33,0xff,0x66,0xff},{0x33,0xff,0x66,0xff}}, // 10
	{"Cyan",              3,{0x55,0xaa,0xff,0xff},{0x55,0xaa,0xff,0xff}}, // 11
	{"Light-Red",         4,{0xff,0x55,0x55,0xff},{0xff,0x55,0x55,0xff}}, // 12
	{"Pink",              5,{0xff,0xb0,0xcc,0xff},{0xff,0xb0,0xcc,0xff}}, // 13
	{"Yellow",            8,{0xff,0xe5,0x00,0xff},{0xff,0xe5,0x00,0xff}}, // 14
	{"White",             8,{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff}}, // 15
	{"Main-Color",        0,{0x7f,0x7f,0x7f,0xff},{0x7f,0x7f,0x7f,0xff}}, // 16
	{"Pastel-Green",      2,{0x66,0xf0,0x99,0xff},{0x66,0xf0,0x99,0xff}}, // 17
	{"Light-Yellow",     14,{0xff,0xff,0x80,0xff},{0xff,0xff,0x80,0xff}}, // 18
	{"Tan",               6,{0xcc,0xaa,0x66,0xff},{0xcc,0xaa,0x66,0xff}}, // 19
	{"Light-Purple",      8,{0xe0,0xcc,0xf0,0xff},{0xe0,0xcc,0xf0,0xff}}, // 20
	{"Glow-In-The-Dark",  0,{0xe0,0xff,0xb0,0xff},{0xe0,0xff,0xb0,0xff}}, // 21
	{"Purple",            8,{0x99,0x33,0x99,0xff},{0x99,0x33,0x99,0xff}}, // 22
	{"Purple-Blue",       8,{0x4c,0x00,0xcc,0xff},{0x4c,0x00,0xcc,0xff}}, // 23
	{"Edge-Color",        0,{0x7f,0x7f,0x7f,0xff},{0x7f,0x7f,0x7f,0xff}}, // 24
	{"Orange-Solid",      8,{0xff,0x66,0x00,0xff},{0xff,0x66,0x00,0xff}}, // 25
	{"Dark-Pink",         0,{0xff,0x33,0x99,0xff},{0xff,0x33,0x99,0xff}}, // 26
	{"Lime-Green",        0,{0xad,0xdd,0x50,0xff},{0xad,0xdd,0x50,0xff}}, // 27
	{"Tan-Solid",         0,{0xcc,0xaa,0x66,0xff},{0xcc,0xaa,0x66,0xff}}, // 28
	{"Light_Purple",      0,{0xe4,0xad,0xc8,0xff},{0xe4,0xad,0xc8,0xff}}, // 29
	{"unused",            0,{0x7f,0x7f,0x7f,0xff},{0x7f,0x7f,0x7f,0xff}}, // 30
	{"unused",            0,{0x7f,0x7f,0x7f,0xff},{0x7f,0x7f,0x7f,0xff}}, // 31
	{"Trans-Black",       8,{0x22,0x22,0x22,0xff},{0x7f,0x7f,0x7f,0x00}}, // 32
	{"Trans-Blue",        9,{0x00,0x00,0x99,0xff},{0x7f,0x7f,0x7f,0x00}}, // 33
	{"Trans-Green",      10,{0x00,0x50,0x18,0xff},{0x7f,0x7f,0x7f,0x00}}, // 34
	{"Trans-Dark_Cyan",  11,{0x00,0xaa,0xaa,0xff},{0x7f,0x7f,0x7f,0x00}}, // 35
	{"Trans-Red",        12,{0xcc,0x00,0x00,0xff},{0x7f,0x7f,0x7f,0x00}}, // 36
	{"Trans-Magenta",    13,{0xff,0x33,0x99,0xff},{0x7f,0x7f,0x7f,0x00}}, // 37  
	{"Trans-Brown",       0,{0x66,0x33,0x00,0xff},{0x7f,0x7f,0x7f,0x00}}, // 38
	{"Trans-Light-Gray",  8,{0x99,0x99,0x99,0xff},{0x7f,0x7f,0x7f,0x00}}, // 39
	{"Trans-Dark-Gray",   0,{0x66,0x66,0x58,0xff},{0x7f,0x7f,0x7f,0x00}}, // 40
	{"Trans-Light-Blue",  1,{0x99,0xc0,0xf0,0xff},{0x7f,0x7f,0x7f,0x00}}, // 41
	{"Trans-Light-Green", 2,{0xcc,0xff,0x00,0xff},{0x7f,0x7f,0x7f,0x00}}, // 42
	{"Trans-Cyan",        3,{0x55,0xaa,0xff,0xff},{0x7f,0x7f,0x7f,0x00}}, // 43
	{"Trans-Light-Red",   4,{0xff,0x55,0x55,0xff},{0x7f,0x7f,0x7f,0x00}}, // 44
	{"Trans-Pink",        5,{0xff,0xb0,0xcc,0xff},{0x7f,0x7f,0x7f,0x00}}, // 45
	{"Trans-Yellow",      8,{0xf0,0xc4,0x00,0xff},{0x7f,0x7f,0x7f,0x00}}, // 46
	{"Trans-White",       8,{0xff,0xff,0xff,0xff},{0x7f,0x7f,0x7f,0x00}}, // 47
	{"unused",            0,{0x7f,0x7f,0x7f,0xff},{0x7f,0x7f,0x7f,0x00}}, // 48
	{"unused",            2,{0x7f,0x7f,0x7f,0xff},{0x7f,0x7f,0x7f,0x00}}, // 49
	{"unused",            0,{0x7f,0x7f,0x7f,0xff},{0x7f,0x7f,0x7f,0x00}}, // 50
	{"unused",            0,{0x7f,0x7f,0x7f,0xff},{0x7f,0x7f,0x7f,0x00}}, // 51
	{"unused",            0,{0x7f,0x7f,0x7f,0xff},{0x7f,0x7f,0x7f,0x00}}, // 52
	{"unused",            0,{0x7f,0x7f,0x7f,0xff},{0x7f,0x7f,0x7f,0x00}}, // 53
	{"unused",            0,{0x7f,0x7f,0x7f,0xff},{0x7f,0x7f,0x7f,0x00}}, // 54
	{"unused",            0,{0x7f,0x7f,0x7f,0xff},{0x7f,0x7f,0x7f,0x00}}, // 55
	{"unused",            0,{0x7f,0x7f,0x7f,0xff},{0x7f,0x7f,0x7f,0x00}}, // 56
	{"Trans-Orange",      8,{0xff,0x66,0x00,0xff},{0x7f,0x7f,0x7f,0x00}}, // 57
	{"unused",            0,{0x7f,0x7f,0x7f,0xff},{0x7f,0x7f,0x7f,0x00}}, // 58
	{"unused",            0,{0x7f,0x7f,0x7f,0xff},{0x7f,0x7f,0x7f,0x00}}, // 59
	{"unused",            0,{0x7f,0x7f,0x7f,0xff},{0x7f,0x7f,0x7f,0x00}}, // 60
	{"unused",            0,{0x7f,0x7f,0x7f,0xff},{0x7f,0x7f,0x7f,0x00}}, // 61
	{"unused",            0,{0x7f,0x7f,0x7f,0xff},{0x7f,0x7f,0x7f,0x00}}, // 62
	{"unused",            0,{0x7f,0x7f,0x7f,0xff},{0x7f,0x7f,0x7f,0x00}}  // 63
};

ZCOLOR_TABLE_ENTRY *zcolor_table_stack[MAX_ZCOLOR_DEPTH];
ZCOLOR_TABLE_ENTRY *zcolor_table;
ZCOLOR_TABLE_ENTRY *zcolor_prev_table;

typedef struct zcolor_namelist_entry_type {
	char *name;
	int color_value;
	struct zcolor_namelist_entry_type *next;
}	ZCOLOR_NAMELIST_ENTRY;

ZCOLOR_NAMELIST_ENTRY *zcolor_namelist_stack[MAX_ZCOLOR_DEPTH];

typedef struct zpoint_namelist_entry_type {
	char *name;
	vector3d point;
	struct zpoint_namelist_entry_type *next;
}	ZPOINT_NAMELIST_ENTRY;

ZPOINT_NAMELIST_ENTRY *zpoint_namelist_stack[MAX_ZCOLOR_DEPTH];

typedef struct zmatrix_namelist_entry_type {
	char *name;
	matrix3d matrix;
	struct zmatrix_namelist_entry_type *next;
}	ZMATRIX_NAMELIST_ENTRY;

ZMATRIX_NAMELIST_ENTRY *zmatrix_namelist_stack[MAX_ZCOLOR_DEPTH];

int znamelist_stack_index=0;

#ifdef USE_OPENGL

// Allow all 512 LDRAW colors to be redefined.
#undef ZCOLOR_TABLE_SIZE
#define ZCOLOR_TABLE_SIZE 512

/***************************************************************/
int alias_peeron_colors(void)
{
  #define UNKNOWN_COLOR 16

  zcolor_alias(0, "Black");
  zcolor_alias(1, "Blue");
  zcolor_alias(UNKNOWN_COLOR, "Brass");
  zcolor_alias(6, "Brown");
  zcolor_alias(10, "BtGreen");
  zcolor_alias(UNKNOWN_COLOR, "BurntOrange");
  zcolor_alias(UNKNOWN_COLOR, "Butterscotch");
  zcolor_alias(UNKNOWN_COLOR, "ChromeBlue");
  zcolor_alias(UNKNOWN_COLOR, "ChromeGreen");
  zcolor_alias(47, "Clear");
  zcolor_alias(UNKNOWN_COLOR, "DkFlatSilver");
  zcolor_alias(8, "DkGray");
  zcolor_alias(UNKNOWN_COLOR, "DkOrange");
  zcolor_alias(5, "DkPink");
  zcolor_alias(UNKNOWN_COLOR, "DkPurple");
  zcolor_alias(323, "DkRed");
  zcolor_alias(UNKNOWN_COLOR, "FlatGold");
  zcolor_alias(UNKNOWN_COLOR, "FlatSilver");
  zcolor_alias(21, "GlowInTheDark");
  zcolor_alias(334, "Gold");
  zcolor_alias(7, "Gray");
  zcolor_alias(2, "Green");
  zcolor_alias(UNKNOWN_COLOR, "Lime");
  zcolor_alias(11, "LtBlue");
  zcolor_alias(UNKNOWN_COLOR, "LtGray");
  zcolor_alias(UNKNOWN_COLOR, "LtGreen");
  zcolor_alias(UNKNOWN_COLOR, "LtOrange");
  zcolor_alias(UNKNOWN_COLOR, "LtPink");
  zcolor_alias(UNKNOWN_COLOR, "LtSwampGreen");
  zcolor_alias(UNKNOWN_COLOR, "LtTeal");
  zcolor_alias(UNKNOWN_COLOR, "LtViolet");
  zcolor_alias(18, "LtYellow");
  zcolor_alias(UNKNOWN_COLOR, "MdBlue");
  zcolor_alias(UNKNOWN_COLOR, "MetallicBlue");
  zcolor_alias(UNKNOWN_COLOR, "MetallicGreen");
  zcolor_alias(UNKNOWN_COLOR, "NavyBlue");
  zcolor_alias(UNKNOWN_COLOR, "NeonBlue");
  zcolor_alias(UNKNOWN_COLOR, "Olive");
  zcolor_alias(25, "Orange");
  zcolor_alias(UNKNOWN_COLOR, "PaleGreen");
  zcolor_alias(17, "PasGreen");
  zcolor_alias(UNKNOWN_COLOR, "Pearlescent");
  zcolor_alias(13, "Pink");
  zcolor_alias(22, "Purple");
  zcolor_alias(4, "Red");
  zcolor_alias(379, "SandBlue");
  zcolor_alias(378, "SandGreen");
  zcolor_alias(373, "SandPurple");
  zcolor_alias(335, "SandRed");
  zcolor_alias(383, "Silver");
  zcolor_alias(39, "Smoke");
  zcolor_alias(UNKNOWN_COLOR, "SwampGreen");
  zcolor_alias(19, "Tan");
  zcolor_alias(3, "Teal");
  zcolor_alias(33, "TrBlue");
  zcolor_alias(42, "TrFlYellow");
  zcolor_alias(34, "TrGreen");
  zcolor_alias(41, "TrLtBlue");
  zcolor_alias(UNKNOWN_COLOR, "TrLuWhite");
  zcolor_alias(57, "TrOrange");
  zcolor_alias(UNKNOWN_COLOR, "TrPink");
  zcolor_alias(UNKNOWN_COLOR, "TrPinkGlitter");
  zcolor_alias(UNKNOWN_COLOR, "TrPurple");
  zcolor_alias(36, "TrRed");
  zcolor_alias(UNKNOWN_COLOR, "TrViolet");
  zcolor_alias(46, "TrYellow");
  zcolor_alias(UNKNOWN_COLOR, "Violet");
  zcolor_alias(15, "White");
  zcolor_alias(14, "Yellow");
}
#endif

void zcolor_init()
{
	ZCOLOR_TABLE_ENTRY defcolor;
	int i;

	defcolor.inverse_index=0;
	defcolor.primary.r=0x7f;
	defcolor.primary.g=0x7f;
	defcolor.primary.b=0x7f;
	defcolor.primary.a=0xff;
	defcolor.dither.r=0x7f;
	defcolor.dither.g=0x7f;
	defcolor.dither.b=0x7f;
	defcolor.dither.a=0xff;


	znamelist_stack_index = 0;
	for(i=0; i<MAX_ZCOLOR_DEPTH; i++) {
		zcolor_table_stack[i] = NULL;
		zcolor_namelist_stack[i] = NULL;
		zpoint_namelist_stack[i] = NULL;
		zmatrix_namelist_stack[i] = NULL;
	}

	zcolor_table_stack[znamelist_stack_index] = 
		(ZCOLOR_TABLE_ENTRY *)malloc(ZCOLOR_TABLE_SIZE*sizeof(ZCOLOR_TABLE_ENTRY));
	if (zcolor_table_stack[znamelist_stack_index] == NULL) {
		zWrite("Fatal error: cannot malloc zcolor table\n");
		exit(-1);
	}
	zcolor_table = zcolor_table_stack[znamelist_stack_index];
	zcolor_prev_table = NULL;
	for (i=0; i<ZCOLOR_TABLE_SIZE; i++) {
		if (i<ZCOLOR_TABLE_DEFAULT_SIZE) {
			zcolor_table[i].inverse_index = zcolor_table_default[i].inverse_index;
			zcolor_table[i].primary = zcolor_table_default[i].primary;
			zcolor_table[i].dither = zcolor_table_default[i].dither;
			// add zcolor_table_default[i].name
			if (strcmp("unused",zcolor_table_default[i].name)) {
				// Store name in namelist
				zcolor_alias(i, zcolor_table_default[i].name);
			}
#ifdef USE_OPENGL
		} else if (i>255) {
		  // insert the LDRAW dithered colors in the bigger table.
		  ZCOLOR *zcp = &(zcolor_table[i].primary);
		  ZCOLOR *zcs = &(zcolor_table[i].dither);

		  zcolor_table[i].inverse_index = zcolor_table[(i & 0xf0)>>4].inverse_index;
		  *zcp = zcolor_table[(i & 0xf0)>>4].primary;
		  *zcs = zcolor_table[i & 0xf].primary;

		  // No dithering for opengl, just average the numbers  
		  zcp->r = (unsigned char) (((int)zcp->r + (int)zcs->r) / 2);
		  zcp->g = (unsigned char) (((int)zcp->g + (int)zcs->g) / 2);
		  zcp->b = (unsigned char) (((int)zcp->b + (int)zcs->b) / 2);
		  zcs = zcp;

		  // use colors borrowed from ldview
		  if ( i == 334) // Gold
		  {
		    zcp->r = zcs->r = 240; 
		    zcp->g = zcs->g = 176;
		    zcp->b = zcs->b = 51;
		  }
		  else if (( i == 383) // Chrome silver
			   || (i == 494)) // Electrical Contacts
		  {
		    zcp->r = zcs->r = 180; //204; // I prefer my plastic chrome a bit darker with 
		    zcp->g = zcs->g = 180; //204; // bright but fuzzy  specular highlights.
		    zcp->b = zcs->b = 180; //204;
		  }
#endif
		} else {
			zcolor_table[i] = defcolor;
		}
	}
	// some standard names
	zcolor_alias(383,"Chrome");
	zcolor_alias(494,"Electrical-Contacts");
	zcolor_alias(334,"Gold");
	zcolor_alias(495,"Light-Yellow-Belville");
	zcolor_alias(431,"Mint-Green-Belville");
	zcolor_alias(462,"Medium_Orange");
	zcolor_alias(382,"Tannish");

#ifdef USE_OPENGL
	// Add some standard colors from ldconfig.ldr
	// But force them to use ldconfig.ldr to override a few special colors.
	// 
	// The ldlite colors Tan and Tan-Solid were too similar.  Use ldcfgalt.ldr colors.
	zcolor_modify(19,"Tan", 0x2E2CBA0, 0xE4, 0xCD, 0x9E, 0xff, 0xE4, 0xCD, 0x9E, 0xff);
	zcolor_modify(28,"Dark_Tan", 0x2756A53, 0x95, 0x8A, 0x73, 0xff, 0x95, 0x8A, 0x73, 0xff);
	// 
	// Replace some BASIC transparent colors with new ones from ldconfig.ldr (not happy about this)
	zcolor_modify(32,"Trans_Black_IR_Lens", 0x205131D, 0x00, 0x0, 0x00, 0x7f, 0x95, 0x8A, 0x73, 0x7f);
	zcolor_modify(35,"Trans_Bright_Green", 0x29DA86B, 0xD9, 0xE4, 0xA7, 0x7f, 0xD9, 0xE4, 0xA7, 0x7f);
	zcolor_modify(38,"Trans_Neon_Orange", 0x2BD2400, 0xFF, 0x80, 0x0D, 0x7f, 0xFF, 0x80, 0x0D, 0x7f);
	zcolor_modify(39,"Trans_Very_Light_Blue", 0x285A3B4, 0xC1, 0xDF, 0xF0, 0x7f, 0xC1, 0xDF, 0xF0, 0x7f);
	zcolor_modify(44,"Trans_Light_Purple", 0x25A3463, 0x96, 0x70, 0x9F, 0x7f, 0x96, 0x70, 0x9F, 0x7f);
	// 
	// Replace some empty transparent colors with new ones from ldconfig.ldr (a much better idea)
	zcolor_modify(52,"Trans_Purple", 0x2280025, 0xA5, 0xA5, 0xCB, 0x7f, 0xA5, 0xA5, 0xCB, 0x7f);
	zcolor_modify(54,"Trans_Neon_Yellow", 0x2C3BA3F, 0xDA, 0xB0, 0x00, 0x7f, 0xDA, 0xB0, 0x00, 0x7f);
	// 
	// Replace some Basic TRANSPARENT colors with chromes from ldconfig.ldr (really unhappy now)
	//zcolor_modify(60,"Chrome_Antique_Brass", 0x2281E10, 0x64, 0x5A, 0x4C, 0xff, 0x64, 0x5A, 0x4C, 0xff);
	//zcolor_modify(61,"Chrome_Blue", 0x2202A68, 0x6C, 0x96, 0xBF, 0xff, 0x6C, 0x96, 0xBF, 0xff);
	//zcolor_modify(62,"Chrome_Green", 0x2007735, 0x3C, 0xB3, 0x71, 0xff, 0x3C, 0xB3, 0x71, 0xff);
	//zcolor_modify(63,"Chrome_Pink", 0x26E1152, 0xAA, 0x4D, 0x8E, 0xff, 0xAA, 0x4D, 0x8E, 0xff);
	zcolor_modify(60,"Chrome_Antique_Brass", 19, 0x64, 0x5A, 0x4C, 0xff, 0x64, 0x5A, 0x4C, 0xff);
	zcolor_modify(61,"Chrome_Blue", 17, 0x6C, 0x96, 0xBF, 0xff, 0x6C, 0x96, 0xBF, 0xff);
	zcolor_modify(62,"Chrome_Green", 17, 0x3C, 0xB3, 0x71, 0xff, 0x3C, 0xB3, 0x71, 0xff);
	zcolor_modify(63,"Chrome_Pink", 13, 0xAA, 0x4D, 0x8E, 0xff, 0xAA, 0x4D, 0x8E, 0xff);
	// This one is OK because its outside the translucent range.
	zcolor_modify(64,"Chrome_Black", 0x2000000, 0x1B, 0x2A, 0x34, 0xff, 0x1B, 0x2A, 0x34, 0xff);
	// 
	// LDraw Rubber Colours
	zcolor_modify(65, "Rubber_Yellow", 0x2333333, 0xF5, 0xCD, 0x2F, 0xff, 0xF5, 0xCD, 0x2F, 0xff);
	zcolor_modify(66, "Rubber_Trans_Yellow", 0x28E7400, 0xCA, 0xB0, 0x00, 0xff, 0xCA, 0xB0, 0x00, 0x00);
	zcolor_modify(67, "Rubber_Trans_Clear", 0x2C3C3C3, 0xFF, 0xFF, 0xFF, 0xff, 0xFF, 0xFF, 0xFF, 0x00);
	zcolor_modify(68, "Very_Light_Orange", 0x2333333, 0xF3, 0xCF, 0x9B, 0xff, 0xF3, 0xCF, 0x9B, 0xff);

	zcolor_modify(70,"Reddish_Brown", 8, 0x69, 0x40, 0x27, 0xff, 0x69, 0x40, 0x27, 0xff);
	zcolor_modify(71,"Stone_Gray", 0, 0xA3, 0xA2, 0xA4, 0xff, 0xA3, 0xA2, 0xA4, 0xff);
	zcolor_modify(72,"Dark_Stone_Gray", 0, 0x63, 0x5F, 0x61, 0xff, 0x63, 0x5F, 0x61, 0xff);
	zcolor_modify(134,"Pearl_Copper", 6, 0x93, 0x87, 0x67, 0xff, 0x93, 0x87, 0x67, 0xff);
	zcolor_modify(135,"Pearl_Gray", 8, 0xAB, 0xAD, 0xAC, 0xff, 0xAB, 0xAD, 0xAC, 0xff);
	zcolor_modify(142,"Pearl_Gold", 12, 0xD7, 0xA9, 0x4B, 0xff, 0xD7, 0xA9, 0x4B, 0xff);
	zcolor_modify(137,"Pearl_Sand_Blue", 1, 0x6A, 0x7A, 0x96, 0xff, 0x6A, 0x7A, 0x96, 0xff);
	//zcolor_modify(256,"Rubber_Black", 0x2000000, 0x21, 0x21, 0x21, 0xff, 0x21, 0x21, 0x21, 0xff);
	zcolor_modify(272,"Dark_Blue", 0x2000000, 0x00, 0x1D, 0x68, 0xff, 0x00, 0x1D, 0x68, 0xff);
	zcolor_modify(273,"Rubber_Blue", 0, 0x00, 0x33, 0xB2, 0xff, 0x00, 0x33, 0xB2, 0xff);
	zcolor_modify(288,"Dark_Green", 0x2000000, 0x27, 0x46, 0x2C, 0xff, 0x27, 0x46, 0x2C, 0xff);
	zcolor_modify(320,"Dark_Red", 0x2000000, 0x78, 0x00, 0x1C, 0xff, 0x78, 0x00, 0x1C, 0xff);
	zcolor_modify(324,"Rubber_Red", 0, 0xC4, 0x00, 0x26, 0xff, 0xC4, 0x00, 0x26, 0xff);
	//zcolor_modify(334,"Chrome_Gold", 14, 0xE1, 0x6E, 0x13, 0xff, 0xE1, 0x6E, 0x13, 0xff);
	zcolor_modify(335,"Sand_Red", 0, 0xBF, 0x87, 0x82, 0xff, 0xBF, 0x87, 0x82, 0xff);
	zcolor_modify(366,"Earth_Orange", 0, 0xD1, 0x83, 0x04, 0xff, 0xD1, 0x83, 0x04, 0xff);
	zcolor_modify(373,"Sand_Violet", 0, 0x84, 0x5E, 0x84, 0xff, 0x84, 0x5E, 0x84, 0xff);
	zcolor_modify(375,"Rubber_Gray", 8, 0xC1, 0xC2, 0xC1, 0xff, 0xC1, 0xC2, 0xC1, 0xff);
	zcolor_modify(378,"Sand_Green", 0, 0xA0, 0xBC, 0xAC, 0xff, 0xA0, 0xBC, 0xAC, 0xff);
	zcolor_modify(379,"Sand_Blue", 0, 0x6A, 0x7A, 0x96, 0xff, 0x6A, 0x7A, 0x96, 0xff);
	//zcolor_modify(383,"Chrome_Silver", 8, 0xE0, 0xE0, 0xE0, 0xff, 0xE0, 0xE0, 0xE0, 0xff);
	zcolor_modify(462,"Light_Orange", 0, 0xFE, 0x9F, 0x06, 0xff, 0xFE, 0x9F, 0x06, 0xff);
	zcolor_modify(484,"Dark_Orange", 0, 0xB3, 0x3E, 0x00, 0xff, 0xB3, 0x3E, 0x00, 0xff);
	//zcolor_modify(494,"Electric_Contact", 8, 0xD0, 0xD0, 0xD0, 0xff, 0xD0, 0xD0, 0xD0, 0xff);
	zcolor_modify(503,"Light_Gray", 0, 0xE6, 0xE3, 0xDA, 0xff, 0xE6, 0xE3, 0xDA, 0xff);
	zcolor_modify(511,"Rubber_White", 0, 0xFF, 0xFF, 0xFF, 0xff, 0xFF, 0xFF, 0xFF, 0xff);

	zcolor_modify(69,"Bright_Purple", 0, 0xCD, 0x62, 0x98, 0xff, 0xCD, 0x62, 0x98, 0xff);
	zcolor_modify(73,"Medium_Blue", 0, 0x6E, 0x99, 0xC9, 0xff, 0x6E, 0x99, 0xC9, 0xff);
	//zcolor_modify(74,"Medium_Green", 0, 0xA1, 0xC4, 0x8B, 0xff, 0xA1, 0xC4, 0x8B, 0xff);
	zcolor_modify(74,"Medium_Green", 0x333333, 0x73, 0xDC, 0xA1, 0xff, 0x73, 0xDC, 0xA1, 0xff);
	zcolor_modify(75,"Speckle_Black_Copper", 0x595959, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff);
	zcolor_modify(76,"Speckle_Dark_Bluish_Gray_Silver", 0x595959, 0x63, 0x5F, 0x61, 0xff, 0x63, 0x5F, 0x61, 0xff);
	zcolor_modify(77,"Paradisa_Pink", 0, 0xFE, 0xCC, 0xCC, 0xff, 0xFE, 0xCC, 0xCC, 0xff);
	zcolor_modify(78,"Light_Flesh", 8, 0xFA, 0xD7, 0xC3, 0xff, 0xFA, 0xD7, 0xC3, 0xff);
	zcolor_modify(79,"Translucent_White", 8, 0xFF, 0xFF, 0xFF, 0xE0, 0xFF, 0xFF, 0xFF, 0xE0);
	zcolor_modify(85,"Medium_Lilac", 0, 0x34, 0x2B, 0x75, 0xff, 0x34, 0x2B, 0x75, 0xff);
	zcolor_modify(86,"Dark_Flesh", 0, 0x7C, 0x5C, 0x45, 0xff, 0x7C, 0x5C, 0x45, 0xff);
	zcolor_modify(89,"Royal_Blue", 0, 0x9B, 0xB2, 0xEF, 0xff, 0x9B, 0xB2, 0xEF, 0xff);
	zcolor_modify(92,"Flesh", 0, 0xCC, 0x8E, 0x68, 0xff, 0xCC, 0x8E, 0x68, 0xff);
	zcolor_modify(151,"Light_Stone", 8, 0xE5, 0xE4, 0xDE, 0xff, 0xE5, 0xE4, 0xDE, 0xff);
	zcolor_modify(313,"Maersk_Blue", 0, 0x35, 0xA2, 0xBD, 0xff, 0x35, 0xA2, 0xBD, 0xff);

	zcolor_alias(256,"Matte-Black"); // rubber for tires.
	alias_peeron_colors(); // Gotta cross reference missing ones with ldconfig.ldr
#endif  
}

// Call this when a file finishes.
// If a color change caused a new table to be malloc'd, free it.
void znamelist_pop()
{
	int i,j;

	if (znamelist_stack_index >= 0) {
		if (zcolor_namelist_stack[znamelist_stack_index] != NULL) {
			ZCOLOR_NAMELIST_ENTRY *znep;
			ZCOLOR_NAMELIST_ENTRY *zneq;

			znep = zcolor_namelist_stack[znamelist_stack_index];
			while(znep != NULL) {
				zneq = znep->next;
				if (ldraw_commandline_opts.debug_level>0) {
					char buf[256];
					sprintf(buf,"Removing zcolor namelist entry %s",znep->name);
					zWrite(buf);
				}
				free(znep->name);
				free(znep);
				znep = zneq;
			}
		}
		if (zpoint_namelist_stack[znamelist_stack_index] != NULL) {
			ZPOINT_NAMELIST_ENTRY *znep;
			ZPOINT_NAMELIST_ENTRY *zneq;

			znep = zpoint_namelist_stack[znamelist_stack_index];
			while(znep != NULL) {
				zneq = znep->next;
				if (ldraw_commandline_opts.debug_level>0) {
					char buf[256];
					sprintf(buf,"Removing zpoint namelist entry %s",znep->name);
					zWrite(buf);
				}
				free(znep->name);
				free(znep);
				znep = zneq;
			}
		}
		if (zmatrix_namelist_stack[znamelist_stack_index] != NULL) {
			ZMATRIX_NAMELIST_ENTRY *znep;
			ZMATRIX_NAMELIST_ENTRY *zneq;

			znep = zmatrix_namelist_stack[znamelist_stack_index];
			while(znep != NULL) {
				zneq = znep->next;
				if (ldraw_commandline_opts.debug_level>0) {
					char buf[256];
					sprintf(buf,"Removing zmatrix namelist entry %s",znep->name);
					zWrite(buf);
				}
				free(znep->name);
				free(znep);
				znep = zneq;
			}
		}
		if (zcolor_table_stack[znamelist_stack_index] != NULL) {
			if (ldraw_commandline_opts.debug_level>0) {
				char buf[256];
				sprintf(buf,"Removing color table index %d",znamelist_stack_index);
				zWrite(buf);
			}
			free(zcolor_table_stack[znamelist_stack_index]);
			zcolor_table_stack[znamelist_stack_index] = NULL;
			// Set zcolor_table to point to previous valid table.
			znamelist_stack_index--;
			for(i=znamelist_stack_index; i>=0; i--) {
				if (zcolor_table_stack[i]!=NULL) {
					zcolor_table = zcolor_table_stack[i];
					if (zcolor_table != zcolor_prev_table) {
						zWrite("Fatal error in zcolor table stack");
						exit(-1);
					}
					break;
				}
			}
			// Set zcolor_prev_table to point to previous previous valid table.
			if (i == 0) {
				zcolor_prev_table = NULL;
			} else {
				for(j=(i-1); j >=0 ; j++) {
					if (zcolor_table_stack[j]!=NULL) {
						zcolor_prev_table = zcolor_table_stack[j];
						break;
					}
				}
			}
		} else {
			znamelist_stack_index--;
		}
	} else {
		zWrite("Stack error in zcolor routines");
		exit(-1);
	}
}
	
// Call this before a color change statement to make sure
// the current scope has a table malloc'd.
void zcolor_copy_table_if_needed()
{
	ZCOLOR_TABLE_ENTRY *new_table;
	int i;

	if(zcolor_table == zcolor_table_stack[znamelist_stack_index]) {
		return;
	}
	if (ldraw_commandline_opts.debug_level>0) {
		char buf[256];
		sprintf(buf,"Creating new color table index %d",znamelist_stack_index);
		zWrite(buf);
	}
	new_table = 
		(ZCOLOR_TABLE_ENTRY *)malloc(ZCOLOR_TABLE_SIZE*sizeof(ZCOLOR_TABLE_ENTRY));
	if (new_table == NULL) {
		zWrite("Fatal error: cannot malloc zcolor table\n");
		exit(-1);
	}
	for (i=0; i<ZCOLOR_TABLE_SIZE; i++) {
			new_table[i] = zcolor_table[i];
	}
	zcolor_table_stack[znamelist_stack_index] = new_table;
	zcolor_prev_table = zcolor_table;
	zcolor_table = zcolor_table_stack[znamelist_stack_index];
}

// Do every time the scoping of color tables could change.
// To avoid unneeded work, we only malloc and copy the table
// if the colors are changed.
// Call this when a new file starts.
void znamelist_push()
{
	if (znamelist_stack_index < (MAX_ZCOLOR_DEPTH-1)) {
		znamelist_stack_index++;
		zcolor_table_stack[znamelist_stack_index] = NULL; // probably redundant
		zcolor_namelist_stack[znamelist_stack_index] = NULL; // probably redundant
		zpoint_namelist_stack[znamelist_stack_index] = NULL; // probably redundant
		zmatrix_namelist_stack[znamelist_stack_index] = NULL; // probably redundant
	} else {
		zWrite("Stack error in zcolor routines");
		exit(-1);
	}
}

int zcolor_lookup(char *name)
{
	int i;
	ZCOLOR_NAMELIST_ENTRY *znep;

	// See if it is in the namelist
	for(i=znamelist_stack_index; i>=0; i--) {
		znep = zcolor_namelist_stack[i];
		while (znep != NULL) {
			if (!strcmp(znep->name, name)) {
				return znep->color_value;
			}
			znep= znep->next;
		}
	}
	{
		char buf[256];
		sprintf(buf,"Warning, color \"%s\" not defined, using default instead",
			name);
		zWrite(buf);
	}
	return ldraw_commandline_opts.C;  // default if name does not exist
}

vector3d* zpoint_lookup(char *name)
{
	int i;
	ZPOINT_NAMELIST_ENTRY *znep;

	// See if it is in the namelist
	for(i=znamelist_stack_index; i>=0; i--) {
		znep = zpoint_namelist_stack[i];
		while (znep != NULL) {
			if (!strcmp(znep->name, name)) {
				// make a copy
				return copyvec(&(znep->point));
			}
			znep= znep->next;
		}
	}
	{
		char buf[256];
		sprintf(buf,"Warning, point \"%s\" not defined, using default instead",
			name);
		zWrite(buf);
	}
	return savevec(0.0,0.0,0.0);  // default if name does not exist
}

matrix3d* zmatrix_lookup(char *name)
{
	int i;
	ZMATRIX_NAMELIST_ENTRY *znep;

	// See if it is in the namelist
	for(i=znamelist_stack_index; i>=0; i--) {
		znep = zmatrix_namelist_stack[i];
		while (znep != NULL) {
			if (!strcmp(znep->name, name)) {
				// make a copy
				return copymat(&(znep->matrix));
			}
			znep= znep->next;
		}
	}
	{
		char buf[256];
		sprintf(buf,"Warning, matrix \"%s\" not defined, using default instead",
			name);
		zWrite(buf);
	}
	return savemat(1.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,1.0);  // default if name does not exist
}

int zcolor_alias(int index, char *name) 
{
	ZCOLOR_NAMELIST_ENTRY *znep;

	if (name == NULL) {
		return 0;
	}
	// Store name in namelist
	znep = (ZCOLOR_NAMELIST_ENTRY *) malloc(sizeof(ZCOLOR_NAMELIST_ENTRY));
	znep->name = malloc(strlen(name)+1);
	strcpy(znep->name, name);
	znep->color_value = index;
	znep->next = zcolor_namelist_stack[znamelist_stack_index];
	zcolor_namelist_stack[znamelist_stack_index] = znep;
	return 0;
}

int zpoint_alias(char *name, vector3d *point) 
{
	ZPOINT_NAMELIST_ENTRY *zpep;

	// Store name in namelist
	zpep = (ZPOINT_NAMELIST_ENTRY *) malloc(sizeof(ZPOINT_NAMELIST_ENTRY));
	zpep->name = malloc(strlen(name)+1);
	strcpy(zpep->name, name);
	zpep->point = *point;
	zpep->next = zpoint_namelist_stack[znamelist_stack_index];
	zpoint_namelist_stack[znamelist_stack_index] = zpep;
	return 0;
}

int zmatrix_alias(char *name, matrix3d *matrix) 
{
	ZMATRIX_NAMELIST_ENTRY *zmep;

	if (matrix == NULL) {
		return 0;
	}
	// Store name in namelist
	zmep = (ZMATRIX_NAMELIST_ENTRY *) malloc(sizeof(ZMATRIX_NAMELIST_ENTRY));
	zmep->name = malloc(strlen(name)+1);
	strcpy(zmep->name, name);
	zmep->matrix = *matrix;
	zmep->next = zmatrix_namelist_stack[znamelist_stack_index];
	zmatrix_namelist_stack[znamelist_stack_index] = zmep;
	return 0;
}

int zcolor_modify(int index, char *name, int inverse_index,
				  int p_r, int p_g, int p_b, int p_a,
				  int d_r, int d_g, int d_b, int d_a)
{
	if ((index >= 0)&&(index < ZCOLOR_TABLE_SIZE)) {
		zcolor_copy_table_if_needed();
		zcolor_table[index].inverse_index = inverse_index;
		zcolor_table[index].primary.r = p_r;
		zcolor_table[index].primary.g = p_g;
		zcolor_table[index].primary.b = p_b;
		zcolor_table[index].primary.a = p_a;
		zcolor_table[index].dither.r = d_r;
		zcolor_table[index].dither.g = d_g;
		zcolor_table[index].dither.b = d_b;
		zcolor_table[index].dither.a = d_a;
		zcolor_alias(index,name);
		if (ldraw_commandline_opts.debug_level>0){
			char buf[256];
			sprintf(buf,"Added color index %d name %s",
				index, name);
			zWrite(buf);
		}
	}
	return 0;
}

//
// Returns ldraw edge (i.e. contrasting) color.
// Note that complementing is not commutative.
// c != complement_color(complement_color(c)), for example when c = 7
int edge_color(int c)
{
	if ((c>=0) && (c<ZCOLOR_TABLE_SIZE)) {
		// Pallette Entries
		return zcolor_table[c].inverse_index;
	} else if ((c >= 256)&&(c < 512)) {
		// dithered colors get complement of higher color
		return edge_color( ((c-256)/16) );
	} else if ((c >= 0x4000000)&&(c<=0x7ffffff)) {
		// Numbers of 0x4000000 to 0x7ffffff are hard coded color values. 
		return (0x4000000); // black
	} else {
		// anything else
		return 0;
	}
}

//
// zcp is the primary color
// zcs is the secondary color used for dithering
void translate_color(int c, ZCOLOR *zcp, ZCOLOR *zcs)
{
	if ((c>=0) && (c<ZCOLOR_TABLE_SIZE)) {
		// Pallette Entries
		*zcp = zcolor_table[c].primary;
		*zcs = zcolor_table[c].dither;
	} 
#ifdef USE_OPENGL
	//else if ((c >= 256)&&(c<512)) {
		// Dithered colors are now included in bigger ZCOLOR_TABLE_SIZE
	//}
	else if ((c >= 0x2000000)&&(c<=0x3ffffff)) {
                // L3P extended RGB colors (NOTE: add to ldlite CVS sources.)
	        zcp->r = (c & 0x00ff0000) >> 16;
		zcp->g = (c & 0x0000ff00) >> 8;
		zcp->b = (c & 0x000000ff) >> 0;
                zcs->r = zcp->r;
                zcs->g = zcp->g;
                zcs->b = zcp->b;
		if (c & 0x1000000) {
		  zcs->a = 0x0;
		} else {
		  zcs->a = 0xff;
		}
	}
	else if ((c >= 0x4000000)&&(c<=0x7ffffff)) {
		// Numbers of 0x4000000 to 0x7ffffff are hard coded color values. 
	        zcp->r = 17*((c & 0x00000f00) >> 8);
		zcp->g = 17*((c & 0x000000f0) >> 4);
		zcp->b = 17*((c & 0x0000000f) >> 0);
		if (c & 0x1000000) {
		  zcp->a = 0x0;
		} else {
		  zcp->a = 0xff;
		}
		zcs->r = 17*((c & 0x00f00000) >> 20);
		zcs->g = 17*((c & 0x000f0000) >> 16);
		zcs->b = 17*((c & 0x0000f000) >> 12);
		// No dithering, just average the numbers  
		zcp->r = (unsigned char) (((int)zcp->r + (int)zcs->r) / 2);
		zcp->g = (unsigned char) (((int)zcp->g + (int)zcs->g) / 2);
		zcp->b = (unsigned char) (((int)zcp->b + (int)zcs->b) / 2);
                zcs->r = zcp->r;
                zcs->g = zcp->g;
                zcs->b = zcp->b;
		if (c & 0x2000000) {
		  zcs->a = 0x0;
		} else {
		  zcs->a = 0xff;
		}
	} 
#else
	else if ((c >= 256)&&(c<512)) {
		// Dithered colors
		*zcp = zcolor_table[(c & 0xf0)>>4].primary;
		*zcs = zcolor_table[c & 0xf].primary;
	}
	else if ((c >= 0x4000000)&&(c<=0x7ffffff)) {
		// Numbers of 0x4000000 to 0x7ffffff are hard coded color values. 
	        zcp->r = 16*((c & 0x00000f00) >> 8);
		zcp->g = 16*((c & 0x000000f0) >> 4);
		zcp->b = 16*((c & 0x0000000f) >> 0);
		if (c & 0x1000000) {
		  zcp->a = 0x0;
		} else {
		  zcp->a = 0xff;
		}
		zcs->r = 16*((c & 0x00f00000) >> 20);
		zcs->g = 16*((c & 0x000f0000) >> 16);
		zcs->b = 16*((c & 0x0000f000) >> 12);
		if (c & 0x2000000) {
		  zcs->a = 0x0;
		} else {
		  zcs->a = 0xff;
		}
	} 
#endif
	else {
	        // anything else - solid grey
	        zcp->r = 0x7f;
		zcp->g = 0x7f;
		zcp->b = 0x7f;
		zcp->a = 0xff;
		zcs->r = 0x7f;
		zcs->g = 0x7f;
		zcs->b = 0x7f;
		zcs->a = 0xff;
	}
}

