/**
This file contains random code that may become useful at some point.
Hopefully this will make approaching the project a bit easier.
**/

/*
Defines fake booleans.
*/
#define bool int
#define false 0
#define true 1

/*
Defines constants for dlopen (if there is no better way to do it).
*/
#define RTLD_LAZY 1
#define RTLD_NOW 2

/*
Defines internal variables.
*/
#define ADOM_turns (*((uint32_t *)0x082b16e0))
#define ADOM_random ((uint32_t *)0x082ada40)//0x100 in size

/*
Returns the smaller of two integers.
*/
inline int min(int x, int y) {
	if (x < y) return x;
	else return y;
}

/*
Returns the bigger of two integers.
*/
inline int max(int x, int y) {
	if (x > y) return x;
	else return y;
}

/*
Defines types for some library functions.
*/
typedef int (*INIT_PAIR)(short pair, short f, short b);
typedef int (*WCLEAR)(WINDOW *win);
typedef int (*WREFRESH)(WINDOW *win);
typedef int (*WMOVE)(WINDOW *win, int y, int x);
typedef int (*WADDCH)(WINDOW *win, chtype ch);
typedef int (*WADDNSTR)(WINDOW *win, const char *str, int n);
typedef chtype (*WINCH)(WINDOW *win);
typedef int (*WGETCH)(WINDOW *win);
typedef int (*WGETNSTR)(WINDOW *win, char *str, int n);
typedef int (*SPRINTF)(char *str, const char *format, ...);
typedef int (*VSPRINTF)(char *str, const char *format, va_list ap);
typedef int (*UNLINK)(const char *pathname);

/*
Declares library functions that are not overloaded.
*/
INIT_PAIR real_init_pair;
WCLEAR real_wclear;
WREFRESH real_wrefresh;
WMOVE real_wmove;
WADDCH real_waddch;
WADDNSTR real_waddnstr;
WINCH real_winch;
WGETCH real_wgetch;
WGETNSTR real_wgetnstr;
SPRINTF real_sprintf;
VSPRINTF real_vsprintf;
UNLINK real_unlink;

/*
Serves as an annotation for overloads.
*/
#define OVERLOAD(x) x

/**
Loads functions from standard libraries (libc and libncurses).
**/
#define LIBNCURSES "/lib/libncurses.so.5"
#define LIBC "/lib/i686/cmov/libc.so.6"
void load_dynamic_libraries() {
	char *libc_path;
	char *curses_path;

	/*
	Extracts library paths from environment variables.
	*/
	libc_path = getenv("LIBC_PATH");
	if (libc_path == NULL) libc_path = LIBC;
	curses_path = getenv("CURSES_PATH");
	if (curses_path == NULL) curses_path = LIBNCURSES;

	void *handle;

	/*
	Imports functions from libc.
	*/
	handle = (void*) dlopen(libc_path, 1);
	if (handle == NULL) {
		printf("<error message>\n");
		exit(1);//<error code>
	}
	real_sprintf = (SPRINTF )dlsym(handle, "sprintf");
	real_vsprintf = (VSPRINTF )dlsym(handle, "vsprintf");
	real_unlink = (UNLINK )dlsym(handle, "unlink");

	/*
	Imports functions from libncurses.
	*/
	handle = (void *)dlopen(curses_path, RTLD_LAZY);//requires either RTLD_LAZY or RTLD_NOW
	if (handle == NULL) {
		printf("<error message>\n");
		exit(1);//<error code>
	}
	real_init_pair = (INIT_PAIR )dlsym(handle, "init_pair");
	real_wclear = (WCLEAR )dlsym(handle, "wclear");
	real_wrefresh = (WREFRESH )dlsym(handle, "wrefresh");
	real_wmove = (WMOVE )dlsym(handle, "wmove");
	real_waddch = (WADDCH )dlsym(handle, "waddch");
	real_waddnstr = (WADDNSTR )dlsym(handle, "waddnstr");
	real_winch = (WINCH )dlsym(handle, "winch");
	real_wgetch = (WGETCH )dlsym(handle, "wgetch");
	real_wgetnstr = (WGETNSTR )dlsym(handle, "wgetnstr");

	/*
	Prevents reloading libraries for child processes.
	*/
	unsetenv("LD_PRELOAD");
}

/*
Wraps load_dynamic_libraries.
*/
bool initialized = false;
void initialize() {
	if (initialized) return;
	initialized = true;

	load_dynamic_libraries();
}

/*
Overloads wgetch with a simple log wrapper.
*/
int OVERLOAD(wgetch) (WINDOW *win) {
	log("call/wgetch %x %i\n",
			(ULONG_PTR )win,
			result);
	return real_wgetch(win);
}

/*
Serves as a structure for holding key inputs.
*/
typedef struct Command {
	const char *code;
	Something cmd;
};

/*
Lists all key inputs.
*/
Command *commands = {
	//inbuilt commands
	{"ACT", cmdActivateTrap},
	{"ASK", cmdApplySkill},
	{"ASC", cmdAscend},
	{"CST", cmdCast},
	{"TLK", cmdChat},
	{"HIL", cmdChangeHighlightMode},
	{"TAC", cmdChangeTactic},
	{"LIT", cmdCheckLiteracy},
	{"CLE", cmdCleanEars},
	{"CLO", cmdClose},
	{"CSE", cmdContinuousSearch},
	{"CSL", cmdCreateShortLogfile},
	{"CVL", cmdCreateVerboseLogfile},
	{"CSS", cmdCreateScreenshot},
	{"DSC", cmdDescend},
	{"DEW", cmdDescribeWeather},
	{"DIP", cmdDip},
	{"DAT", cmdDisplayAvailableTalents},
	{"DSK", cmdDisplaySkills},
	{"BKG", cmdDisplayBackground},
	{"BIL", cmdDisplayBill},
	{"DBL", cmdDisplayBurdenLevels},
	{"CFG", cmdDisplayConfiguration},
	{"CHP", cmdDisplayChaosPowers},
	{"DCI", cmdDisplayCharacterInformation},
	{"DCO", cmdDisplayCompanions},
	{"MNY", cmdDisplayCurrentWealth},
	{"DID", cmdDisplayDeity},
	{"DET", cmdDisplayElapsedGameTime},
	{"IDI", cmdDisplayIdentifiedItems},
	{"KST", cmdDisplayKickStats},
	{"DKM", cmdDisplayKilledMonsters},
	{"DLM", cmdDisplayLevelMap},
	{"MSG", cmdDisplayMessageBuffer},
	{"DMS", cmdDisplayMissileStats},
	{"MWS", cmdDisplayMonsterWoundStatus},
	{"QST", cmdDisplayQuestStatus},
	{"RCP", cmdDisplayRecipes},
	{"EXP", cmdDisplayRequiredExp},
	{"DTL", cmdDisplayTalents},
	{"VER", cmdDisplayVersion},
	{"DWS", cmdDisplayWeaponSkills},
	{"WST", cmdDisplayWeaponStats},
	{"DRK", cmdDrink},
	{"DRO", cmdDrop},
	{"DRC", cmdDropComfortably},
	{"EAT", cmdEat},
	{"EXE", cmdExamine},
	{"EDR", cmdExtDrop},
	{"EPA", cmdExtPay},
	{"EUS", cmdExtUse},
	{"GIV", cmdGive},
	{"HDL", cmdHandle},
	{"INV", cmdInventory},
	{"INM", cmdInvokeMindcraft},
	{"ISO", cmdIssueOrder},
	{"KCK", cmdKick},
	{"LOK", cmdLook},
	{"MSP", cmdMarkSpells},
	{"STF", cmdMiscEquip},
	{"MSW", cmdMoveSW},
	{"MOS", cmdMoveS},
	{"MSE", cmdMoveSE},
	{"MOW", cmdMoveW},
	{"MOE", cmdMoveE},
	{"MNW", cmdMoveNW},
	{"MON", cmdMoveN},
	{"MNE", cmdMoveNE},
	{"BAP", cmdName},
	{"HLP", cmdHelp},
	{"OPN", cmdOpen},
	{"PAY", cmdPay},
	{"PPK", cmdPickUpFast},
	{"PCK", cmdPickUp},
	{"CPC", cmdPickUpComfortably},
	{"PRA", cmdPray},
	{"QIT", cmdQuit},
	{"REA", cmdRead},
	{"RMM", cmdRecall},
	{"RED", cmdRedrawScreen},
	{"SAC", cmdSacrifice},
	{"SAV", cmdSave},
	{"SEA", cmdSearch},
	{"ST0", cmdSetTactics1},
	{"ST1", cmdSetTactics2},
	{"ST2", cmdSetTactics3},
	{"ST3", cmdSetTactics4},
	{"ST4", cmdSetTactics5},
	{"ST5", cmdSetTactics6},
	{"ST6", cmdSetTactics7},
	{"SEV", cmdSetVariable},
	{"SHT", cmdShoot},
	{"SWA", cmdSwapPosition},
	{"CAP", cmdSwitchAutoPickup},
	{"STS", cmdSwitchDynamicDisplay},
	{"CMK", cmdSwitchMoreKey},
	{"ULD", cmdUnlock},
	{"USE", cmdUseItem},
	{"UMP", cmdUseClassPower},
	{"SPA", cmdUseSpecialAbility},
	{"UTO", cmdUseTool},
	{"WAT", cmdWait},
	{"WM1", cmdWalkSW},
	{"WM2", cmdWalkS},
	{"WM3", cmdWalkSE},
	{"WM4", cmdWalkW},
	{"WM5", cmdWalkSpot},
	{"WM6", cmdWalkE},
	{"WM7", cmdWalkNW},
	{"WM8", cmdWalkN},
	{"WM9", cmdWalkNE},
	{"WIF", cmdWipeFace},
	{"ZAP", cmdZap},
	//custom commands
	{"STA", cmdStartRecording},
	{"STO", cmdStopRecording},
	{"QSA", cmdQuickSave},
	{"QLO", cmdQuickLoad},
	{"SSS", cmdSetSaveState},
	{"SNS", cmdSetNextSeed},
	{"SFT", cmdSetFrameTime},
	{"REF", cmdRefresh}
};
