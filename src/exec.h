/**
@file exec.h
@author Sampsa "Tuplanolla" Kiiskinen

TODO sort
**/
#ifndef EXEC_H
#define EXEC_H

#include <stddef.h>//size_t

struct exec_item_data_s {
	unsigned int category;
	unsigned char stuff[4];
	//...
	int color;
	unsigned char other_stuff[88];
	//...
	unsigned int material;
	unsigned char more_stuff[8];
	//...
};
typedef struct exec_item_data_s exec_item_data_d;

struct exec_monster_data_s {
	char * name_stuff;
	char * more_name_stuff;
	char character;
	unsigned char other_stuff[3 + 52];
	//...
	unsigned int color;
	unsigned char more_stuff[68];
	//...
};
typedef struct exec_monster_data_s exec_monster_data_d;

struct exec_item_s {
	unsigned int type;
	//...
};
typedef struct exec_item_s exec_item_d;

struct exec_monster_s {
	char name[32];
	unsigned int type;
	unsigned char stuff[20];
	//...
	int x;
	int y;
	//...
};
typedef struct exec_monster_s exec_monster_d;

struct exec_map_item_s {
	struct exec_item_s * item;
	struct exec_map_item_s * next;
};
typedef struct exec_map_item_s exec_map_item_d;

struct exec_map_monster_s {
	struct exec_monster_s * monster;
	struct exec_map_monster_s * next;
};
typedef struct exec_map_monster_s exec_map_monster_d;

extern const char * const exec_name;
extern const size_t exec_size;
extern const int exec_hash;
extern const unsigned char exec_version[4];

extern const int exec_rows_min;
extern const int exec_rows_max;
extern const int exec_cols_min;
extern const int exec_cols_max;

extern const char * const exec_data_directory;
extern const char * const exec_temporary_directory;
extern const char * const exec_temporary_file;
extern const int exec_temporary_levels;
extern const int exec_temporary_parts;
extern const char * const exec_config_file;
extern const char * const exec_process_file;
extern const char * const exec_keybind_file;
extern const char * const exec_version_file;
extern const char * const exec_count_file;
extern const char * const exec_error_file;

extern const int exec_question_effects[51][4][9];
extern const int * const exec_questions;

extern const char * const exec_terrain_chars;
extern const char * const exec_object_chars;
extern const char * const exec_item_chars;
extern const char const exec_material_colors[14];

extern const exec_item_data_d * const exec_item_data;
extern const exec_monster_data_d * const exec_monster_data;
extern unsigned char ** const exec_terrain;
extern unsigned char ** const exec_objects;
extern exec_map_item_d *** const exec_items;
extern exec_map_monster_d ** const exec_monsters;

extern short int pairs;
extern long int turns;

extern long int * const exec_turns;
extern long int * const exec_saves;

#endif
