ADoM TAS (adom-tas)
===================

Tools for speedrunning ADoM (Ancient Domains of Mystery).

Meta
----

This readme is incomplete.

Installation
------------

ADoM TAS is only for Linux since it relies on injecting assembly instructions to another Linux executable.

The following examples are for installing it from source on (a freshly installed) Arch Linux.

Install the tools to build things with:

	[user@arch ~]$ pacman -S gcc make

Install the tools to build the documentation with if you feel like it:

	[user@arch ~]$ pacman -S doxygen

Install the dependencies (not libcurses or libconfig++) using your package manager of choice:

	[user@arch ~]$ pacman -S libc libncurses libconfig

Install the tools to clone the repository:

	[user@arch ~]$ pacman -S ssh git

Clone the repository:

	[user@arch ~]$ git clone git@github.com:Tuplanolla/adom-tas.git

Build the files:

	[user@arch ~]$ cd adom-tas
	[user@arch adom-tas]$ make

Build the documentation if you feel like it:

	[user@arch adom-tas]$ doxygen

Run the launcher to generate a template configuration file in the current working directory:

	[user@arch adom-tas]$ bin/adom-tas

Edit the configuration file (more about it later):

	[user@arch adom-tas]$ nano adom-tas.cfg

Run the launcher normally:

	[user@arch adom-tas]$ bin/adom-tas

If you want to encode video files you'll need extra packages:

	[user@arch ~]$ pacman -S ttyrec ffmpeg

Recording is managed by scripts (the arguments are command to run and the output file):

	[user@arch adom-tas]$ bin/ttyrec.sh -e bin/adom-tas -o output.tty
	[user@arch adom-tas]$ bin/ffmpeg.sh -e bin/adom-tas -o output.avi

More options can be found by using the help switch:

	[user@arch adom-tas]$ bin/ffmpeg.sh -h
	[user@arch adom-tas]$ bin/ffmpeg.sh -e "ttyplay output.tty" -r 32 -m 16 -s sd -o output.avi

It is currently recommended to first record a `tty` file and then convert it to an `avi` file since `adom-tas` is only synchronized with `nanosleep` and frame times add to processing time.

Installing the whole thing from the binaries will eventually be possible:

	[user@arch ~]$ wget adom-tas.tar.gz
	[user@arch ~]$ tar -x -f adom-tas.tar.gz

A copy of ADoM is included.
The package is unaltered as required by its license.

Configuration
-------------

ADoM TAS uses a single configuration file.
When it is run it checks for the configuration file `adom-tas.cfg`.
If the name is displeasing it can be changed by
 modifying the variable `default_config_path` in the file `default.h` and
 rebuilding ADoM TAS.
The file will be only checked in the current working directory.
If the file is not found it will be created from a template and ADoM TAS will terminate.

The extension is `*.cfg` since
 libconfig uses it by default and
 `*.conf` was originally planned to be reserved for a configuration language.

The configuration file contains

* the location of the ADoM `executable`,
* the location of ADoM's `data` directory,
* the location of ADoM TAS's `loader`,
* the location of the C standard library `libc`,
* the location of the New Cursor Optimization library `libncurses` and
* the amount of character `generations` (`100` by default),
* the amount of save `states` excluding the currently active state (at least `1` and `9` by default),
* the height of the terminal in `rows` (at least `25`, at most `127` and `25` by default),
* the width of the terminal `cols` in columns (at least `77`, at most `127` and `80` by default),
* the string `iterator` to replace with the save state number when processing output file names (`#` by default),
* the location of the `input` file for playback (`input.tas` by default),
* the location of the `output` files for recording (recommended to be `output.#.tas` and `output.tas` by default),
* the reserved location for the shared memory segment `shm` (`adom-tas.shm` by default),
* the location of the `error` log (`stderr` by default),
* the location of the `warning` log (`stderr` by default),
* the location of the `note` log (`stderr` by default) and
* the location of the `call` log (`null` by default).

The standard streams are `stdin`, `stdout`, `stderr` and `null`.
The configuration should be correct initially and errors will be reported otherwise.
Assuming you run ADoM TAS from the same directory you maked (sic) it in the only values that may be wrong are the library paths.
If you have no idea what is going on use the following command to find them.

	[user@arch ~]$ find /lib /usr/lib -maxdepth 1 \( -name libc.so.\* -o -name libncurses.so.\* \) 2>/dev/null

Make sure all the files exist.
Both absolute and relative paths work.
Symbolic links are also resolved.
The shell variable `~` is recognized too, but only expanded once per value.

The input file is read when the playback key or signal is caught on the first frame.
The output file is written when the corresponding save state is used.
The location of the shared memory segment
 may or may not exist and
 may contain whatever
  as it's only used for identification.

Take note that you need all kinds of permissions to be able to use library preloading and other wack stuff.

File Format
-----------

Recordings are saved to `*.tas` files in a custom format. The files consist of

* a four-byte `char *` header `54 41 53 00` and
* five-byte `frame_t` chunks consisting of
	* a one-byte `unsigned char` duration and
	* a four-byte `time_t` excerpt.

Schedule
--------

This project is in active development and expected to be finished by 2012-07-01.

Motivation
----------

What is ADoM?
ADoM is a bad game that's fun to break.
What are tool-assisted speedruns?
Completing video games without actually having to play them.

History
-------

All things ye olde and boring.

Philosophy
----------

A reference to the Arch Way.

Conventions
-----------

The naming conventions used in the project follow those of the implementations of the functions.

The syntatic conventions in the other hand follow the simplest possible consistent set of rules that allows condensed code.
Keywords are always followed by spaces, pointers are always separated from the type and the identifier, comments and casts are removable using the simplest possible algorithm to leave no traces, etc.

Making of
---------

Let's decipher the random number generator with GDB.
Let's fix this mess later.

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

It is called once as well.
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

	(gdb) set $s = (unsigned char *)0x082ada40
	(gdb) set $i = (unsigned char *)0x082adb40
	(gdb) set $j = (unsigned char *)0x082adb41
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

`eax` contains an epoch time for 2012-06-01 17:46:40 UTC.

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

	(gdb) set $f = (void (*)())0x081504e7
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
Later states can be calculated with brute force once the first few states can be simulated reliably.
More about that and more about fixing what was just written later.

Note that it turned out to not be a proper ARC4; the order of operations was atypical.

Notes
-----

A character is considered generated when the text "You are now a fully learned wizard." appears.

Troubleshooting
---------------

Parsing the configuration file failed with `CONFIG_PARSE_PROBLEM`? Consider

* making sure the syntax is correct with
	* an equals sign `=` between keys and values and
	* strings enclosed in quotation marks `"`,
* updating libconfig,
* taking care of legacy problems by
	* adding semicolons `;` to the end of each line and
	* ensuring the last line ends with a line break `\n`,
* removing uncommon whitespace characters like no-break spaces `\xa0`,
* generating a new configuration file or
* asking for help.

Checklist
---------

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
	[ ] Implement emulation tools
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
		[ ] Make processes actually work
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
		[X] Add a status bar (char[80] line)
		[X] Include in the status bar
			[X] Last input
			[X] Frame count
			[X] Turn count
			[X] Save state
			[X] Time manipulation
			[X] Random number generator state
		[ ] Implement cheats that disable the recording
		[X] Refine sloppy implementations
	[ ] Refactor everything
	[ ] Document
		[X] Documentation
		[ ] Project
		[ ] Executable
		[ ] Emulation tools
		[ ] Recording tools
		[ ] Analysis tools

Progression
-----------

The typical usage of ADoM TAS creates a process tree:

	Start playing:
	----[ ]
	Save the first state:
	----[1]
	Keep playing:
	----(1)
	     `----[ ]
	Save the second state:
	----(1)
	     `----[2]
	Keep playing:
	----(1)
	     `----(2)
	           `----[ ]
	Save the first state again:
	----.
	     `----(2)
	           `----[1]
	Keep playing:
	----.
	     `----(2)
	           `----(1)
	                 `----[ ]
	Load the first state:
	----.
	     `----(2)
	           `----[1]
	Keep playing:
	----.
	     `----(2)    .----[ ]
	           `----(1)
	Load the second state:
	----.
	     `----[2]
	           `----(1)
	Keep playing:
	----.      .----[ ]
	     `----(2)
	           `----(1)
	Repeat:
	----.      .----(3)
	     `----(2)    `----(4)
	           |           `----.
	           `----(1)          `----(5)
	                 |----(6)
	                 |     `----.
	                 |           `----[ ]
	                 `----(7)
	                       `----(8)

User Interface
--------------

The user interface deserves a mention here since it's so intuitive.

The status bar will look like

<pre>
Coward                  I: \M\Cf  F: 2/21  T: 0/7  D: 15  R: 0xe87de001  S: 2/9
</pre>

and contains the last recorded inputs (Alt Ctrl F), the amount of the last recorded frames (2) and the amount of all frames (21), the amount of the last elapsed turns (0) and the amount of all turns (7 ignoring negative turns), the time elapsed since the last frame (15 seconds), the current hash of the random number generator's state and the currently selected save state (#2) and the amount of all save states (9).

The currently defined keys are F8 to play a recording (only on frame 0), F9 to save, F10 to load, F11 to change the system time, F12 to simulate actual save and load, Ctrl F12 to change the currently selected save state and _ to save a recording (and other unnecessary details).
