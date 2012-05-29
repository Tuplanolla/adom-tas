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
