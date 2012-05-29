adom-tas
========

Tools for speedrunning ADoM (Ancient Domains of Mystery).

Checklist
---------

<pre>
[X] Install
	[X] Linux
	[X] GCC
[ ] Implement emulation tools
	[X] Find the random number generator's seed function
	[X] Redirect the seed function to the recorder and back again
	[ ] Implement save state selector
	[X] Implement fast save (memory-memory)
	[X] Implement fast load (memory-memory)
	[ ] Implement slow save (memory-file)
	[ ] Implement slow load (file-memory)
	[ ] Refine sloppy implementations
[ ] Implement recording tools
	[X] Find the input handler
	[X] Redirect the input handler to the recorder and back again
	[ ] Implement saving to a file
		[ ] Inputs (char** string)
		[ ] Time manipulations (uint32 seconds)
		[ ] Frame durations for playback (uint8 multiplier)
	[ ] Implement playing back a recording
	[X] Implement dumping an avi
	[ ] Refine sloppy implementations
[ ] Implement analysis tools
	[X] Find the turn count variable
	[X] Find a way to get around overloading macros
	[X] Create an overlay bigger than 80x25
	[X] Add a status bar (char* string)
	[ ] Include in the status bar
		[ ] Current save state
		[X] Last used inputs
		[X] Last used time manipulations
		[X] Inputs per turn
	[ ] Implement cheats that disable the recording
	[ ] Refine sloppy implementations
</pre>

Progression
-----------

<pre>
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
</pre>

User Interface
--------------

The user interface deserves a mention here since it's so intuitive.

The status bar looks like
<pre>
<span style="color: red;">I: \M\Cf  </span> <span style="color: yellow;">F: 32        </span> <span style="color: green;">G: 16        </span> <span style="color: cyan;">R: 0x08dc</span> <span style="color: blue;">T: 0x7fe86be0</span>
</pre>
and contains the last input I, the number of the last recorded frame F, the number of the last played turn G, the current index pair of the random number generator R and the current system time T.

The keys to save, load and control the statusbar are currently undefined.
