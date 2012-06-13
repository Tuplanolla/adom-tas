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

Run the launcher to generate a template configuration file:

	[user@arch adom-tas]$ bin/adom-tas

Edit the configuration file (more about it later):

	[user@arch adom-tas]$ nano adom-tas.cfg

Run the launcher normally:

	[user@arch adom-tas]$ bin/adom-tas

If you want to encode video files you'll need extra packages:

	[user@arch ~]$ pacman -S ttyrec ffmpeg

Recording is managed by scripts (the arguments are command to run and the output file):

	[user@arch ~]$ bin/ttyrec.sh bin/adom-tas output.tty
	[user@arch ~]$ bin/ffmpeg.sh "ttyplay output.tty" output.avi

Encoding can be done both unfiltered and filtered:

	[user@arch ~]$ bin/ffmpeg.sh bin/adom-tas output.avi
	[user@arch ~]$ bin/ffmpegf.sh bin/adom-tas output.avi

Installing the whole thing from the binaries will eventually be possible:

	[user@arch ~]$ wget adom-tas.tar.gz
	[user@arch ~]$ tar -x -f adom-tas.tar.gz

Configuration
-------------

Edit `adom-tas.cfg` for maximum efficiency, but don't touch the optional fields yet.
Documentation on how will be written later.

Schedule
--------

This project is in active development and expected to be finished by 2012-07-01.

Motivation
----------

What is ADoM? What are tool-assisted speedruns?

Conventions
-----------

The naming conventions used in the project follow those of the implementations of the functions.

The syntatic conventions in the other hand follow the simplest possible consistent set of rules that allows condensed code.
Keywords are always followed by spaces, pointers are always separated from the type and the identifier, comments and casts are removable using the simplest possible algorithm to leave no traces, etc.

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
		[ ] Disable the actual quit function
		[X] Find the random number generator's seed function
		[X] Redirect the seed function to the recorder and back again
		[X] Find the random number generator's reseed function
		[X] Simulate the reseed function
		[X] Implement save state selector
		[X] Implement fast save (memory-memory)
		[X] Implement fast load (memory-memory)
		[ ] Implement slow save (memory-file)
		[ ] Implement slow load (file-memory)
		[X] Make processes independent
		[ ] Refine sloppy implementations
	[X] Implement recording tools
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
	[ ] Document
		[X] Documentation
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

and contains the last recorded inputs (Alt Ctrl F), the amount of the last recorded frames (2) and the amount of all frames (21), the amount of the last elapsed turns (0) and the amount of all turns (7), the time elapsed since the last frame (15 seconds), the current hash of the random number generator's state and the currently selected save state (#2) and the amount of all save states (9).

The currently defined keys are F8 to play a recording (only on frame 0), F9 to save, F10 to load, F11 to change the system time, F12 to simulate actual save and load, Ctrl F12 to change the currently selected save state and _ to save a recording (and other unnecessary details).
