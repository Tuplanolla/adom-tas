/**
Provides information about
 the files,
 the memory and
 the behavior
  of the executable.

TODO sort

@file exec.c
@author Sampsa "Tuplanolla" Kiiskinen
**/
#include <stddef.h>//size_t
#include <stdio.h>//*print*
#include <string.h>//str*, mem*

#include <curses.h>//KEY_*

#include "util.h"//intern, SWAP

#include "exec.h"

/**
The name of the executable.
**/
const char * const exec_name = "adom";
/**
The size of the executable.
**/
const size_t exec_size = 2452608;
/**
The hash code of the executable.
**/
const int exec_hash = 893530599;
/**
The version of the executable.
**/
const unsigned char exec_version[4] = {1, 1, 1, 0};

/**
The minimum height of the terminal.
**/
const int exec_rows_min = 25;
/**
The maximum height of the terminal.
**/
const int exec_cols_min = 77;
/**
The minimum width of the terminal.
**/
const int exec_rows_max = 127;
/**
The maximum width of the terminal.
**/
const int exec_cols_max = 127;

/**
The name of the data directory.
**/
const char * const exec_data_directory = ".adom.data";
/**
The name of the temporary file directory.
**/
const char * const exec_temporary_directory = "tmpdat";
/**
The name prefix of temporary files.
**/
const char * const exec_temporary_file = "adom_tdl";
/**
The amount of temporary file groups.
**/
const int exec_temporary_levels = 51;
/**
The amount of temporary files in groups.
**/
const int exec_temporary_parts = 4;
/**
The name of the configuration file.
**/
const char * const exec_config_file = ".adom.cfg";
/**
The name of the process lock file.
**/
const char * const exec_process_file = ".adom.prc";
/**
The name of the keybinding file.
**/
const char * const exec_keybind_file = ".adom.kbd";
/**
The name of the version file.
**/
const char * const exec_version_file = ".adom.ver";
/**
The name of the error file.
**/
const char * const exec_error_file = ".adom.err";
/**
The name of the count file.
**/
const char * const exec_count_file = ".adom.cnt";

/**
The effects of the questions.
**/
const int exec_question_effects[51][4][9] = {
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
const int * const exec_questions = (void * )0x082b65f4;

/**
A pointer to the terrain characters.
**/
const char * const exec_terrain_chars = "?##.+##/#+.+./=\"~^&\".^^!~\".o*.=+*=*****ooo*=**o***o&***o****#*";//TODO find the address
/**
A pointer to the object characters.
**/
const char * const exec_object_chars = " ><><_**^0^^|0&+8\"\"\"\"\"\"$$____...._.&!T";//TODO find the address
/**
A pointer to the item characters.
**/
const char * const exec_item_chars = (void * )0x08286c0c;//"[[[[[[['~((}/]{=\\!?\"%$*"
/**
A pointer to the material colors.
**/
const char const exec_material_colors[14] = {7, 7, 6, 6, 10, 15, 15, 6, 6, 14/*8?*/, 8, 7, 7, 8};//TODO find the address
/**
A pointer to the item data.
**/
const exec_item_data_d * const exec_item_data = (void * )0x08273b00;
/**
A pointer to the monster data.
**/
const exec_monster_data_d * const exec_monster_data = (void * )0x08264fc0;

/**
A pointer to the terrain map.
**/
unsigned char ** const exec_terrain = (void * )0x0829ea60;
/**
A pointer to the object map.
**/
unsigned char ** const exec_objects = (void * )0x0829ea78;
/**
A pointer to the item map.
**/
exec_map_item_d *** const exec_items = (void * )0x082a4e7c;
/**
A pointer to the monster map.
**/
exec_map_monster_d ** const exec_monsters = (void * )0x082b6a08;

/*
Undocumented.
*/
unsigned int * const exec_blindness = (void * )0x082b17dc;
unsigned int * const exec_stunnedness = (void * )0x082b17e0;
unsigned int * const exec_confusion = (void * )0x082b17e4;
unsigned int * const exec_slowness = (void * )0x082b17e8;
unsigned int * const exec_paralyzation = (void * )0x082b17ec;
unsigned int * const exec_sickness = (void * )0x082b17f0;
unsigned int * const exec_deafness = (void * )0x082b17f4;
unsigned int * const exec_muteness = (void * )0x082b17f8;
unsigned int * const exec_sleepiness = (void * )0x082b17fc;
unsigned int * const exec_blessedness = (void * )0x082b1800;
unsigned int * const exec_invisibility = (void * )0x082b1804;
unsigned int * const exec_drunkenness = (void * )0x082b5f50;
unsigned int * const exec_tactics = (void * )0x082b5f54;
unsigned int * const exec_satiety = (void * )0x082b5f60;
unsigned int * const exec_intrinsics = (void * )0x082b5f64;
int * const exec_alignment = (void * )0x082b5fac;
int * const exec_chaotic_piety = (void * )0x082b5fb0;
int * const exec_neutral_piety = (void * )0x082b5fb4;
int * const exec_lawful_piety = (void * )0x082b5fb8;
unsigned int * const exec_corruption = (void * )0x082b615c;
unsigned int * const exec_corruptions = (void * )0x082b6160;
unsigned int * const exec_strength_of_atlas = (void * )0x082b61e8;
unsigned int * const exec_farsight = (void * )0x082b61ec;
unsigned int * const exec_time = (void * )0x082b61f4;//?
unsigned int * const exec_speed = (void * )0x082b6230;
int * const exec_speed_modifier = (void * )0x082b6238;

const char * const exec_blindness_string = "Blind";
const char * const exec_stunnedness_string = "Stunned";
const char * const exec_confusion_string = "Confused";
const char * const exec_slowedness_string = "Slowed";
const char * const exec_paralyzation_string = "Paralyzed";
const char * const exec_sickness_string = "Sick";
const char * const exec_deafness_string = "Deaf";
const char * const exec_muteness_string = "Mute";
const char * const exec_sleepiness_string = "Sleeping";
const char * const exec_blessedness_string = "Blessed";
const char * const exec_invisibility_string = "Invisible";
const char * const exec_drunkenness_string = "Drunk";
const char * const exec_severe_berserking_string = "Extremely Berserk";//naked
const char * const exec_berserking_string = "Berserk";//= 0
const char * const exec_severe_aggressiveness_string = "Very Aggressive";
const char * const exec_aggressiveness_string = "Aggressive";
const char * const exec_defensiveness_string = "Defensive";
const char * const exec_severe_defensiveness_string = "Very Defensive";
const char * const exec_cowardice_string = "Cowardly";//= 6
const char * const exec_severe_cowardice_string = "Extremely Cowardly";//low hp
const char * const exec_fatal_starvation_string = "Starved";//< 0
const char * const exec_starvation_string = "Starving";//< 50
const char * const exec_severe_hunger_string = "Very Hungry";//< 100
const char * const exec_hunger_string = "Hungry";//< 200
const char * const exec_satiety_string = "Satiated";//> 1500 && <= 2500
const char * const exec_bloatedness_string = "Bloated";//> 2500
const char * const exec_fatal_bloatedness_string = "Overfed";
const char * const exec_intrinsic_fire_resistance_string = "Fire Resistant";
const char * const exec_intrinsic_poison_resistance_string = "Poison Resistant";
const char * const exec_intrinsic_cold_resistance_string = "Cold Resistant";
const char * const exec_intrinsic_acid_resistance_string = "Acid Resistant";
const char * const exec_intrinsic_luck_string = "Lucky";
const char * const exec_intrinsic_fate_smile_string = "Fate Smilant";
const char * const exec_intrinsic_curse_string = "Cursed";
const char * const exec_intrinsic_sleep_resistance_string = "Sleep Resistant";
const char * const exec_intrinsic_petrification_resistance_string = "Petrification Resistant";
const char * const exec_intrinsic_doom_string = "Doomed";
const char * const exec_intrinsic_teleportitis_string = "Teleporting";
const char * const exec_intrinsic_invisibility_string = "Permanently Invisible";
const char * const exec_intrinsic_teleport_control_string = "Teleport Controlling";
const char * const exec_intrinsic_stun_resistance_string = "Stun Resistant";
const char * const exec_intrinsic_death_ray_resistance_string = "Death Ray Resistant";
const char * const exec_intrinsic_paralyzation_resistance_string = "Paralyzation Resistant";
const char * const exec_intrinsic_shock_resistance_string = "Shock Resistant";
const char * const exec_intrinsic_invisible_sight_string = "Invisible Seeing";
const char * const exec_intrinsic_fire_immunity_string = "Fire Immune";
const char * const exec_intrinsic_acid_immunity_string = "Acid Immune";
const char * const exec_intrinsic_cold_immunity_string = "Cold Immune";
const char * const exec_intrinsic_shock_immunity_string = "Shock Immune";
const char * const exec_intrinsic_water_breathing_string = "Water Breathing";
const char * const exec_intrinsic_holiness_string = "Holy";
const char * const exec_intrinsic_confusion_resistance_string = "Confusion Resistant";
const char * const exec_severe_chaoticness_string = "Extremely Chaotic";//<= -5000
const char * const exec_chaoticness_string = "Chaotic";//< -1250
const char * const exec_chaotic_neutrality_string = "Chaotic Neutral";//< -125
const char * const exec_neutrality_string = "Neutral";//>= 125 && <= -125
const char * const exec_lawful_neutrality_string = "Lawful Neutral";//> 125
const char * const exec_lawfulness_string = "Lawful";//> 1250
const char * const exec_severe_lawfulness_string = "Extremely Lawful";//>= 5000
const char * const exec_corruption_string = "Corrupted";//>= 1000 && < 17000
const char * const exec_severe_corruption_string = "Very Corrupted";//>= 17000
const char * const exec_strength_of_atlas_string = "With Strength of Atlas";
const char * const exec_farsight_string = "With Farsight";
const char * const exec_severe_fastness_string = "Very Fast";//>= 150 plus starsign
const char * const exec_fastness_string = "Fast";//>= 105
const char * const exec_slowness_string = "Slow";//<= 95
const char * const exec_severe_slowness_string = "Very Slow";//>= 65

const char * const exec_poisonedness_string = "Poisoned";//?
const char * const exec_severe_bleeding_string = "Severely Bleeding";
const char * const exec_bleeding_string = "Bleeding";//?
const char * const exec_fatal_overburdenedness_string = "Crushed";
const char * const exec_overburdenedness_string = "Overburdened";
const char * const exec_severe_strainedness_string = "Very Strained";
const char * const exec_strainedness_string = "Strained";
const char * const exec_burdenedness_string = "Burdened";//?

/**
The amount of color pairs initialized (including erroneously initialized pairs).

The value is set by <code>init_pair</code>.
**/
short int pairs = 0;
/**
The actual turn count without negative turns.

The value is set by <code>wgetch</code>.
**/
long int turns = 0;

/**
A pointer to the turn count.
**/
long int * const exec_turns = (void * )0x082b16e0;
/**
A pointer to the save count.
**/
long int * const exec_saves = (void * )0x082b6140;
