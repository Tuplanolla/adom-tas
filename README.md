# ADoM TAS (adom-tas)

ADoM TAS stands for Ancient Domains of Mystery Tool-Assisted Speedruns.
It makes recording deterministic runs as input sequences possible and thus
 allows creating theoretically perfect speedruns and
 other demonstrations.
Its possibilities are not restricted to speedruns although it's the main focus.
The name is not very descriptive since it's built around an acronym.

## Meta

This readme is incomplete.

## Schedule

This project is in active development and
 the first version was finished by 2012-07-01,
 the first working binaries were built 2012-07-02 and
 the rest should be done by 2012-08-01.

## Motivation

### ADoM

ADoM is a roguelike video game and as such characterized by
 turn-based movement,
 random level generation,
 permanent death,
 textual graphics,
 luck-based gameplay,
 an unnecessarily complicated metagame,
 an inconvenient user interface and
 bad programming.
Altogether it's an excellent game to
 disassemble and
 learn from other people's mistakes.

### Tool-Assisted Speedruns

Tool-assisted speedruns combine two concepts: tool-assisting and speedrunning.
Tool-assisting means controlling time and the environment to
 make theoretically flawless execution possible.
Speedrunning obviously means playing as fast as possible.
Typically being fast means minimizing the time spent,
 but for turn-based games the goal may be different.

## Installation

ADoM TAS is written in C and only works on Linux since it
 relies on injecting assembly instructions to another executable and
 makes heavy use of features like `fork` and `execve`.
ADoM TAS comes with a copy of ADoM that's
 unaltered and
 packaged as it was originally distributed
  as outlined in its license.
ADoM TAS can be installed from
 binaries (already compiled executables) or
 sources (text files that need to be compiled).
In either case it's recommended to
 browse through the whole installation instructions and
 possibly the troubleshoot section.

The following examples are from a (freshly installed) Arch Linux.

### Binaries

Binaries only need to be downloaded and extracted.

	[user@arch ~]$ wget https://github.com/Tuplanolla/adom-tas/blob/master/adom-tas.tar.gz
	[user@arch ~]$ tar -xf adom-tas.tar.gz

They may be older than the sources.

### Sources

Building ADoM TAS relies on the GNU Compiler Collection and GNU Make although
 any other C compiler and build automation tool should work as well.
GNU extensions are used, but not required.

	[user@arch ~]$ pacman -S gcc make

The libraries ADoM TAS depends on are
 the C standard library `libc`,
 the New Cursor Optimization library `libncurses` and
 a configuration file library `libconfig`.

	[user@arch ~]$ pacman -S libc libncurses libconfig

Acquiring ADoM TAS from GitHub also requires
 SSH and
 Git.

	[user@arch ~]$ pacman -S ssh git

Once the required packages are installed the repository can be cloned

	[user@arch ~]$ git clone git@github.com:Tuplanolla/adom-tas.git

and ADoM TAS can be built.

	[user@arch ~]$ cd adom-tas
	[user@arch adom-tas]$ make

The binaries go in the `bin` directory and
 temporary objects in the `obj` directory.
The object files can be removed after compilation.

	[user@arch adom-tas]$ rm -f obj/*

## Running

Note that the configuration files used by ADoM are overwritten by default
 to achieve consistency in recorded input sequences.
To preserve the configuration files move or copy them elsewhere first.

	[user@arch adom-tas]$ cp -u ~/.adom.data ~/adom.data

Running the launcher for the first time will
 generate a template configuration file in the current working directory and
 terminate with a note.

	[user@arch adom-tas]$ bin/adom-tas

The configuration file may need to be edited.
The process of doing so is addressed in its own section.

	[user@arch adom-tas]$ nano adom-tas.cfg

After taking care of the configuration file the launcher will start properly.

	[user@arch adom-tas]$ bin/adom-tas

## Recording

Encoding video files requires
 the Teletypewriter Recorder `ttyrec` for `*.tty` files and
 FFmpeg `ffmpeg` for `*.avi` files.
Some formats also require
 the Audio/Video Codec library `libavcodec` or
 the Audio/Video Filter library `libavfilter`.

	[user@arch adom-tas]$ pacman -S ttyrec ffmpeg

Recording is managed by shell scripts.

	[user@arch adom-tas]$ bin/ttyrec.sh -e bin/adom-tas -o output.tty
	[user@arch adom-tas]$ bin/ffmpeg.sh -e bin/adom-tas -o output.avi

Advanced options like quality and filtering are also available

	[user@arch adom-tas]$ bin/ffmpeg.sh -e "ttyplay output.tty" -r 32 -m 16 -s sd -o output.avi

and can be found in the help.

	[user@arch adom-tas]$ bin/ffmpeg.sh -h

It is currently recommended to
 first record a `tty` file and
 then convert it to an `avi` file
  since `adom-tas` is only synchronized with `nanosleep` so
  frame processing times skew the frame durations.

## Configuration

ADoM TAS uses a configuration file since
 command-line arguments are passed through to ADoM and
 the configuration is mostly static.
When ADoM TAS is run it
 checks for the existence of its configuration file and
 creates the file from a template if it doesn't exist.
By default the configuration file is called `adom-tas.cfg`
 in the current working directory,
but the default location can be changed by
 modifying the variable `default_config_path` in the file `def.c` and
 rebuilding ADoM TAS.
The default file extension is `*.cfg` since
 the configuration file library uses it by default and
 `*.conf` was originally planned to be reserved for a configuration language.

The configuration file contains

* the location of the ADoM `executable`,
* the location of ADoM's `data` directory,
* the location of ADoM TAS's `loader`,
* the location of the C standard library `libc`,
* the location of the New Cursor Optimization library `libncurses` and
* whether default configuration files are enforced (`true` by default),
* the location of the `home` directory,
* the amount of save `states` excluding the currently active state (at least `1` and `9` by default),
* the height of the terminal in `rows` (at least `25`, at most `127` and `25` by default),
* the width of the terminal `cols` in columns (at least `77`, at most `127` and `80` by default),
* the location of the shared memory segment lock `shm` (`adom-tas.shm` by default),
* the amount of character `generations` (`100` by default),
* the `timestamp` of the initial system time (`0` by default),
* whether the save-quit-load emulation is enabled (`true` by default),
* whether playback starts automatically if an input file is present (`false` by default),
* whether the user interface has colorful backgrounds (`true` by default),
* the string `iterator` to replace with the save state number when processing output file names (`#` by default),
* the location of the `input` file for playback (`input.tas` by default),
* the location of the `output` files for recording (recommended to be `output.#.tas` and `output.tas` by default),
* the location of the `error` log (`stderr` by default),
* the location of the `warning` log (`stderr` by default),
* the location of the `note` log (`stderr` by default),
* the location of the `call` log (`/dev/null` by default) and
* the `key` numbers of various commands.

The standard streams are `stdin`, `stdout` and `stderr`.
All file paths can be
 absolute,
 relative or
 linked and
  the shell variable `~` is expanded to the home directory.
The shared memory segment lock file has to exist,
but its contents don't matter since it's only used for identification.

The template configuration should work out of the box.
If ADoM TAS is run from the same directory it's maked from (sic)
 the only values that may be wrong are the library locations.
The libraries can be searched for to make life easier.

	[user@arch ~]$ find /lib /usr/lib -maxdepth 1 \( -name libc.so.\* -o -name libncurses.so.\* \) 2>/dev/null

The input file is read when the playback key is pressed on the first frame.
The output file is written when the corresponding save state is used.

## User Interface

The user interface deserves a mention since it's so intuitive.

The status bar looks like

	I: \M\Cf  F: 2/21  T: 0/7  D: 1/2  E: 15  R: 0xe87de001  S: 2/9  P: 16384

and contains
 the last recorded inputs (Alt Ctrl F),
 the amount of last recorded frames (2) and
  the amount of all frames (21),
 the amount of actual turns elapsed since the last input (0) and
  the amount of all actual turns (7),
 the duration of the next frame (half a second),
 the time elapsed since the last emulated save-quit-load process (15 seconds),
 the current hash of the random number generator's state and
 the currently selected save state (#2) and
  the amount of all save states (9) and
 the current process identifier.

By default

* `F2` saves the current save state,
* `F3` loads it,
* `F8` selects the next save state,
* `Shift F8` selects the previous one,
* `F5` increases (doubles) the duration of the next frame,
* `Shift F5` decreases (halves) it,
* `F6` shifts the system time forwards (by one second),
* `Shift F6` shifts it backwards (but not to a negative value),
* `F9` opens or closes the save state menu,
* `F10` condenses or expands the user interface,
* `Shift F10` hides and shows it,
* `F11` plays or pauses a record (but only starting on the first frame),
* `Shift F11` stops a record (and enables appending to it),
* `Shift F12` quits and
* the save key (typically `S`) emulates the save-quit-load process.

The keys only used to interact with ADoM TAS are not recorded.

## Troubleshooting

Compilation failed with `No such file or directory`? Consider

* ensuring the installed libraries are
	* `libncurses` instead of `libcurses` and
	* `libconfig` instead of `libconfig++`,
* checking that `gcc` and `make` are installed properly and
* making sure the commands `rm`, `mkdir` and `cp` are unaltered.

Running failed with `Parsing the configuration file failed`? Consider

* making sure the syntax is correct with
	* an equals sign `=` between keys and values and
	* strings enclosed in quotation marks `"`,
* updating libconfig,
* taking care of legacy problems by
	* adding semicolons `;` to the end of each line and
	* ensuring the last line ends with a line break `\n`,
* removing uncommon whitespace characters like no-break spaces `\xa0` or
* generating a new configuration file.

Something else happened? Consider

* trying to understand the error message better,
* looking it up in the well-documented source code,
* reinstalling ADoM TAS or
* asking for help.

## Notes

The attributes of a character are set when
 the text "You are born to be a male gnome." appears and
 varied until the text "You are now a fully learned wizard." appears.
The items of a character are generated when
 the talent menu opens.
A character is considered generated when
 the text "You are now a fully learned wizard." appears.

Putting the resource file `ADOM.DBG` in the current working directory will
 enable ADoM's debug mode.
I have no idea what it does,
 but hopefully someone can figure it out and tell me.

## Development

### Tracing

ADoM TAS comes with a wrapper for `ltrace` that logs library calls.

	[user@arch adom-tas]$ pacman -S ltrace

The log files it generates are enormous, so use it with care.

	[user@arch adom-tas]$ bin/ltrace.sh

### Documentation

It's possible to generate automated documentation for ADoM TAS with Doxygen.

	[user@arch adom-tas]$ pacman -S doxygen

The process is similar to makeing (sic).

	[user@arch adom-tas]$ doxygen

### Conventions

The naming conventions used in the project follow those of
 the implementations and
 the man pages
  of similar functions in the C standard library.

The syntax conventions in the other hand follow
 the simplest possible consistent set of rules that allows
  condensed and
  patterned
   code.
Keywords are always followed by spaces and functions never,
binary operators are always separated by spaces and unary never,
pointers are always separated from their types and identifiers
 since their binding varies depending on the context,
comments and casts are removable using the simplest possible pattern to
 leave no traces of their existence and
continued lines are always indented twice to
 separate them from scopes they may start
to name a few.

### Directory Structure

Files are named and organized in a typical manner. The directories are

* `/` for configurations,
* `/src` for sources,
* `/obj` for temporary build files,
* `/lib` for libraries,
* `/bin` for binaries,
* `/res` for resources,
* `/doc` for documentation,
* `/doxygen` for automated documentation,
* `/cat` for catalogs,
* `/rec` for records and
* `/adom` for ADoM.

### File Format

Records are saved to `*.tas` files in a custom format. The files contain

* a 4-byte `char [4]` header (always `54 41 53 00` for "TAS"),
* a 256-byte `char [256]` executable name (for example `61 64 6f 6d 00 ...` for ADoM),
* a 4-byte `unsigned int` category (where `00 00 00 00` is uncategorized),
* a 256-byte `char [256]` author (for example `54 75 70 6c 61 6e 6f 6c 6c 61 00 ...` for "Tuplanolla"),
* a 4-byte `unsigned int` frames,
* a 4-byte `unsigned int` time,
* a 4-byte `unsigned int` turns,
* padding to 1024-byte alignment and
* executable-specific `frame_t` chunks.

For ADoM the categories are

* `01 00 00 00` for minimum actual turns (without negative turns) and
* `02 00 00 00` for minimum ideal time (without saving, quitting and loading)
* although all categories up to `ff 00 00 00` are reserved for challenge games

and the chunks consist of

* a 1-byte `unsigned char` duration and
* a 4-byte `time_t` excerpt.

Additionally characters are catalogued to `*.tac` files. The files contain

* a 4-byte `char [4]` header (always `54 41 67 00` for "TAC"),
* a 256-byte `char [256]` executable name (for example `61 64 6f 6d 00 ...` for ADoM),
* padding to 1024-byte alignment and
* an executable-specific `catalog_t` chunk.

For ADoM the chunk consists of

* a 4-byte `unsigned int` birthday (day of the year minus one),
* a 4-byte `unsigned int` sex,
* a 4-byte `unsigned int` race,
* a 4-byte `unsigned int` profession,
* a 52-byte `char [51]` list of answers,
* a 4-byte `unsigned int` potential crowning gift,
* a 36-byte `unsigned int [9]` list of initial attributes,
* a 2788-byte `unsigned int [697]` list of identified items and
* a 188-byte `unsigned int [47]` list of identified books.

### Making of ADoM TAS

June 2012 was when it all begun...

...and that's it (more will be written later).

#### Random Number Generator

The best way to record what's essentially a nondeterministic finite automaton
 is finding the sequence of inputs that result in the desired output.
Eliminating entropy guarantees the same inputs always map to the same output,
 which is essential to avoid desynchronization.
Practically it means
 finding out what kind of a random number generator is used,
 emulating it to predict how it changes and
 controlling external factors that affect it.

The GNU Debugger is used to
 create a controlled test environment and
 find out how the random number generator works.

	[user@arch ~]$ cd adom
	[user@arch adom]$ gdb adom

Commonly used random number generators are
 `rand`,
 `random` and
 `drand48`.
To find out which of them are linked with the executable
 the `info functions` command
  is used.

	(gdb) i fu rand
	0x08049380  random
	0x080496d0  srandom

The nonlinear additive feedback random number generator `random` and
 its seed function `srandom`
  are found.
Breakpoints set with the `break` command and
 `ignore`d to
  help count how many times they're called.
The `run` command starts the process.

	(gdb) b random
	(gdb) b srandom
	(gdb) ig 1 0xffff
	(gdb) ig 2 0xffff
	(gdb) r

Once the process terminates
 `info break` is used to inspect the breakpoints and
 `delete breakpoints` to clean up afterwards.

	(gdb) i b
	(gdb) d br

Both of the functions are only called once, so
 a custom random number generator is used and
 the functions are likely there just to initialize it.
However `random` is only a pseudo-random number generator and
 as such requires a source of entropy.
Timers are commonly used as such sources.

	(gdb) i fu time
	0x080492e0  localtime
	0x08049420  time
	0x08049610  notimeout
	0x08049700  wtimeout

Only `time` and `localtime` are interesting since
 they're absolute timers and
 reside in the standard library.
Breakpoints help count how many times they're called again.

	(gdb) b localtime
	(gdb) b time
	(gdb) ig 3 0xffff
	(gdb) ig 4 0xffff
	(gdb) r
	(gdb) i b
	(gdb) d 3

Since `time` is also called once at startup, it's likely that
 `srandom(time(NULL))` is first called to
  seed the pseudo-random number generator and
 `random()` is then called to
  seed the custom random number generator.
The assumption is checked by using
 `break` and
 `finish`
  to position `$pc` (the program counter) and
  e`x`amine `i`nstructions to read the disassembly.

	(gdb) ig 4 0
	(gdb) r
	(gdb) fin
	(gdb) x /17i $pc - 0x13
		0x08125d10:  push  %ebp
		0x08125d11:  mov   %esp, %ebp
		0x08125d13:  sub   $0x8, %esp
		0x08125d16:  add   $0xfffffff4, %esp
		0x08125d19:  add   $0xfffffff4, %esp
		0x08125d1c:  push  $0x0
	    0x08125d1c:  push  $0x0
	    0x08125d1e:  call  0x08049420 <time@plt>
	=>  0x08125d23:  push  %eax
	    0x08125d24:  call  0x080496d0 <srandom@plt>
	    0x08125d29:  add   $0x20,%esp
	    0x08125d2c:  add   $0xfffffff4,%esp
	    0x08125d2f:  call  0x08049380 <random@plt>
	    0x08125d34:  push  %eax
	    0x08125d35:  call  0x08125ea0
	    0x08125d3a:  mov   %ebp,%esp
	    0x08125d3c:  pop   %ebp
	    0x08125d3d:  ret

Random number generators often store their state statically, so
 the location is most likely fixed and
 the random number generator can be identified by analyzing its storage's
  size and
  contents.
To know where to look at
 `info proc mappings` shows the location of the heap.

	(gdb) i proc m
	0x08048000  0x08262000  0x21a000  0x0       /home/user/adom/adom
	0x08262000  0x0829f000  0x3d000   0x219000  /home/user/adom/adom
	0x0829f000  0x082ec000  0x4d000   0x0       heap

The heap is
 dumped to files `m1` and `m2` at different times with the help of
  `define`,
  `dump memory`,
  `info stack`,
  `disable` and
  `continue` and
 compared with the `shell` command
  `cmp`.

	(gdb) def dm
	du m $arg0 0x0829f000 0x082ec000
	end
	(gdb) i s
	#0  0xb7fda14f in time from /lib/libc.so.6
	#1  0x08125d23 in ??
	#2  0x080dd8fb in ??
	#3  0x081503da in ??
	#4  0xb7df43d5 in __libc_start_main from /lib/libc.so.6
	(gdb) disa 1
	(gdb) b *0x081503da
	(gdb) c
	(gdb) dm m1
	(gdb) c
	(gdb) r
	(gdb) dm m2
	(gdb) disa 2
	(gdb) c
	(gdb) she cmp -l m1 m2 | wc -l

The heaps differ by 260 bytes, which indicates
 a 256-byte state and
 a 4-byte collection of iterators, counters or unrelated garbage.
Upon closer inspection one of the bytes stands out as
 it's only ever incremented by one and
 another byte next to it is always incremented with it,
  but by an arbitrary amount.
Two of the bytes change haphazardly, so they're probably unrelated.
The layout of the storage means
 the random number generator is most likely
  a variation of an ARC4 stream cipher.
ARC4 uses three variables: the state S and the iterators i and j.

	(gdb) set $s = (unsigned char * )0x082ada40
	(gdb) set $l = (unsigned int )256
	(gdb) set $i = (unsigned char * )0x082adb40
	(gdb) set $j = (unsigned char * )0x082adb41

In order to get reliable test results
 entropy has to be removed.
The breakpoint is `enable`d to catch `time` again.

	(gdb) ena 4
	(gdb) r
	(gdb) fin

Inspecting the registers with `info registers`
 help find the return value.

	(gdb) i r
	eax     0x4fc90000  +1338572800
	ecx     0xbffff694  -1073744236
	edx     0x00000001  +1
	ebx     0xbffff9a4  -1073743452
	esp     0xbffff8b4  0xbffff8b4
	ebp     0xbffff8d8  0xbffff8d8
	esi     0x00000001  +1
	edi     0x00000000   0
	eip     0x08125d23  0x08125d23
	eflags  0000000296  PF | AF | SF | IF
	cs      0x00000073  +115
	ss      0x0000007b  +123
	ds      0x0000007b  +123
	es      0x0000007b  +123
	fs      0x00000000   0
	gs      0x00000033  +51

The `eax` register seems to contain an timestamp of 2012-06-01 17:46:40 UTC and
 needs to be re`set`.

	(gdb) set $eax = 0

It's 1970-01-01 00:00:00 UTC again and
 no further actions are needed, since `time` is only called once.
The variables of the random number generator can now be tracked reliably
 with the `watch` command.

	(gdb) wa *$s
	(gdb) wa *$i
	(gdb) wa *$j
	(gdb) c

First `s` changes from 0 to 103 at 0x08125f2c,
 so the watchpoints are late to the party.
Analyzing the state should tell how late exactly.

	(gdb) def ds
	du m $arg0 $s $s + $l
	end
	(gdb) ds su

The first byte is 0x67 and the rest are uninitialized (so that `s[i] = i`),
 so not too late.

	(gdb) c
	(gdb) c

Then `j` changes from 0 to 15 at 0x0812615d and
 `i` from 0 to 1 in 0x081261a3.
The random number generator is obviously a variant of ARC4.
The initialization has already started,
 so the state can't be inspected any further,
 but the breakpoints for the next run can be set now.

	(gdb) disa 5
	(gdb) i s
	#0  0x081261a3 in ??
	#1  0x08125ba2 in ??
	#2  0x080dd208 in ??
	#3  0x080d166d in ??
	#4  0x081504ec in ??
	#5  0xb7df43d5 in __libc_start_main from /lib/libc.so.6
	(gdb) fin
	(gdb) fin
	(gdb) fin
	(gdb) fin
	(gdb) x /4i $pc - 0x8
	   0x81504e4:  add   $0x20,%esp
	   0x81504e7:  call  0x080d15f0
	=> 0x81504ec:  add   $0xfffffff4,%esp
	   0x81504ef:  push  $0x082614eb
	(gdb) b *0x081504e7
	(gdb) c

The initial state is dumped once the breakpoint is reached.

	(gdb) r
	(gdb) fin
	(gdb) set $eax = 0
	(gdb) c
	(gdb) ds s1

The next two states are dumped after the iterators have changed.

	(gdb) c
	(gdb) c
	(gdb) ds s2
	(gdb) c
	(gdb) c
	(gdb) ds s3

The state files `s1`, `s2` and `s3` show how to initialize a replica ARC4.
The debugger has done its job and may `quit`.

	(gdb) d br
	(gdb) c
	(gdb) q

However since the custom random number generator operates cyclically,
 its output must also to be replicated.
It's more complicated, so `objdump` does the heavy lifting.

	[user@arch adom]$ objdump -d -w adom >adom.s

The disassembly is 333620 lines long,
 but searching for the relevant parts (accesses to `s`, `i` and `j`) is easy.

	.data
	s:
	0x082ada40:  00 01 02 ... ff       .byte   0, 1, 2, ..., 255
	i:
	0x082adb40:  00                    .byte   0
	j:
	0x082adb41:  00                    .byte   0
	n:
	0x082beb00:  00 00 00 ... 00       .int    0, 1, 2, ..., 19
	z:
	0x082beb4c:  00                    .int    0
	.text
	init:
	0x0815055c:  e8 3f f3 fc ff        call    0x0811f8a0 <init_rng>
	init_rng:
	0x0811f8a0:  55                    push    %ebp
	0x0811f8a1:  89 e5                 mov     %esp, %ebp
	0x0811f8a3:  83 ec 0c              sub     $0xc, %esp
	0x0811f8a6:  b8 4c eb 2b 08        mov     z, %eax
	0x0811f8ab:  57                    push    %edi
	0x0811f8ac:  56                    push    %esi
	0x0811f8ad:  53                    push    %ebx
	0x0811f8ae:  bb 13 00 00 00        mov     $0x13, %ebx
	0x0811f8b3:  89 18                 mov     %ebx, (%eax) <----------------.
	0x0811f8b5:  83 c0 fc              add     $0xfffffffc, %eax <sub $0x3>  |
	0x0811f8b8:  4b                    dec     %ebx                          |
	0x0811f8b9:  79 f8                 jns     0x0811f8b3 <init_rng + 0x13> -´
	0x0811f8bb:  83 c4 f4              add     $0xfffffff4, %esp <sub $0xb>
	0x0811f8be:  6a 14                 push    $0x14
	0x0811f8c0:  e8 2b d9 fb ff        call    0x080dd1f0 <counted_rng>
	0x0811f8c5:  83 c4 10              add     $0x10, %esp
	0x0811f8c8:  8d 58 09              lea     0x9(%eax), %ebx
	0x0811f8cb:  83 fb ff              cmp     $0xffffffff, %ebx
	0x0811f8ce:  74 56                 je      0x0811f926 <init_rng + 0x86> -------.
	0x0811f8d0:  83 c4 f4              add     $0xfffffff4, %esp <sub $0xb> <----. |
	0x0811f8d3:  6a 12                 push    $0x12                             | |
	0x0811f8d5:  e8 16 d9 fb ff        call    0x080dd1f0 <counted_rng>          | |
	0x0811f8da:  89 c6                 mov     %eax, %esi                        | |
	0x0811f8dc:  83 c4 10              add     $0x10, %esp                       | |
	0x0811f8df:  8d 7b ff              lea     -0x1(%ebx), %edi                  | |
	0x0811f8e2:  83 c4 f4              add     $0xfffffff4, %esp <sub $0xb> <-.  | |
	0x0811f8e5:  6a 12                 push    $0x12                          |  | |
	0x0811f8e7:  e8 04 d9 fb ff        call    0x080dd1f0 <counted_rng>       |  | |
	0x0811f8ec:  83 c4 10              add     $0x10, %esp                    |  | |
	0x0811f8ef:  39 c6                 cmp     %eax, %esi                     |  | |
	0x0811f8f1:  74 ef                 je      0x0811f8e2 <init_rng + 0x42> --´  | |
	0x0811f8f3:  8d 0c b5 00 00 00 00  lea     0x0(0, %esi, 4), %ecx             | |
	0x0811f8fa:  8b 99 00 eb 2b 08     mov     n(%ecx), %ebx                     | |
	0x0811f900:  83 fb 11              cmp     $0x11, %ebx                       | |
	0x0811f903:  74 1a                 je      0x0811f91f <init_rng + 0x7f> ---. | |
	0x0811f905:  c1 e0 02              shl     $0x2, %eax                      | | |
	0x0811f908:  8b 90 00 eb 2b 08     mov     n(%eax), %edx                   | | |
	0x0811f90e:  83 fa 11              cmp     $0x11, %edx                     | | |
	0x0811f911:  74 0c                 je      0x0811f91f <init_rng + 0x7f> -. | | |
	0x0811f913:  89 91 00 eb 2b 08     mov     %edx, n(%ecx)                 | | | |
	0x0811f919:  89 98 00 eb 2b 08     mov     %ebx, n(%eax)                 , | | |
	0x0811f91f:  89 fb                 mov     %edi, %ebx <-----------------<  | | |
	0x0811f921:  83 fb ff              cmp     $0xffffffff, %ebx             `-´ | |
	0x0811f924:  75 aa                 jne     0x0811f8d0 <init_rng + 0x30> -----´ |
	0x0811f926:  8d 65 e8              lea     -0x18(%ebp), %esp <-----------------´
	0x0811f929:  5b                    pop     %ebx
	0x0811f92a:  5e                    pop     %esi
	0x0811f92b:  5f                    pop     %edi
	0x0811f92c:  89 ec                 mov     %ebp, %esp
	0x0811f92e:  5d                    pop     %ebp
	0x0811f92f:  c3                    ret
	counted_rng:
	0x080dd1f0:  55                    push    %ebp
	0x080dd1f1:  89 e5                 mov     %esp, %ebp
	0x080dd1f3:  83 ec 08              sub     $0x8, %esp
	0x080dd1f6:  8b 45 08              mov     0x8(%ebp), %eax
	0x080dd1f9:  ff 05 60 4a 26 08     incl    c
	0x080dd1ff:  83 c4 f4              add     $0xfffffff4, %esp <sub $0xb>
	0x080dd202:  50                    push    %eax
	0x080dd203:  e8 88 89 04 00        call    0x08125b90 <uncounted_rng>
	0x080dd208:  89 ec                 mov     %ebp, %esp
	0x080dd20a:  5d                    pop     %ebp
	0x080dd20b:  c3                    ret
	uncounted_rng:
	0x08125b90:  55                    push    %ebp
	0x08125b91:  89 e5                 mov     %esp, %ebp
	0x08125b93:  83 ec 08              sub     $0x8, %esp
	0x08125b96:  8b 45 08              mov     0x8(%ebp), %eax
	0x08125b99:  83 c4 f4              add     $0xfffffff4, %esp <sub $0xb>
	0x08125b9c:  50                    push    %eax
	0x08125b9d:  e8 8e 05 00 00        call    0x08126130 <rng>
	0x08125ba2:  89 ec                 mov     %ebp, %esp
	0x08125ba4:  5d                    pop     %ebp
	0x08125ba5:  c3                    ret
	rng:
	0x08126130:  55                    push    %ebp
	0x08126131:  89 e5                 mov     %esp, %ebp
	0x08126133:  83 ec 1c              sub     $0x1c, %esp
	0x08126136:  31 c0                 xor     %eax, %eax
	0x08126138:  57                    push    %edi
	0x08126139:  56                    push    %esi
	0x0812613a:  53                    push    %ebx
	0x0812613b:  8b 7d 08              mov     0x8(%ebp), %edi
	0x0812613e:  a0 40 db 2a 08        mov     i, %al
	0x08126143:  8a 90 40 da 2a 08     mov     s(%eax), %dl
	0x08126149:  88 55 ff              mov     %dl, -0x1(%ebp)
	0x0812614c:  a0 41 db 2a 08        mov     j, %al
	0x08126151:  00 45 ff              add     %al, -0x1(%ebp)
	0x08126154:  8a 4d ff              mov     -0x1(%ebp), %cl
	0x08126157:  88 0d 41 db 2a 08     mov     %cl, j
	0x0812615d:  31 c0                 xor     %eax, %eax
	0x0812615f:  88 c8                 mov     %cl, %al
	0x08126161:  8a 80 40 da 2a 08     mov     s(%eax), %al
	0x08126167:  88 45 fe              mov     %al, -0x2(%ebp)
	0x0812616a:  31 c0                 xor     %eax, %eax
	0x0812616c:  a0 40 db 2a 08        mov     i, %al
	0x08126171:  8a 5d fe              mov     -0x2(%ebp), %bl
	0x08126174:  88 98 40 da 2a 08     mov     %bl, s(%eax)
	0x0812617a:  31 c0                 xor     %eax, %eax
	0x0812617c:  a0 41 db 2a 08        mov     j, %al
	0x08126181:  88 90 40 da 2a 08     mov     %dl, s(%eax)
	0x08126187:  00 da                 add     %bl, %dl
	0x08126189:  31 c0                 xor     %eax, %eax
	0x0812618b:  88 d0                 mov     %dl, %al
	0x0812618d:  0f b6 b0 40 da 2a 08  movzbl  s(%eax), %esi
	0x08126194:  a0 40 db 2a 08        mov     i, %al
	0x08126199:  88 45 fd              mov     %al, -0x3(%ebp)
	0x0812619c:  fe c0                 inc     %al
	0x0812619e:  a2 40 db 2a 08        mov     %al, i
	0x081261a3:  31 c0                 xor     %eax, %eax
	0x081261a5:  a0 40 db 2a 08        mov     i, %al
	0x081261aa:  8a 90 40 da 2a 08     mov     s(%eax), %dl
	0x081261b0:  00 d1                 add     %dl, %cl
	0x081261b2:  88 4d fc              mov     %cl, -0x4(%ebp)
	0x081261b5:  88 0d 41 db 2a 08     mov     %cl, j
	0x081261bb:  31 c0                 xor     %eax, %eax
	0x081261bd:  88 c8                 mov     %cl, %al
	0x081261bf:  8a 80 40 da 2a 08     mov     s(%eax), %al
	0x081261c5:  88 45 fb              mov     %al, -0x5(%ebp)
	0x081261c8:  31 c0                 xor     %eax, %eax
	0x081261ca:  a0 40 db 2a 08        mov     i, %al
	0x081261cf:  8a 4d fb              mov     -0x5(%ebp), %cl
	0x081261d2:  88 88 40 da 2a 08     mov     %cl, s(%eax)
	0x081261d8:  31 c0                 xor     %eax, %eax
	0x081261da:  a0 41 db 2a 08        mov     j, %al
	0x081261df:  88 90 40 da 2a 08     mov     %dl, s(%eax)
	0x081261e5:  00 ca                 add     %cl, %dl
	0x081261e7:  31 c0                 xor     %eax, %eax
	0x081261e9:  88 d0                 mov     %dl, %al
	0x081261eb:  31 db                 xor     %ebx, %ebx
	0x081261ed:  8a 98 40 da 2a 08     mov     s(%eax), %bl
	0x081261f3:  89 5d f0              mov     %ebx, -0x10(%ebp)
	0x081261f6:  8a 45 fd              mov     -0x3(%ebp), %al
	0x081261f9:  04 02                 add     $0x2, %al
	0x081261fb:  a2 40 db 2a 08        mov     %al, i
	0x08126200:  31 c0                 xor     %eax, %eax
	0x08126202:  a0 40 db 2a 08        mov     i, %al
	0x08126207:  8a 90 40 da 2a 08     mov     s(%eax), %dl
	0x0812620d:  8a 4d fc              mov     -0x4(%ebp), %cl
	0x08126210:  00 d1                 add     %dl, %cl
	0x08126212:  88 4d fa              mov     %cl, -0x6(%ebp)
	0x08126215:  88 0d 41 db 2a 08     mov     %cl, j
	0x0812621b:  31 c0                 xor     %eax, %eax
	0x0812621d:  88 c8                 mov     %cl, %al
	0x0812621f:  8a 80 40 da 2a 08     mov     s(%eax), %al
	0x08126225:  88 45 f9              mov     %al, -0x7(%ebp)
	0x08126228:  31 c0                 xor     %eax, %eax
	0x0812622a:  a0 40 db 2a 08        mov     i, %al
	0x0812622f:  8a 5d f9              mov     -0x7(%ebp), %bl
	0x08126232:  88 98 40 da 2a 08     mov     %bl, s(%eax)
	0x08126238:  31 c0                 xor     %eax, %eax
	0x0812623a:  a0 41 db 2a 08        mov     j, %al
	0x0812623f:  88 90 40 da 2a 08     mov     %dl, s(%eax)
	0x08126245:  00 da                 add     %bl, %dl
	0x08126247:  31 c0                 xor     %eax, %eax
	0x08126249:  88 d0                 mov     %dl, %al
	0x0812624b:  31 c9                 xor     %ecx, %ecx
	0x0812624d:  8a 88 40 da 2a 08     mov     s(%eax), %cl
	0x08126253:  8a 45 fd              mov     -0x3(%ebp), %al
	0x08126256:  04 03                 add     $0x3, %al
	0x08126258:  a2 40 db 2a 08        mov     %al, i
	0x0812625d:  31 c0                 xor     %eax, %eax
	0x0812625f:  a0 40 db 2a 08        mov     i, %al
	0x08126264:  8a 90 40 da 2a 08     mov     s(%eax), %dl
	0x0812626a:  8a 5d fa              mov     -0x6(%ebp), %bl
	0x0812626d:  00 d3                 add     %dl, %bl
	0x0812626f:  88 1d 41 db 2a 08     mov     %bl, j
	0x08126275:  31 c0                 xor     %eax, %eax
	0x08126277:  88 d8                 mov     %bl, %al
	0x08126279:  8a 80 40 da 2a 08     mov     s(%eax), %al
	0x0812627f:  88 45 f8              mov     %al, -0x8(%ebp)
	0x08126282:  31 c0                 xor     %eax, %eax
	0x08126284:  a0 40 db 2a 08        mov     i, %al
	0x08126289:  8a 5d f8              mov     -0x8(%ebp), %bl
	0x0812628c:  88 98 40 da 2a 08     mov     %bl, s(%eax)
	0x08126292:  31 c0                 xor     %eax, %eax
	0x08126294:  a0 41 db 2a 08        mov     j, %al
	0x08126299:  88 90 40 da 2a 08     mov     %dl, s(%eax)
	0x0812629f:  00 da                 add     %bl, %dl
	0x081262a1:  31 c0                 xor     %eax, %eax
	0x081262a3:  88 d0                 mov     %dl, %al
	0x081262a5:  8a 80 40 da 2a 08     mov     s(%eax), %al
	0x081262ab:  8a 55 fd              mov     -0x3(%ebp), %dl
	0x081262ae:  80 c2 04              add     $0x4, %dl
	0x081262b1:  88 15 40 db 2a 08     mov     %dl, i
	0x081262b7:  8b 55 f0              mov     -0x10(%ebp), %edx
	0x081262ba:  c1 e2 08              shl     $0x8, %edx
	0x081262bd:  01 f2                 add     %esi, %edx
	0x081262bf:  89 4d f4              mov     %ecx, -0xc(%ebp)
	0x081262c2:  c1 65 f4 10           shll    $0x10, -0xc(%ebp)
	0x081262c6:  03 55 f4              add     -0xc(%ebp), %edx
	0x081262c9:  c1 e0 18              shl     $0x18, %eax
	0x081262cc:  01 d0                 add     %edx, %eax
	0x081262ce:  85 ff                 test    %edi, %edi
	0x081262d0:  74 06                 je      0x081262d8 <rng + 0x1a8> -.
	0x081262d2:  31 d2                 xor     %edx, %edx                |
	0x081262d4:  f7 f7                 div     %edi                      |
	0x081262d6:  89 d0                 mov     %edx, %eax                |
	0x081262d8:  5b                    pop     %ebx <--------------------´
	0x081262d9:  5e                    pop     %esi
	0x081262da:  5f                    pop     %edi
	0x081262db:  89 ec                 mov     %ebp, %esp
	0x081262dd:  5d                    pop     %ebp
	0x081262de:  c3                    ret

The disassembly shows how to operate the replica ARC4.
It also shows that the order of operations is atypical as
 `i` is incremented after the rest of the operations,
 how bytes combined into an integer and
 how the return value is bound.
Interestingly
 the worst-case complexity of the algorithm is O(infinity) and
 it may return anything if the given upper bound is equal to the lower bound.

Most of the relevant dumps can be found in the project's resources.
