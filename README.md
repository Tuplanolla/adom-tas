# ADoM TAS (adom-tas)

ADoM TAS stands for Ancient Domains of Mystery Tool-Assisted Speedruns.
It makes recording deterministic runs as input sequences possible and thus
allows creating theoretically perfect speedruns and other demonstrations.
Its possibilities are not restricted to speedruns despite being the main focus.
The name is not very descriptive since it's built around an acronym.

## Meta

This readme is incomplete.

## Schedule

This project is in active development and
the first version was expected to be finished by 2012-07-01.
The first working binaries were built 2012-07-02.
The rest should be done by 2012-08-01.

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
	[user@arch ~]$ tar -x -f adom-tas.tar.gz

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
temporary objects that can be removed after compilation in the `obj` directory.

## Running

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

	[user@arch ~]$ pacman -S ttyrec ffmpeg

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
 modifying the variable `default_config_path` in the file `def*.c` and
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
* the location of the `call` log (`/dev/null` by default)
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

	Drunk Coward  I: \M\Cf  F: 2/21  T: 0/7  D: 1/2  E: 15  R: 0xe87de001  S: 2/9  P: 16384

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

* `F5` saves the current save state,
* `F8` loads it,
* `F6` selects the next save state,
* `Shift F6` the previous one,
* `F2` increases (doubles) the duration of the next frame,
* `Shift F2` decreases (halves) it,
* `F3` shifts time forwards (by one second),
* `Shift F3` shifts it backwards (but not to a negative value),
* `F9` opens or closes the save state menu,
* `F10` condenses or expands the user interface,
* `Shift F10` hides and shows it,
* `F11` plays or pauses a recording (but only starting on the first frame),
* `Shift F11` stops a recording (and enables appending to it),
* `Shift F12` quits and
* the save key (typically `S`) emulates the save-quit-load process.

## Troubleshooting

Compilation failed with `No such file or directory`? Consider

* ensuring the installed libraries are
	* `libncurses` instead of `libcurses` and
	* `libconfig` instead of `libconfig++`,
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
* looking it up in the well-documented source code or
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

### Documentation

It's possible to generate automated documentation for ADoM TAS with Doxygen.

	[user@arch ~]$ pacman -S doxygen

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

Recordings are saved to `*.tas` files in a custom format. The files contain

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

### Making of ...

This section is supposed to be about insightful revelations about
 how things are done and
 some backstory of why this was made,
 but currently it's just a mess.

When writing a recorder the most important thing is entropy.
Since recordings are basically inputs sent to the game the game acts like an automata:
even the slightest difference can cause desynchronization.
It's not desirable, so all sources of entropy need to be understood and controlled (since they're external it's not cheating).

Let's decipher the random number generator with GDB.
The command names are no abbreviated for clarity.

	[user@arch ~]$ rm -f .adom.data/.adom.prc
	[user@arch ~]$ cd adom
	[user@arch adom]$ gdb adom

Let's check for typical random number generators.

	(gdb) break rand
	(gdb) break random
	(gdb) break drand48
	(gdb) ignore 1 0xff
	(gdb) ignore 2 0xff
	(gdb) ignore 3 0xff
	(gdb) run
	(gdb) info break 1
	(gdb) info break 2
	(gdb) info break 3

`random` is called once.
Let's look for its seed.

	(gdb) break srandom
	(gdb) ignore 4 0xff
	(gdb) run
	(gdb) info break 4

It's called once as well.
Time is a typical source of entropy.
Let's check for it.

	(gdb) break time
	(gdb) break gettimeofday
	(gdb) break clock
	(gdb) break localtime
	(gdb) ignore 5 0xff
	(gdb) ignore 6 0xff
	(gdb) ignore 7 0xff
	(gdb) ignore 8 0xff
	(gdb) run
	(gdb) info break 5
	(gdb) info break 6
	(gdb) info break 7
	(gdb) info break 8

`time` is called once too. 
The sequence is probably the typical `srandom(time(NULL))` and `random()`.
Let's check it out.

	(gdb) ignore 4 0
	(gdb) run
	(gdb) finish
	(gdb) x/12i $pc-13

	    0x8125d1c:  push  $0x0
	    0x8125d1e:  call  0x8049420 <time@plt>
	    0x8125d23:  push  %eax
	    0x8125d24:  call  0x80496d0 <srandom@plt>
	=>  0x8125d29:  add   $0x20,%esp
	    0x8125d2c:  add   $0xfffffff4,%esp
	    0x8125d2f:  call  0x8049380 <random@plt>
	    0x8125d34:  push  %eax
	    0x8125d35:  call  0x8125ea0
	    0x8125d3a:  mov   %ebp,%esp
	    0x8125d3c:  pop   %ebp
	    0x8125d3d:  ret

So it seems.
The rest is more complicated though.

	(gdb) continue

Running the executable at different times and comparing the memory reveal a 258 byte state.
However the two last bytes only change after the rest.
It is therefore likely an ARC4 stream cipher.
Assuming `i` comes before `j` the variables it uses can be mapped.

	(gdb) set $s = (unsigned char * )0x082ada40
	(gdb) set $i = (unsigned char * )0x082adb40
	(gdb) set $j = (unsigned char * )0x082adb41
	(gdb) set $l = (unsigned int )0x100

Entropy is a problem when trying to get reliable results, so let's get rid of it.

	(gdb) ignore 5 0
	(gdb) run

`time` is caught.
Let's look around.

	(gdb) x/6i $pc

	=>  0xb7e7eeb0 <time>:     xor   %eax,%eax
	    0xb7e7eeb2 <time+2>:   mov   0x4(%esp),%edx
	    0xb7e7eeb6 <time+6>:   mov   %eax,%ecx
	    0xb7e7eeb8 <time+8>:   xchg  %ecx,%ebx
	    0xb7e7eeba <time+10>:  mov   $0xd,%eax
	    0xb7e7eebf <time+15>:  call  *%gs:0x10
	    0xb7e7eec6 <time+22>:  xchg  %ecx,%ebx
	    0xb7e7eec8 <time+24>:  test  %edx,%edx

That call is the actual system call to get the time so let's step past it and change its return value.

	(gdb) nexti 6
	(gdb) info registers

	eax     0x4fc90000  +1338572800
	ecx     0xbffffa54  -1073743276
	edx     0x00000000   0
	ebx     0x00000000   0
	esp     0xbffff960  0xbffff960
	ebp     0xbffff988  0xbffff988
	esi     0x00000001  +1
	edi     0x00000000   0
	eip     0xb7e7eec6  0xb7e7eec6 <time+22>
	eflags  0000000246  [ PF ZF IF ]
	cs      0x00000073  +115
	ss      0x0000007b  +123
	ds      0x0000007b  +123
	es      0x0000007b  +123
	fs      0x00000000   0
	gs      0x00000033  +51

`eax` seems to contain an epoch time for 2012-06-01 17:46:40 UTC.

	(gdb) set $eax = 0

Back to 1970-01-01 00:00:00 UTC.
It be that time until we're done since it's only called once during the execution.
Let's add watches.

	(gdb) watch *$s
	(gdb) watch *$i
	(gdb) watch *$j
	(gdb) continue

`s` already changed from 0 to 103 in 0x08125f2c so we're late to the party.
Let's dump `s` to find out how late.

	(gdb) dump binary memory late_s $s $s+$l

The first byte of `s` is 0x67 and the rest are uninitialized (seemingly to `s[i] = i`) so not too late.

	(gdb) continue

Turns out `j` changes from 0 to 15 in 0x0812615d and

	(gdb) continue

`i` changes from 0 to 1 in 0x081261a3. This confirms `i` was indeed first.
Now we're late, but at least know where to set breakpoints next.

	(gdb) finish
	(gdb) x/3i $pc-6
	    0x8125b9c:  push  %eax
	    0x8125b9d:  call  0x8126130
	=>  0x8125ba2:  mov   %ebp,%esp

It appears 0x81504e7 calls 0x80d15f0,
0x80d1668 calls 0x80dd1f0,
0x80dd203 calls 0x8125b90 and
0x8125b9d calls 0x8126130 which causes all this to happen.
A breakpoint is set accordingly.

	(gdb) set $f = (void (*)() )0x081504e7
	(gdb) break *$f

Let's go.

	(gdb) run
	(gdb) nexti 6
	(gdb) set $eax = 0
	(gdb) continue

Now we can dump the initial state

	(gdb) dump binary memory first_s $s $s+$l

and the next two states since the index watches are still active.

	(gdb) continue
	(gdb) continue
	(gdb) dump binary memory second_s $s $s+$l
	(gdb) continue
	(gdb) continue
	(gdb) dump binary memory third_s $s $s+$l

Let's get out of here.

	(gdb) ignore 9 0xffff
	(gdb) ignore 10 0xffff
	(gdb) ignore 11 0xffff
	(gdb) continue
	(gdb) quit

The state files `first_s`, `second_s` and `third_s` can now be analyzed and help write a replica ARC4.
They can be found in the project resources for completeness' sake.
Later states can be calculated with brute force once the first few states can be simulated reliably.
More about that and more about fixing what was just written later.

Note that it turned out to not be a proper ARC4; the order of operations was atypical.

Why isn't real life this easy?

## Some Diagrams

### Checklist

	[X] Install
		[X] Linux
		[X] Packages
			[X] gcc
			[X] libc
			[X] libncurses
			[X] libconfig
			[X] make
			[X] git
			[X] ttyrec
			[X] ffmpeg
	[X] Implement emulation tools
		[X] Disable the actual save function
		[X] Find the random number generator's seed function
		[X] Redirect the seed function to the recorder and back again
		[X] Find the random number generator's reseed function
		[X] Simulate the reseed function
		[X] Implement save state selector
		[X] Implement fast save (memory-memory)
		[X] Implement fast load (memory-memory)
		[X] Implement slow save (memory-file)
		[X] Implement slow load (file-memory)
		[X] Make processes independent
		[X] Make processes actually work
		[X] Save temporary files
		[X] Refine sloppy implementations
	[ ] Implement recording tools
		[X] Find the input handler
		[X] Redirect the input handler to the recorder and back again
		[X] Implement saving to a file each frame
			[X] Input type (enum type_t)
			[X] Input code (int32_t key)
			[X] Time manipulation (uint32_t t)
			[X] Playback duration (uint8_t sixteenths)
		[X] Implement playing back a recording
		[X] Implement dumping a ttyrec
		[X] Implement dumping an avi
		[ ] Synchronize timing with the processor
		[X] Refine sloppy implementations
	[ ] Implement analysis tools
		[X] Find the turn count variable
		[X] Find a way to get around overloading macros
		[X] Create an overlay bigger than 80x25
		[X] Add a status bar (char * line)
		[X] Include in the status bar
			[X] Last input
			[X] Frame count
			[X] Turn count
			[X] Save state
			[X] Time manipulation
			[X] Random number generator state
		[X] Implement a journaled character roller
		[ ] Roll 4294967295 characters
		[ ] Implement cheats that disable the recording
		[X] Refine sloppy implementations
	[X] Refactor everything
	[ ] Refactor everything harder
	[ ] Document
		[ ] Code
		[X] Documentation
		[X] Project
		[ ] Executable
		[ ] Meta
