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
	[ ] Implement fast save (memory-memory)
	[ ] Implement fast load (memory-memory)
	[ ] Implement slow save (memory-file)
	[ ] Implement slow load (file-memory)
[ ] Implement recording tools
	[ ] Find the input handler
	[ ] Redirect the input handler to the recorder and back again
	[ ] Implement saving to a file
		[ ] Inputs (char** string)
		[ ] Time manipulations (uint32 seconds)
		[ ] Frame durations for playback (uint8 multiplier)
	[ ] Implement playing back a recording
	[ ] Implement dumping an avi
[ ] Implement analysis tools
	[X] Find the turn count variable
	[ ] Create an overlay bigger than 80x25
	[ ] Add a status bar (char* string)
	[ ] Include in the status bar
		[ ] Current save state
		[ ] Last used inputs
		[ ] Last used time manipulations
		[ ] Inputs per turn
	[ ] Implement cheats that disable the recording
</pre>
