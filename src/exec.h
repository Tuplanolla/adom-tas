/**
@see exec.c
@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef EXEC_H
#define EXEC_H

#include <stddef.h>//size_t

extern short pairs;
extern int turns;

extern const size_t executable_size;
extern const int executable_hash;
extern const unsigned char executable_version[4];

extern const int executable_rows_min;
extern const int executable_rows_max;
extern const int executable_cols_min;
extern const int executable_cols_max;

extern const char * const executable_data_directory;
extern const char * const executable_temporary_directory;
extern const char * const executable_temporary_file;
extern const unsigned int executable_temporary_levels;
extern const unsigned int executable_temporary_parts;
extern const char * const executable_config_file;
extern const char * const executable_process_file;
extern const char * const executable_keybind_file;
extern const char * const executable_version_file;
extern const char * const executable_count_file;
extern const char * const executable_error_file;

extern const char * const executable_config;
extern const char * const executable_keybind;

extern const char * const executable_question_strings[51];
extern const size_t executable_question_lens[51];
extern const size_t executable_question_max;
extern const int executable_question_effects[51][4][9];

extern const char const executable_terrain_chars[62];
extern const char const executable_object_chars[38];
extern const char * const executable_item_chars;
extern const char const executable_material_colors[14];

struct executable_item_data_s {
	unsigned int category;
	unsigned char stuff[4];
	//...
	unsigned int color;
	unsigned char other_stuff[88];
	//...
	unsigned int material;
	unsigned char more_stuff[8];
	//...
};
typedef struct executable_item_data_s executable_item_data_t;

struct executable_monster_data_s {
	char * name_stuff;
	char * more_name_stuff;
	char character;
	unsigned char other_stuff[3 + 52];
	//...
	unsigned int color;
	unsigned char more_stuff[68];
	//...
};
typedef struct executable_monster_data_s executable_monster_data_t;

extern const executable_item_data_t * const executable_item_data;
extern const executable_monster_data_t * const executable_monster_data;

extern int * const executable_turns;
extern int * const executable_saves;

struct executable_item_s {
	unsigned int type;
	//...
};
typedef struct executable_item_s executable_item_t;

struct executable_map_item_s {
	struct executable_item_s * item;
	struct executable_map_item_s * next;
};
typedef struct executable_map_item_s executable_map_item_t;

struct executable_monster_s {
	char name[32];
	unsigned int type;
	unsigned char stuff[20];
	//...
	int x;
	int y;
	//...
};
typedef struct executable_monster_s executable_monster_t;

struct executable_map_monster_s {
	struct executable_monster_s * monster;
	struct executable_map_monster_s * next;
};
typedef struct executable_map_monster_s executable_map_monster_t;

extern unsigned char ** const executable_terrain;
extern unsigned char ** const executable_objects;
extern executable_map_item_t *** const executable_items;
extern executable_map_monster_t ** const executable_monsters;

extern unsigned char * const executable_arc4_s;
extern unsigned char * const executable_arc4_i;
extern unsigned char * const executable_arc4_j;

extern const unsigned int executable_arc4_calls_menu;
extern const unsigned int executable_arc4_calls_automatic_load;
extern const unsigned int executable_arc4_calls_manual_load;

void sarc4(int seed);
unsigned char arc4(void);
void iarc4(unsigned int seed, unsigned int bytes);//TODO replace bytes with location_t
void iiarc4(unsigned int seed);
const char * key_code(int key);

#endif
