ADoM TAS (adom-tas)
===================

Tools for speedrunning ADoM (Ancient Domains of Mystery).

Meta
----

This readme is incomplete.

Installation
------------

ADoM TAS is only for Linux since it relies on injecting assembly instructions to another Linux executable.
Installing it is simple (here with Arch Linux):

	[user@arch ~]$ pacman -S gcc libc libncurses libconfig make git
	[user@arch ~]$ git clone git@github.com:Tuplanolla/adom-tas.git
	[user@arch ~]$ cd adom-tas
	[user@arch adom-tas]$ make
	[user@arch adom-tas]$ nano adom-tas.cfg
	[user@arch adom-tas]$ bin/adom-tas

Configuration
-------------

Edit `adom-tas.cfg` for maximum efficiency.
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

The syntatic conventions in the other hand follow the simplest possible consistent set of rules that allows condensed code (keywords are always followed by spaces, pointers are always separated from the type and the identifier, comments and casts are removable using the simplest possible algorithm to leave no traces, etc).

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
		[ ] Find the random number generator's reseed function
		[ ] Simulate the reseed function
		[X] Implement save state selector
		[X] Implement fast save (memory-memory)
		[X] Implement fast load (memory-memory)
		[ ] Implement slow save (memory-file)
		[ ] Implement slow load (file-memory)
		[ ] Refine sloppy implementations
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
		[ ] Documentation
		[ ] Executable
		[ ] Emulation tools
		[ ] Recording tools
		[ ] Analysis tools

Progression
-----------

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

The user interface (out of date now) deserves a mention here since it's so intuitive.

The status bar looks like

<pre>
Coward    <span style="background-color: #d58080; color: #000000;">I: \M\Cf </span> <span style="background-color: #d5d580; color: #000000;">F: 21        </span> <span style="background-color: #80d580; color: #000000;">G: 7         </span> <span style="background-color: #8080d5; color: #000000;">R: 0xe87de001</span> <span style="background-color: #80d5d5; color: #000000;">T: 0x7fe81780</span> <span style="background-color: #d580d5; color: #000000;">S: 2</span>
</pre>

and contains the last input (Alt Ctrl F), the number of the last recorded frame (#21), the number of the last played turn (#7), the current hash of the random number generator's state, the current system time (2038-01-01 00:00:00) and the selected save state (#2).

The keys to save, load and control the statusbar are currently undefined.
