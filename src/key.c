/**
Emulates
 the key code interpreter
  of the executable.

@file key.c
@author Sampsa "Tuplanolla" Kiiskinen
**/
#include "key.h"

/*
Returns the key code of a key number.

The code is generated automatically:
<pre>
gcc src/meta.c -O3 -o obj/meta
mkdir -p src/meta
obj/meta key_code > src/meta/key_code.c
rm -f obj/meta
</pre>

@param code The key number.
@return The key code.
*/
#include "meta/key_code.c"
