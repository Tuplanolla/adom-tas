/**
Sets the default values of various variables.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#include <stddef.h>//size_t

#include <curses.h>//KEY_*

#include "util.h"//intern, bool, FALSE, TRUE

#include "def.h"

/**
The name of this project.

Used in unique identifier generation.
**/
intern const char * const project_name = "adom-tas";
/**
The release version of this project.
**/
intern const char * const project_version = "1.0.0";

/**
The indicator used when a list extends left.
**/
intern const char * const def_gui_left_more = "<-";
/**
The indicator used when a list extends right.
**/
intern const char * const def_gui_right_more = "->";
/**
The indicator used on the left side of a used field.
**/
intern const char * const def_gui_left_used = "[";
/**
The indicator used on the right side of a used field.
**/
intern const char * const def_gui_right_used = "]";
/**
The indicator used on the left side of an unused field.
**/
intern const char * const def_gui_left_unused = "";
/**
The indicator used on the right side of an unused field.
**/
intern const char * const def_gui_right_unused = "";
/**
The colors used.

Ordered by hue (wavelength).
**/
intern const int def_interface_colors[6] = {
	COLOR_RED,
	COLOR_YELLOW,
	COLOR_GREEN,
	COLOR_CYAN,
	COLOR_BLUE,
	COLOR_MAGENTA
};

/**
The separator between error message fields.
**/
intern const char * const def_log_separator = " - ";
/**
The error string.
**/
intern const char * const def_log_error = "Error";
/**
The warning string.
**/
intern const char * const def_log_warning = "Warning";
/**
The notice string.
**/
intern const char * const def_log_notice = "Note";
/**
The library call string.
**/
intern const char * const def_log_call = "Call";

/**
The location of the executable.
**/
intern const char * const def_exec_path = "adom/adom";
/**
The location of this library.
**/
intern const char * const def_lib_path = "bin/adom-tas.so";
/**
The location of the C standard library.
**/
intern const char * const def_libc_path = "/lib/libc.so.6";
/**
The location of the New Cursor Optimization library.
**/
intern const char * const def_libncurses_path = "/usr/lib/libncurses.so.5";
/**
Whether default configurations are enforced.

Custom configurations may cause desynchronization during played back.
**/
intern const int def_enforce = TRUE;
/**
The amount of save states.
**/
intern const int def_states = 9;
/**
The height of the terminal.
**/
intern const int def_rows = 25;
/**
The width of the terminal.
**/
intern const int def_cols = 77;
/**
The location of the shared memory segment.

For System V it's relative to the current working directory and
 for POSIX it's relative to /dev/shm.
**/
intern const char * const def_shm_path = "adom-tas.shm";
/**
The initial system time.
**/
intern const int def_timestamp = 0;
/**
The amount of generated characters.
**/
intern const int def_generations = 100;
/**
Whether save-quit-load emulation is enabled.
**/
intern const int def_sql = TRUE;
/**
Whether save states are preserved upon loading them.
**/
intern const int def_preserve = TRUE;
/**
Whether the input file is played back automatically.
**/
intern const int def_autoplay = FALSE;
/**
Whether the graphical user interface is colorful.
**/
intern const int def_monochrome = TRUE;
/**
The string to replace with a number.
**/
intern const char * const def_iterator = "#";
/**
The location of the input file.
**/
intern const char * const def_input_path = "input.tas";
/**
The location of the output file.
**/
intern const char * const def_output_path = "output.tas";
/**
The name of the error stream.
**/
intern const char * const def_error_stream = "stderr";
/**
The name of the warning stream.
**/
intern const char * const def_warning_stream = "stderr";
/**
The name of the notice stream.
**/
intern const char * const def_notice_stream = "stderr";
/**
The name of the call stream.
**/
intern const char * const def_call_stream = "/dev/null";
/**
The key used to save the active state.
**/
intern const int def_save_key = KEY_F(2);
/**
The key used to load the active state.
**/
intern const int def_load_key = KEY_F(3);
/**
The key used to change the active state by incrementing.
**/
intern const int def_state_key = KEY_F(8);
/**
The key used to change the active state by decrementing.
**/
intern const int def_unstate_key = MOD_SHIFT + KEY_F(8);
/**
The key used to change the frame duration by incrementing.
**/
intern const int def_duration_key = KEY_F(5);
/**
The key used to change the frame duration by decrementing.
**/
intern const int def_unduration_key = MOD_SHIFT + KEY_F(5);
/**
The key used to change the system time by incrementing.
**/
intern const int def_time_key = KEY_F(6);
/**
The key used to change the system time by decrementing.
**/
intern const int def_untime_key = MOD_SHIFT + KEY_F(6);
/**
The key used to open or close the save state menu.
**/
intern const int def_menu_key = KEY_F(9);
/**
The key used to open or close the information screen.
**/
intern const int def_info_key = MOD_SHIFT + KEY_F(9);
/**
The key used to condense or expand the graphical user interface.
**/
intern const int def_condense_key = KEY_F(10);
/**
The key used to hide or show the graphical user interface.
**/
intern const int def_hide_key = MOD_SHIFT + KEY_F(10);
/**
The key used to play back a record.
**/
intern const int def_play_key = KEY_F(11);
/**
The key used to stop a record to create a branch.
**/
intern const int def_stop_key = MOD_SHIFT + KEY_F(11);
/**
The key used to terminate all processes.
**/
intern const int def_quit_key = MOD_SHIFT + KEY_F(12);

/**
The location of the configuration file.
**/
intern const char * const def_config_path = "adom-tas.cfg";
/**
The template configuration.
**/
intern const char * const def_config = \
		"#required\n"
		"executable     = \"adom/adom\"\n"
		"data           = \"~/.adom.data\"\n"
		"loader         = \"bin/adom-tas.so\"\n"
		"libc           = \"/lib/libc.so.6\"\n"
		"libncurses     = \"/usr/lib/libncurses.so.5\"\n"
		"\n"
		"#optional\n"
		"#home           = \"/home/user\"\n"
		"#enforce        = true\n"
		"#states         = 4\n"
		"#rows           = 25\n"
		"#cols           = 77\n"
		"#shm            = \"adom-tas.shm\"\n"
		"#generations    = 100\n"
		"#timestamp      = 189284349\n"
		"#sql            = true\n"
		"#preserve       = true\n"
		"#autoplay       = false\n"
		"#monochrome     = false\n"
		"#iterator       = \"#\"\n"
		"#input          = \"input.tas\"\n"
		"#output         = \"output.#.tas\"\n"
		"#error          = \"adom-tas.log\"\n"
		"#warning        = \"adom-tas.log\"\n"
		"#notice           = \"adom-tas.log\"\n"
		"#call           = \"/dev/null\"\n"
		"#save_key       = 266#F2\n"
		"#load_key       = 267#F3\n"
		"#state_key      = 272#F8\n"
		"#unstate_key    = 284#Shift F8\n"
		"#duration_key   = 270#F5\n"
		"#unduration_key = 282#Shift F5\n"
		"#time_key       = 269#F6\n"
		"#untime_key     = 281#Shift F6\n"
		"#menu_key       = 273#F9\n"
		"#info_key       = 285#Shift F9\n"
		"#condense_key   = 274#F10\n"
		"#hide_key       = 286#Shift F10\n"
		"#play_key       = 275#F11\n"
		"#stop_key       = 287#Shift F11\n"
		"#quit_key       = 288#Shift F12\n"
	;

/**
The default configuration of the executable.
**/
intern const char * const def_exec_config = \
		"Allow_Default_Names    = false\n"
		"Auto_Lock_Doors        = false\n"
		"Auto_Lock_Nearest      = true\n"
		"Auto_Open_Doors        = true\n"
		"Auto_Pickup            = ['~(}/]{=\\!?\"%$*\n"
		"Auto_Select_Doors      = true\n"
		"Base_Delay_Factor      = 1\n"
		"Check_Item_In_Hand     = true\n"
		"Colored_Menus          = true\n"
		"Colored_PC             = true\n"
		"Fast_Missile_Readying  = true\n"
		"Fast_More              = true\n"
		"Item_Status_Color      = cyan\n"
		"Low_Hitpoint_Warning   = true\n"
		"Message_Buffer_Size    = 10000\n"
		"Metric_Measuring       = true\n"
		"Nice_Stuff             = true\n"
		"No_Book_Spellcasting   = false\n"
		"Persistence            = 10000\n"
		"Questioned_Attributes  = true\n"
		"Reload_Missiles        = false\n"
		"Reverse_Message_Order  = true\n"
		"Show_Experience        = true\n"
		"Sorted_Skills          = true\n"
		"Starvation_Warning     = true\n"
		"Uncursed_String        = mundane\n"
		"Verbose_Level_Messages = false\n"
		"Walk_Carefully         = true\n"
		"Zap_Wands_In_Hand      = true\n"
	;
/**
The default keybindings of the executable.
**/
intern const char * const def_exec_keybind = \
		"#Activate trap\n"
		"ACT:\\Ct\n\n"
		"#Apply skill\n"
		"ASK:a\n\n"
		"#Ascend stairway/Leave location\n"
		"ASC:<\n\n"
		"#Cast spell\n"
		"CST:Z\n\n"
		"#Chat with monsters\n"
		"TLK:C\n\n"
		"#Change highlight mode\n"
		"HIL:H\n\n"
		"#Change tactic\n"
		"TAC:T\n\n"
		"#Check literacy\n"
		"LIT:L\n\n"
		"#Clean ears\n"
		"CLE:E\n\n"
		"#Close door\n"
		"CLO:c\n\n"
		"#Continuous search\n"
		"CSE:ws\n\n"
		"#Create short character logfile\n"
		"CSL:(\n\n"
		"#Create verbose character logfile\n"
		"CVL:)\n\n"
		"#Create screenshot\n"
		"CSS:[\n\n"
		"#Descend stairway/Enter location\n"
		"DSC:>\n\n"
		"#Describe weather\n"
		"DEW::W\n\n"
		"#Dip (something) into (potion)\n"
		"DIP:!\n\n"
		"#Display available talents\n"
		"DAT::T\n\n"
		"#Display and quick-mark skills\n"
		"DSK:A\n\n"
		"#Display background\n"
		"BKG:B\n\n"
		"#Display bill\n"
		"BIL:P\n\n"
		"#Display burden levels\n"
		"DBL::b\n\n"
		"#Display configuration variables\n"
		"CFG:=\n\n"
		"#Display chaos powers\n"
		"CHP:\\\\\n\n"
		"#Display character information\n"
		"DCI:\\Mq\n\n"
		"#Display companions\n"
		"DCO::c\n\n"
		"#Display current wealth\n"
		"MNY:$\n\n"
		"#Display name of your deity\n"
		"DID::g\n\n"
		"#Display elapsed game time\n"
		"DET:\\Ce\n\n"
		"#Display identified items\n"
		"IDI:/\n\n"
		"#Display kick statistics\n"
		"KST:K\n\n"
		"#Display killed monsters\n"
		"DKM::k\n\n"
		"#Display level map\n"
		"DLM:\\Ml\n\n"
		"#Display message buffer\n"
		"MSG::m\n\n"
		"#Display missile statistics\n"
		"DMS:M\n\n"
		"#Display monster wound status\n"
		"MWS::w\n\n"
		"#Display quest status\n"
		"QST:q\n\n"
		"#Display recipes\n"
		"RCP:R\n\n"
		"#Display required experience\n"
		"EXP:x\n\n"
		"#Display talents\n"
		"DTL:\\Mt\n\n"
		"#Display version\n"
		"VER:V\n\n"
		"#Display weapon skills\n"
		"DWS:\\Cw\n\n"
		"#Display weapon statistics\n"
		"WST:W\n\n"
		"#Drink\n"
		"DRK:D\n\n"
		"#Drop item(s)\n"
		"DRO:d\n\n"
		"#Drop items in a comfortable way\n"
		"DRC:\\Cd\n\n"
		"#Eat\n"
		"EAT:e\n\n"
		"#Examine environment\n"
		"EXE:l\n\n"
		"#Extended drop\n"
		"EDR::d\n\n"
		"#Extended pay\n"
		"EPA::p\n\n"
		"#Extended use\n"
		"EUS::u\n\n"
		"#Give item to monster\n"
		"GIV:g\n\n"
		"#Handle something\n"
		"HDL:h\n\n"
		"#Inventory\n"
		"INV:i\n\n"
		"#Invoke mindcraft\n"
		"INM:\\Ci\n\n"
		"#Issue order to companion\n"
		"ISO:\\Co\n\n"
		"#Kick\n"
		"KCK:k\n\n"
		"#Look\n"
		"LOK:\\Cl\n\n"
		"#Mark spells\n"
		"MSP::Z\n\n"
		"#Miscellaneous equipment\n"
		"STF:I\n\n"
		"#Move to the southwest\n"
		"MSW:1\n\n"
		"#Move to the south\n"
		"MOS:2 \\D\n\n"
		"#Move to the southeast\n"
		"MSE:3\n\n"
		"#Move to the west\n"
		"MOW:4 \\L\n\n"
		"#Move to the east\n"
		"MOE:6 \\R\n\n"
		"#Move to the northwest\n"
		"MNW:7\n\n"
		"#Move to the north\n"
		"MON:8 \\U\n\n"
		"#Move to the northeast\n"
		"MNE:9\n\n"
		"#Name monster/yourself\n"
		"BAP:n\n\n"
		"#Online help\n"
		"HLP:?\n\n"
		"#Open door\n"
		"OPN:o\n\n"
		"#Pay\n"
		"PAY:p\n\n"
		"#Pick up items primitively (fast)\n"
		"PPK:;\n\n"
		"#Pick up items\n"
		"PCK:,\n\n"
		"#Pick up items comfortably\n"
		"CPC:\\Cp\n\n"
		"#Pray\n"
		"PRA:_\n\n"
		"#Quit game\n"
		"QIT:Q\n\n"
		"#Read\n"
		"REA:r\n\n"
		"#Recall monster memory\n"
		"RMM:&\n\n"
		"#Redraw screen\n"
		"RED:\\Cr\n\n"
		"#Sacrifice\n"
		"SAC:O\n\n"
		"#Save and quit game\n"
		"SAV:S\n\n"
		"#Search\n"
		"SEA:s\n\n"
		"#Set tactics to 'berserker'\n"
		"ST0:\\1\n\n"
		"#Set tactics to 'very aggressive'\n"
		"ST1:\\2\n\n"
		"#Set tactics to 'aggressive'\n"
		"ST2:\\3\n\n"
		"#Set tactics to 'normal'\n"
		"ST3:\\4\n\n"
		"#Set tactics to 'defensive'\n"
		"ST4:\\5\n\n"
		"#Set tactics to 'very defensive'\n"
		"ST5:\\6\n\n"
		"#Set tactics to 'coward'\n"
		"ST6:\\7\n\n"
		"#Set variable\n"
		"SEV::=\n\n"
		"#Shoot/Throw missile\n"
		"SHT:t\n\n"
		"#Swap position with monster\n"
		"SWA::s\n\n"
		"#Switch auto-pickup on/off\n"
		"CAP:\\Ca\n\n"
		"#Switch the dynamic display\n"
		"STS::t\n\n"
		"#Switch the required (more) key\n"
		"CMK:\\Ck\n\n"
		"#Unlock door\n"
		"ULD:\\Cu\n\n"
		"#Use item\n"
		"USE:U\n\n"
		"#Use class power\n"
		"UMP:\\Cx\n\n"
		"#Use special ability\n"
		"SPA:m\n\n"
		"#Use tool\n"
		"UTO:u\n\n"
		"#Wait\n"
		"WAT:. 5\n\n"
		"#Walk to the southwest\n"
		"WM1:w1\n\n"
		"#Walk to the south\n"
		"WM2:w2\n\n"
		"#Walk to the southeast\n"
		"WM3:w3\n\n"
		"#Walk to the west\n"
		"WM4:w4\n\n"
		"#Walk on the spot\n"
		"WM5:w5\n\n"
		"#Walk to the east\n"
		"WM6:w6\n\n"
		"#Walk to the northwest\n"
		"WM7:w7\n\n"
		"#Walk to the north\n"
		"WM8:w8\n\n"
		"#Walk to the northeast\n"
		"WM9:w9\n\n"
		"#Wipe face\n"
		"WIF:F\n\n"
		"#Zap wand\n"
		"ZAP:z\n"
	;

