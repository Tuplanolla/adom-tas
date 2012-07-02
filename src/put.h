/**
Provides input and output.
**/
#ifndef PUT_H
#define PUT_H

#include <stdio.h>//FILE

#include "record.h"//record_t

size_t freadp(const char * path);
size_t fwritep(const char * path);

#endif
