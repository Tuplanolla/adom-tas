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
	eflags  0000000246   [ PF ZF IF ]
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
