/**
Provides information about
		the files,
		the memory and
		the behavior and
	emulates
		the random number generator and
		the key number interpreter
			of the executable.

TODO sort

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef EXEC_C
#define EXEC_C

#include <stdlib.h>//*random, size_t
#include <stdio.h>//*print*
#include <string.h>//str*, mem*
#include <limits.h>//CHAR_BIT

#include <curses.h>//KEY_*

#include "util.h"//intern, SWAP

#include "exec.h"

/**
The amount of color pairs initialized (including erroneously initialized pairs).

The value is set by <code>init_pair</code>.
**/
intern short int pairs = 0;

/**
The actual turn count without negative turns.

The value is set by <code>wgetch</code>.
**/
intern long int turns = 0;

/**
The size of the executable.
**/
intern const size_t exec_size = 2452608;
/**
The hash code of the executable.
**/
intern const int exec_hash = 893530599;
/**
The version of the executable.
**/
intern const unsigned char exec_version[4] = {1, 1, 1, 0};

/**
The minimum height of the terminal.
**/
intern const int exec_rows_min = 25;
/**
The maximum height of the terminal.
**/
intern const int exec_cols_min = 77;
/**
The minimum width of the terminal.
**/
intern const int exec_rows_max = 127;
/**
The maximum width of the terminal.
**/
intern const int exec_cols_max = 127;

/**
The name of the data directory.
**/
intern const char * const exec_data_directory = ".adom.data";
/**
The name of the temporary file directory.
**/
intern const char * const exec_temporary_directory = "tmpdat";
/**
The name prefix of temporary files.
**/
intern const char * const exec_temporary_file = "adom_tdl";
/**
The amount of temporary file groups.
**/
intern const unsigned int exec_temporary_levels = 51;
/**
The amount of temporary files in groups.
**/
intern const unsigned int exec_temporary_parts = 4;
/**
The name of the configuration file.
**/
intern const char * const exec_config_file = ".adom.cfg";
/**
The name of the process lock file.
**/
intern const char * const exec_process_file = ".adom.prc";
/**
The name of the keybinding file.
**/
intern const char * const exec_keybind_file = ".adom.kbd";
/**
The name of the version file.
**/
intern const char * const exec_version_file = ".adom.ver";
/**
The name of the error file.
**/
intern const char * const exec_error_file = ".adom.err";
/**
The name of the count file.
**/
intern const char * const exec_count_file = ".adom.cnt";

/**
The effects of the questions.
**/
intern const int exec_question_effects[51][4][9] = {
	{
		{+1, -1,  0,  0, +1,  0,  0,  0, -1},
		{+1,  0, -1,  0,  0, -1,  0,  0, +1},
		{ 0, +1,  0,  0,  0, -1,  0,  0,  0},
		{-1, +1, +1,  0,  0, -1,  0,  0,  0}
	},
	{
		{+1, -1, +1,  0, +1, -1, -1,  0,  0},
		{ 0, +1,  0,  0,  0, -1,  0,  0,  0},
		{-1, +1, +1,  0, -1,  0,  0,  0,  0},
		{+1,  0,  0,  0, +1,  0,  0, -2,  0}
	},
	{
		{+1, -1,  0, +1, +1, -1,  0, -1,  0},
		{ 0, +1, -1, +1,  0, -1,  0,  0,  0},
		{-1,  0,  0, -1,  0,  0,  0, +2,  0},
		{-1,  0,  0,  0, -1, -1,  0, +1, +2}
	},
	{
		{+1,  0,  0,  0,  0,  0,  0,  0, -1},
		{ 0,  0, +1,  0,  0,  0,  0, -1,  0},
		{-1, +1,  0,  0,  0,  0,  0,  0,  0},
		{+1, -1,  0,  0, +1, -1,  0,  0,  0}
	},
	{
		{ 0, +2,  0,  0, -1, -1,  0,  0,  0},
		{ 0, +1,  0,  0, -1, -1,  0, +1,  0},
		{ 0, +1, -1, +1,  0, -1, -1,  0, +1},
		{ 0,  0,  0,  0,  0,  0,  0,  0,  0}
	},
	{
		{ 0, +1, +1,  0, -1, -1,  0,  0,  0},
		{ 0, -1,  0,  0, +1,  0,  0,  0,  0},
		{ 0, +1,  0, -1,  0, +1,  0,  0, -1},
		{ 0, -1,  0, +1,  0,  0,  0,  0,  0}
	},
	{
		{ 0, +2,  0, -1, -1,  0,  0,  0,  0},
		{ 0,  0,  0,  0,  0,  0,  0, +1, -1},
		{ 0, -1,  0, +1,  0,  0,  0,  0,  0},
		{ 0,  0, +1, +1, -1,  0,  0, -1,  0}
	},
	{
		{ 0, -1, +2,  0,  0, -1,  0,  0,  0},
		{ 0,  0, -1,  0,  0,  0,  0,  0, +1},
		{ 0, -1, -1, +1,  0,  0,  0,  0, +1},
		{ 0, +1,  0,  0,  0, -1,  0,  0,  0}
	},
	{
		{ 0, -1,  0,  0, +1,  0,  0,  0,  0},
		{ 0,  0, +2, -1, -1,  0,  0,  0,  0},
		{ 0, -1, -1,  0,  0,  0,  0, +2,  0},
		{ 0,  0,  0, +1, -1,  0,  0,  0,  0}
	},
	{
		{ 0, +1,  0, -1,  0,  0,  0,  0,  0},
		{ 0, -2, +2,  0, +1,  0, -1,  0,  0},
		{ 0, +1,  0, +1,  0, -2,  0,  0,  0},
		{ 0, +1, -2,  0,  0,  0,  0,  0, +1}
	},
	{
		{ 0, -1, +2,  0,  0, -1,  0,  0,  0},
		{ 0,  0, -1,  0,  0, -1,  0, +1, +1},
		{ 0,  0, -1, +1,  0, -1,  0, +1,  0},
		{ 0, +1,  0,  0,  0,  0,  0, -1,  0}
	},
	{
		{ 0, -1,  0, +1,  0,  0,  0, -1, +1},
		{ 0, -1,  0, +2,  0, -1,  0,  0,  0},
		{+1,  0,  0, -1,  0, -1,  0,  0, +1},
		{ 0,  0, +1,  0,  0,  0,  0, -1,  0}
	},
	{
		{ 0,  0,  0,  0,  0,  0, +1, -1,  0},
		{ 0, +1,  0,  0, -1,  0,  0,  0,  0},
		{ 0,  0,  0,  0,  0, +1,  0, -1,  0},
		{ 0, -1,  0,  0,  0, -1,  0, +1, +1}
	},
	{
		{ 0, +1, -1,  0, +1, -1,  0,  0,  0},
		{ 0,  0, -2,  0,  0,  0,  0, +2,  0},
		{ 0, -1, +1,  0, -1, +1,  0,  0,  0},
		{ 0, -2,  0, +1,  0,  0,  0, +1,  0}
	},
	{
		{ 0, -1, +1,  0,  0,  0,  0,  0,  0},
		{ 0, +1, -1,  0,  0,  0,  0, -1, +1},
		{ 0,  0, -1, +1,  0, -1,  0, +1,  0},
		{ 0, +1, +1,  0,  0,  0,  0, -2,  0}
	},
	{
		{ 0, +1, -1,  0,  0, -1,  0, +1,  0},
		{ 0, -1, +1,  0,  0,  0,  0,  0,  0},
		{+1, -1, +1,  0, +1,  0,  0, -1, -1},
		{ 0, -1, +1,  0,  0,  0,  0, +1, -1}
	},
	{
		{ 0, -1,  0,  0, +1,  0,  0,  0,  0},
		{ 0,  0,  0, +1, -1,  0,  0,  0,  0},
		{ 0,  0,  0,  0,  0, +1,  0, -1,  0},
		{ 0, +1,  0, -1,  0,  0,  0,  0,  0}
	},
	{
		{ 0,  0,  0, +1, +1, -1,  0, -1,  0},
		{+1, -1,  0,  0, +1,  0,  0, -1,  0},
		{-1,  0,  0, +1, -1,  0,  0, +1,  0},
		{+1,  0,  0, -1,  0,  0,  0,  0,  0}
	},
	{
		{ 0,  0,  0, +1,  0, -1,  0,  0,  0},
		{ 0, -1,  0,  0,  0,  0,  0, +1,  0},
		{ 0, +1,  0,  0,  0,  0,  0, -1,  0},
		{ 0, -1, -1,  0,  0,  0,  0, +2,  0}
	},
	{
		{-1,  0,  0, +1, -1,  0,  0,  0, +1},
		{-1, +1, -2, +1, +1, -1,  0,  0, +1},
		{ 0, +1,  0, +1,  0, -2,  0, -1, +1},
		{-1,  0, +1,  0,  0,  0,  0,  0,  0}
	},
	{
		{ 0,  0, +1,  0,  0, +1,  0, -2,  0},
		{ 0,  0,  0,  0,  0, -1,  0, +1,  0},
		{ 0,  0, -1, +1,  0,  0,  0,  0,  0},
		{-1, +1, -1, +1,  0, -1,  0,  0, +1}
	},
	{
		{ 0,  0,  0, -1,  0, +2,  0, -1,  0},
		{ 0, +1, -1,  0,  0, -1,  0, +1,  0},
		{ 0,  0, -1,  0, +1,  0,  0,  0,  0},
		{ 0,  0, +1,  0,  0,  0,  0,  0, -1}
	},
	{
		{-1, +1,  0,  0,  0, +1,  0, -1,  0},
		{-1,  0,  0,  0, -1,  0,  0, +2,  0},
		{-1, -1,  0, +1,  0,  0,  0, +1,  0},
		{ 0,  0,  0,  0,  0,  0,  0, -1, +1}
	},
	{
		{-1,  0,  0,  0, -1, +1,  0, +1,  0},
		{+1, -1,  0,  0, +1, -1,  0,  0,  0},
		{ 0,  0,  0,  0, +1, +1,  0, -1, -1},
		{ 0,  0, +1,  0, -1,  0,  0,  0,  0}
	},
	{
		{ 0, +1,  0,  0, -2, +1,  0,  0,  0},
		{ 0, -1, +1, +1, -1,  0,  0,  0,  0},
		{-1,  0, -1, +1,  0,  0,  0,  0, +1},
		{ 0,  0, -1,  0, +1,  0,  0, -1, +1}
	},
	{
		{+1,  0,  0,  0, +1, -1,  0, -1,  0},
		{ 0,  0,  0, +1,  0, -1,  0,  0,  0},
		{ 0,  0, +1,  0,  0, -1,  0,  0,  0},
		{ 0,  0, +1,  0,  0,  0, -2, +1,  0}
	},
	{
		{+1,  0, +1, -1,  0, -1,  0,  0,  0},
		{ 0, +1, -1,  0,  0, +1,  0,  0, -1},
		{ 0, -1, +1,  0,  0,  0,  0,  0,  0},
		{-1,  0,  0, +1,  0, +1,  0,  0, -1}
	},
	{
		{ 0, -1,  0,  0,  0, +1,  0, +1, -1},
		{ 0,  0,  0,  0,  0, +1,  0,  0, -1},
		{+1,  0,  0, +1,  0, -1,  0, -1,  0},
		{+1,  0,  0,  0, +1,  0,  0, -1, -1}
	},
	{
		{ 0,  0, -1, +1, -1,  0,  0,  0, +1},
		{+1, -2,  0,  0, +1,  0,  0,  0,  0},
		{ 0, -1,  0,  0,  0, +1,  0, +1, -1},
		{ 0, -1,  0,  0,  0,  0,  0, +1,  0}
	},
	{
		{ 0,  0, -1,  0,  0, +1,  0,  0,  0},
		{ 0,  0, +2,  0,  0, -1,  0, -1,  0},
		{+1, -1,  0,  0, +1, -1,  0,  0,  0},
		{ 0, -1,  0, +1,  0,  0,  0,  0,  0}
	},
	{
		{ 0,  0, -1,  0,  0, +1,  0, -1, +1},
		{ 0,  0, +2,  0,  0, -1,  0,  0, -1},
		{ 0, +1,  0,  0,  0, -1,  0, +1, -1},
		{ 0,  0, +1,  0,  0,  0,  0,  0, -1}
	},
	{
		{ 0, +1,  0,  0,  0, -1,  0,  0,  0},
		{ 0,  0, -1,  0,  0, -1,  0, +2,  0},
		{ 0,  0, +1,  0,  0,  0,  0, -1,  0},
		{ 0,  0, +1,  0,  0, -1,  0,  0,  0}
	},
	{
		{ 0,  0, -1,  0,  0,  0,  0, +1,  0},
		{ 0, +1,  0,  0,  0,  0,  0,  0, -1},
		{ 0,  0,  0,  0, -1, +1,  0,  0,  0},
		{ 0,  0,  0, +1,  0, -1,  0,  0,  0}
	},
	{
		{ 0, +1,  0,  0,  0,  0,  0,  0, -1},
		{+1, -1,  0, +1,  0,  0,  0, -1,  0},
		{ 0, -1,  0, -1,  0,  0,  0, +2,  0},
		{ 0,  0,  0,  0,  0, -1,  0, +1,  0}
	},
	{
		{+1,  0,  0, -1, +1, -1,  0,  0,  0},
		{ 0, +1,  0, +1,  0,  0,  0, -1, -1},
		{ 0, +1,  0,  0,  0,  0,  0, -1,  0},
		{ 0,  0, +1,  0,  0, -1,  0,  0,  0}
	},
	{
		{+1, -1, -1,  0, +1,  0,  0,  0,  0},
		{+1, +1,  0,  0, -1,  0,  0, -1,  0},
		{+1,  0,  0,  0, +1, -2,  0,  0,  0},
		{ 0,  0, +2,  0,  0, -2,  0,  0,  0}
	},
	{
		{+1, -1,  0,  0,  0,  0,  0,  0,  0},
		{ 0,  0,  0,  0,  0, -1,  0, +1,  0},
		{-1,  0,  0, +1,  0,  0,  0,  0,  0},
		{ 0,  0,  0,  0,  0, +1,  0, -1,  0}
	},
	{
		{ 0,  0, +1,  0,  0, -1,  0,  0,  0},
		{ 0,  0,  0,  0,  0, -2,  0, +1, +1},
		{ 0,  0,  0, +1,  0, -1,  0,  0,  0},
		{ 0,  0,  0, -1, +1,  0,  0,  0,  0}
	},
	{
		{+1, -1, +1, -1,  0,  0,  0,  0,  0},
		{-1, +1,  0,  0,  0,  0,  0,  0,  0},
		{ 0, -1,  0,  0,  0, +1,  0,  0,  0},
		{-1,  0,  0,  0, -1, +1,  0, +1,  0}
	},
	{
		{ 0,  0, -1, +1,  0,  0,  0,  0,  0},
		{ 0, +1,  0,  0,  0,  0,  0, -1,  0},
		{+1, -1,  0,  0,  0,  0,  0,  0,  0},
		{ 0, -1,  0, +1,  0,  0,  0,  0,  0}
	},
	{
		{ 0,  0,  0,  0,  0,  0,  0, -1, +1},
		{ 0, +1, -1,  0,  0,  0,  0,  0,  0},
		{ 0,  0,  0,  0,  0, -1,  0, +1,  0},
		{ 0,  0,  0,  0, +1,  0,  0,  0, -1}
	},
	{
		{+1,  0,  0, -1,  0,  0,  0,  0,  0},
		{-1, +1,  0,  0,  0,  0,  0,  0,  0},
		{ 0,  0,  0, -1, +1,  0,  0,  0,  0},
		{ 0,  0,  0, +1, -1,  0,  0,  0,  0}
	},
	{
		{ 0,  0,  0, -1, +1,  0,  0,  0,  0},
		{ 0,  0,  0,  0,  0,  0,  0, -1, +1},
		{ 0, +1,  0,  0,  0,  0,  0, -1,  0},
		{+1, -1,  0,  0,  0,  0,  0,  0,  0}
	},
	{
		{ 0, -1,  0,  0,  0,  0,  0, +1,  0},
		{ 0, -1, +1,  0,  0,  0,  0,  0,  0},
		{+1, -1,  0,  0,  0,  0,  0,  0,  0},
		{ 0,  0, +1, -1,  0,  0,  0, -1, +1}
	},
	{
		{+1, -1,  0,  0,  0,  0,  0,  0,  0},
		{ 0,  0, -1, +1,  0,  0,  0,  0,  0},
		{+1,  0,  0,  0, +1, -1,  0, -1,  0},
		{-1, +1,  0,  0,  0,  0,  0,  0,  0}
	},
	{
		{ 0,  0,  0,  0,  0, +1,  0, -1,  0},
		{ 0,  0,  0, +1,  0,  0,  0, -1,  0},
		{ 0,  0, +1,  0,  0,  0,  0, -1,  0},
		{ 0,  0,  0,  0,  0, -1,  0, +1,  0}
	},
	{
		{ 0, -1, +1,  0,  0,  0,  0,  0,  0},
		{ 0,  0, -1,  0,  0,  0,  0, +1,  0},
		{+1, -1,  0,  0,  0,  0,  0,  0,  0},
		{ 0,  0,  0,  0,  0,  0,  0, -1, +1}
	},
	{
		{ 0, -1, +1,  0,  0,  0,  0,  0,  0},
		{ 0,  0, +1,  0, +1, -1,  0, -1,  0},
		{ 0,  0, +1,  0,  0, -1,  0,  0,  0},
		{ 0,  0, -1, +1,  0, -1,  0, +1,  0}
	},
	{
		{ 0, -1,  0,  0,  0, +1,  0, +1, -1},
		{ 0, +1,  0, -1,  0,  0,  0,  0,  0},
		{ 0, -1,  0, +1,  0, -1,  0, +1,  0},
		{ 0,  0, -1,  0,  0,  0,  0, +1,  0}
	},
	{
		{ 0, -1, +1,  0,  0,  0,  0,  0,  0},
		{ 0,  0,  0,  0,  0,  0,  0, +1, -1},
		{ 0, +1, -1,  0,  0, -1,  0, +1,  0},
		{ 0, +1,  0,  0,  0,  0,  0, -1,  0}
	},
	{
		{ 0,  0, +1,  0,  0,  0,  0,  0, -1},
		{ 0,  0, +1,  0,  0,  0,  0, -1,  0},
		{+1,  0,  0,  0, +1,  0,  0, -2,  0},
		{ 0, +1,  0,  0,  0,  0,  0, -1,  0}
	}
};

/**
A pointer to the list of the questions.

The list is written as the questions are answered.
**/
intern const int * const exec_questions = (void * )0x082b65f4;

/**
A pointer to the terrain characters.
**/
intern const char * const exec_terrain_chars = "?##.+##/#+.+./=\"~^&\".^^!~\".o*.=+*=*****ooo*=**o***o&***o****#*";//TODO find the address
/**
A pointer to the object characters.
**/
intern const char * const exec_object_chars = " ><><_**^0^^|0&+8\"\"\"\"\"\"$$____...._.&!T";//TODO find the address
/**
A pointer to the item characters.
**/
intern const char * const exec_item_chars = (void * )0x08286c0c;//"[[[[[[['~((}/]{=\\!?\"%$*"
/**
A pointer to the material colors.
**/
intern const char const exec_material_colors[14] = {7, 7, 6, 6, 10, 15, 15, 6, 6, 14/*8?*/, 8, 7, 7, 8};//TODO find the address
/**
A pointer to the item data.
**/
intern const exec_item_data_d * const exec_item_data = (void * )0x08273b00;
/**
A pointer to the monster data.
**/
intern const exec_monster_data_d * const exec_monster_data = (void * )0x08264fc0;

/**
A pointer to the terrain map.
**/
intern unsigned char ** const exec_terrain = (void * )0x0829ea60;
/**
A pointer to the object map.
**/
intern unsigned char ** const exec_objects = (void * )0x0829ea78;
/**
A pointer to the item map.
**/
intern exec_map_item_d *** const exec_items = (void * )0x082a4e7c;
/**
A pointer to the monster map.
**/
intern exec_map_monster_d ** const exec_monsters = (void * )0x082b6a08;

/*
Undocumented.
*/
intern unsigned int * const exec_blindness = (void * )0x082b17dc;
intern unsigned int * const exec_stunnedness = (void * )0x082b17e0;
intern unsigned int * const exec_confusion = (void * )0x082b17e4;
intern unsigned int * const exec_slowness = (void * )0x082b17e8;
intern unsigned int * const exec_paralyzation = (void * )0x082b17ec;
intern unsigned int * const exec_sickness = (void * )0x082b17f0;
intern unsigned int * const exec_deafness = (void * )0x082b17f4;
intern unsigned int * const exec_muteness = (void * )0x082b17f8;
intern unsigned int * const exec_sleepiness = (void * )0x082b17fc;
intern unsigned int * const exec_blessedness = (void * )0x082b1800;
intern unsigned int * const exec_invisibility = (void * )0x082b1804;
intern unsigned int * const exec_drunkenness = (void * )0x082b5f50;
intern unsigned int * const exec_tactics = (void * )0x082b5f54;
intern unsigned int * const exec_satiety = (void * )0x082b5f60;
intern unsigned int * const exec_intrinsics = (void * )0x082b5f64;
intern int * const exec_alignment = (void * )0x082b5fac;
intern int * const exec_chaotic_piety = (void * )0x082b5fb0;
intern int * const exec_neutral_piety = (void * )0x082b5fb4;
intern int * const exec_lawful_piety = (void * )0x082b5fb8;
intern unsigned int * const exec_corruption = (void * )0x082b615c;
intern unsigned int * const exec_corruptions = (void * )0x082b6160;
intern unsigned int * const exec_strength_of_atlas = (void * )0x082b61e8;
intern unsigned int * const exec_farsight = (void * )0x082b61ec;
intern unsigned int * const exec_time = (void * )0x082b61f4;//?
intern unsigned int * const exec_speed = (void * )0x082b6230;
intern int * const exec_speed_modifier = (void * )0x082b6238;

intern const char * const exec_blindness_string = "Blind";
intern const char * const exec_stunnedness_string = "Stunned";
intern const char * const exec_confusion_string = "Confused";
intern const char * const exec_slowedness_string = "Slowed";
intern const char * const exec_paralyzation_string = "Paralyzed";
intern const char * const exec_sickness_string = "Sick";
intern const char * const exec_deafness_string = "Deaf";
intern const char * const exec_muteness_string = "Mute";
intern const char * const exec_sleepiness_string = "Sleeping";
intern const char * const exec_blessedness_string = "Blessed";
intern const char * const exec_invisibility_string = "Invisible";
intern const char * const exec_drunkenness_string = "Drunk";
intern const char * const exec_severe_berserking_string = "Extremely Berserk";//naked
intern const char * const exec_berserking_string = "Berserk";//= 0
intern const char * const exec_severe_aggressiveness_string = "Very Aggressive";
intern const char * const exec_aggressiveness_string = "Aggressive";
intern const char * const exec_defensiveness_string = "Defensive";
intern const char * const exec_severe_defensiveness_string = "Very Defensive";
intern const char * const exec_cowardice_string = "Cowardly";//= 6
intern const char * const exec_severe_cowardice_string = "Extremely Cowardly";//low hp
intern const char * const exec_fatal_starvation_string = "Starved";//< 0
intern const char * const exec_starvation_string = "Starving";//< 50
intern const char * const exec_severe_hunger_string = "Very Hungry";//< 100
intern const char * const exec_hunger_string = "Hungry";//< 200
intern const char * const exec_satiety_string = "Satiated";//> 1500 && <= 2500
intern const char * const exec_bloatedness_string = "Bloated";//> 2500
intern const char * const exec_fatal_bloatedness_string = "Overfed";
intern const char * const exec_intrinsic_fire_resistance_string = "Fire Resistant";
intern const char * const exec_intrinsic_poison_resistance_string = "Poison Resistant";
intern const char * const exec_intrinsic_cold_resistance_string = "Cold Resistant";
intern const char * const exec_intrinsic_acid_resistance_string = "Acid Resistant";
intern const char * const exec_intrinsic_luck_string = "Lucky";
intern const char * const exec_intrinsic_fate_smile_string = "Fate Smilant";
intern const char * const exec_intrinsic_curse_string = "Cursed";
intern const char * const exec_intrinsic_sleep_resistance_string = "Sleep Resistant";
intern const char * const exec_intrinsic_petrification_resistance_string = "Petrification Resistant";
intern const char * const exec_intrinsic_doom_string = "Doomed";
intern const char * const exec_intrinsic_teleportitis_string = "Teleporting";
intern const char * const exec_intrinsic_invisibility_string = "Permanently Invisible";
intern const char * const exec_intrinsic_teleport_control_string = "Teleport Controlling";
intern const char * const exec_intrinsic_stun_resistance_string = "Stun Resistant";
intern const char * const exec_intrinsic_death_ray_resistance_string = "Death Ray Resistant";
intern const char * const exec_intrinsic_paralyzation_resistance_string = "Paralyzation Resistant";
intern const char * const exec_intrinsic_shock_resistance_string = "Shock Resistant";
intern const char * const exec_intrinsic_invisible_sight_string = "Invisible Seeing";
intern const char * const exec_intrinsic_fire_immunity_string = "Fire Immune";
intern const char * const exec_intrinsic_acid_immunity_string = "Acid Immune";
intern const char * const exec_intrinsic_cold_immunity_string = "Cold Immune";
intern const char * const exec_intrinsic_shock_immunity_string = "Shock Immune";
intern const char * const exec_intrinsic_water_breathing_string = "Water Breathing";
intern const char * const exec_intrinsic_holiness_string = "Holy";
intern const char * const exec_intrinsic_confusion_resistance_string = "Confusion Resistant";
intern const char * const exec_severe_chaoticness_string = "Extremely Chaotic";//<= -5000
intern const char * const exec_chaoticness_string = "Chaotic";//< -1250
intern const char * const exec_chaotic_neutrality_string = "Chaotic Neutral";//< -125
intern const char * const exec_neutrality_string = "Neutral";//>= 125 && <= -125
intern const char * const exec_lawful_neutrality_string = "Lawful Neutral";//> 125
intern const char * const exec_lawfulness_string = "Lawful";//> 1250
intern const char * const exec_severe_lawfulness_string = "Extremely Lawful";//>= 5000
intern const char * const exec_corruption_string = "Corrupted";//>= 1000 && < 17000
intern const char * const exec_severe_corruption_string = "Very Corrupted";//>= 17000
intern const char * const exec_strength_of_atlas_string = "With Strength of Atlas";
intern const char * const exec_farsight_string = "With Farsight";
intern const char * const exec_severe_fastness_string = "Very Fast";//>= 150 plus starsign
intern const char * const exec_fastness_string = "Fast";//>= 105
intern const char * const exec_slowness_string = "Slow";//<= 95
intern const char * const exec_severe_slowness_string = "Very Slow";//>= 65

intern const char * const exec_poisonedness_string = "Poisoned";//?
intern const char * const exec_severe_bleeding_string = "Severely Bleeding";
intern const char * const exec_bleeding_string = "Bleeding";//?
intern const char * const exec_fatal_overburdenedness_string = "Crushed";
intern const char * const exec_overburdenedness_string = "Overburdened";
intern const char * const exec_severe_strainedness_string = "Very Strained";
intern const char * const exec_strainedness_string = "Strained";
intern const char * const exec_burdenedness_string = "Burdened";//?

intern const char * const exec_blindness_abbreviation = "Bi";
intern const char * const exec_stunnedness_abbreviation = "Su";
intern const char * const exec_confusion_abbreviation = "Cn";
intern const char * const exec_slowedness_abbreviation = "Sw";
intern const char * const exec_paralyzation_abbreviation = "Pa";
intern const char * const exec_sickness_abbreviation = "Si";
intern const char * const exec_deafness_abbreviation = "Da";
intern const char * const exec_muteness_abbreviation = "Mu";
intern const char * const exec_sleepiness_abbreviation = "Se";
intern const char * const exec_blessedness_abbreviation = "Bs";
intern const char * const exec_invisibility_abbreviation = "In";
intern const char * const exec_drunkenness_abbreviation = "Dr";
intern const char * const exec_severe_berserking_abbreviation = "EB";//naked
intern const char * const exec_berserking_abbreviation = "Be";//= 0
intern const char * const exec_severe_aggressiveness_abbreviation = "VA";
intern const char * const exec_aggressiveness_abbreviation = "Ag";
intern const char * const exec_defensiveness_abbreviation = "Df";
intern const char * const exec_severe_defensiveness_abbreviation = "VD";
intern const char * const exec_cowardice_abbreviation = "Cw";//= 6
intern const char * const exec_severe_cowardice_abbreviation = "EC";//low hp
intern const char * const exec_fatal_starvation_abbreviation = "Sd";//< 0
intern const char * const exec_starvation_abbreviation = "Sn";//< 50
intern const char * const exec_severe_hunger_abbreviation = "VH";//< 100
intern const char * const exec_hunger_abbreviation = "Hu";//< 200
intern const char * const exec_satiety_abbreviation = "Sa";//> 1500 && <= 2500
intern const char * const exec_bloatedness_abbreviation = "Bo";//> 2500
intern const char * const exec_fatal_bloatedness_abbreviation = "Of";
intern const char * const exec_intrinsic_fire_resistance_abbreviation = "FR";
intern const char * const exec_intrinsic_poison_resistance_abbreviation = "PR";
intern const char * const exec_intrinsic_cold_resistance_abbreviation = "CR";
intern const char * const exec_intrinsic_acid_resistance_abbreviation = "AR";
intern const char * const exec_intrinsic_luck_abbreviation = "Lu";
intern const char * const exec_intrinsic_fate_smile_abbreviation = "FS";
intern const char * const exec_intrinsic_curse_abbreviation = "Cu";
intern const char * const exec_intrinsic_sleep_resistance_abbreviation = "lR";
intern const char * const exec_intrinsic_petrification_resistance_abbreviation = "eR";
intern const char * const exec_intrinsic_doom_abbreviation = "Do";
intern const char * const exec_intrinsic_teleportitis_abbreviation = "Te";
intern const char * const exec_intrinsic_invisibility_abbreviation = "PE";
intern const char * const exec_intrinsic_teleport_control_abbreviation = "TC";
intern const char * const exec_intrinsic_stun_resistance_abbreviation = "tR";
intern const char * const exec_intrinsic_death_ray_resistance_abbreviation = "DR";
intern const char * const exec_intrinsic_paralyzation_resistance_abbreviation = "aR";
intern const char * const exec_intrinsic_shock_resistance_abbreviation = "SR";
intern const char * const exec_intrinsic_invisible_sight_abbreviation = "IS";
intern const char * const exec_intrinsic_fire_immunity_abbreviation = "FI";
intern const char * const exec_intrinsic_acid_immunity_abbreviation = "AI";
intern const char * const exec_intrinsic_cold_immunity_abbreviation = "CI";
intern const char * const exec_intrinsic_shock_immunity_abbreviation = "SI";
intern const char * const exec_intrinsic_water_breathing_abbreviation = "WB";
intern const char * const exec_intrinsic_holiness_abbreviation = "Ho";
intern const char * const exec_intrinsic_confusion_resistance_abbreviation = "oR";
intern const char * const exec_severe_chaoticness_abbreviation = "EC";//<= -5000
intern const char * const exec_chaoticness_abbreviation = "Ch";//< -1250
intern const char * const exec_chaotic_neutrality_abbreviation = "CN";//< -125
intern const char * const exec_neutrality_abbreviation = "Ne";//>= 125 && <= -125
intern const char * const exec_lawful_neutrality_abbreviation = "LN";//> 125
intern const char * const exec_lawfulness_abbreviation = "La";//> 1250
intern const char * const exec_severe_lawfulness_abbreviation = "EL";//>= 5000
intern const char * const exec_no_corruption_abbreviation = "Un";//< 1000
intern const char * const exec_corruption_abbreviation = "Co";//< 17000
intern const char * const exec_severe_corruption_abbreviation = "VC";//>= 17000
intern const char * const exec_strength_of_atlas_abbreviation = "WA";
intern const char * const exec_farsight_abbreviation = "WF";
intern const char * const exec_severe_fastness_abbreviation = "VF";//>= 150 plus starsign
intern const char * const exec_fastness_abbreviation = "Fa";//>= 105
intern const char * const exec_slowness_abbreviation = "Sl";//<= 95
intern const char * const exec_severe_slowness_abbreviation = "Vl";//>= 65

intern const char * const exec_poisonedness_abbreviation = "Po";//?
intern const char * const exec_severe_bleeding_abbreviation = "SB";
intern const char * const exec_bleeding_abbreviation = "Bl";//?
intern const char * const exec_fatal_overburdenedness_abbreviation = "Cr";
intern const char * const exec_overburdenedness_abbreviation = "Ob";
intern const char * const exec_severe_strainedness_abbreviation = "VS";
intern const char * const exec_strainedness_abbreviation = "St";
intern const char * const exec_burdenedness_abbreviation = "Bu";//?

/**
A pointer to the turn count.
**/
intern long int * const exec_turns = (void * )0x082b16e0;
/**
A pointer to the save count.
**/
intern long int * const exec_saves = (void * )0x082b6140;

/**
The random number generator's counter c.
**/
intern unsigned char * const exec_arc4_c = (void * )0x08264a60;
/**
The random number generator's state S.
**/
intern unsigned char * const exec_arc4_s = (void * )0x082ada40;
/**
The random number generator's first iterator i.
**/
intern unsigned char * const exec_arc4_i = (void * )0x082adb40;
/**
The random number generator's second iterator j.
**/
intern unsigned char * const exec_arc4_j = (void * )0x082adb41;

/**
The amount of random number generator calls measured
	from seeding the random number generator
	to its first cyclic point.
**/
intern const unsigned int exec_arc4_calls = 1165;
/**
The amount of random number generator calls
	from the splash screen or the main menu
	to loading a game automatically.
**/
intern const unsigned int exec_arc4_calls_automatic_load = 205;
/**
The amount of random number generator calls measured
	from the splash screen or the main menu
	to loading a game manually (from a list of one game).
**/
intern const unsigned int exec_arc4_calls_manual_load = 409;

/**
The emulated random number generator's counter c.

Emulates an internal variable:
<pre>
unsigned int * const arc4_calls = (void * )0x08264a60;
</pre>
**/
unsigned int arc4_c = 0;
/**
The emulated random number generator's state S.

Emulates an internal variable:
<pre>
unsigned char * const arc4_state = (void * )0x082ada40;
</pre>
**/
unsigned char arc4_s[256];
/**
The emulated random number generator's first iterator i.

Emulates an internal variable:
<pre>
unsigned char * const first_arc4_iterator = (void * )0x082adb40;
</pre>
**/
unsigned char arc4_i = 0;
/**
The emulated random number generator's second iterator j.

Emulates an internal variable:
<pre>
unsigned char * const second_arc4_iterator = (void * )0x082adb41;
</pre>
**/
unsigned char arc4_j = 0;

/**
Seeds the current state S with the seed k.

Emulates an internal function:
<pre>
void (* const seed_arc4)(unsigned long int seed) = (void * )0x08125ea0;
</pre>

@param k The seed k.
**/
void sarc4(const unsigned long int k) {
	unsigned char i = 0;
	unsigned char j = 0;
	do {
		arc4_s[i] = i;
		i++;
	} while(i != 0);
	do {
		j = (unsigned char )(j + (arc4_s[i] + ((const unsigned char * )&k)[i % sizeof k]));
		SWAP(arc4_s[i], arc4_s[j]);
		i++;
	} while (i != 0);
}

/**
Generates a byte q (and changes the current state).

The order of operations is wrong to replicate the behavior of the executable.

@return The byte q.
**/
unsigned char arc4(void) {
	//point A
	arc4_j = (unsigned char )(arc4_j + arc4_s[arc4_i]);
	SWAP(arc4_s[arc4_i], arc4_s[arc4_j]);
	const unsigned char q = arc4_s[(unsigned char )(arc4_s[arc4_i] + arc4_s[arc4_j])];
	arc4_i++;//should be at point A
	return q;
}

/**
Generates an integer w and increments the count c.

@return The integer w.
**/
unsigned long int carc4(void) {
	arc4_c++;
	unsigned long int w = 0;
	for (size_t bit = 0; bit < sizeof w; bit++) {
		w |= (size_t )arc4() << bit * CHAR_BIT;
	}
	return w;
}

/**
Generates a bound integer b at least 0 and at most s - 1.

Emulates an internal function:
<pre>
unsigned long int (* const bound_arc4)(unsigned long int supremum) = (void * )0x08126130;
</pre>

@param s The supremum s.
@return The integer b.
**/
unsigned long int barc4(const unsigned long int s) {
	const unsigned long int b = carc4();
	if (s == 0) {
		return b;
	}
	return b % s;
}

/**
Generates and injects
	the counter c,
	the state S and
	the iterators i and j.

@param k The seed k to use.
@param calls The amount of calls to do.
**/
void iarc4(const unsigned long int k, const unsigned int calls) {
	arc4_i = 0;
	arc4_j = 0;
	srandom(k);
	sarc4((unsigned long int )random());
	for (unsigned int call = 0; call < exec_arc4_calls; call++) {
		carc4();
	}
	const unsigned long int first_sup = 20;
	const unsigned long int second_sup = 18;
	unsigned long int first = barc4(first_sup) + 10;
	for (unsigned long int iterator = 0; iterator < first; iterator++) {
		const unsigned long int second = barc4(second_sup);
		while (barc4(second_sup) == second);
	}
	for (unsigned int call = 0; call < calls; call++) {
		carc4();
	}
	memcpy(exec_arc4_c, &arc4_c, sizeof arc4_c);
	memcpy(exec_arc4_s, arc4_s, sizeof arc4_s);
	memcpy(exec_arc4_i, &arc4_i, sizeof arc4_i);
	memcpy(exec_arc4_j, &arc4_j, sizeof arc4_j);
}

/*
Returns the key code of a key number.

The code is generated automatically:
<pre>
gcc src/meta.c -O3 -o obj/meta
mkdir -p src/meta
obj/meta key_code > src/meta/key_code.c
rm -f obj/meta
</pre>

@param code The key number.
@return The key code.
*/
#include "meta/key_code.c"

#endif
