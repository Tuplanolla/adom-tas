/**
@see put.c
@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef PUT_H
#define PUT_H

#include <stddef.h>//size_t

#include "record.h"//record_t

size_t freadp(const char * path);
size_t fwritep(const char * path);

#endif
