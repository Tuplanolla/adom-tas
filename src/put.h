/**
@see put.c
@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef PUT_H
#define PUT_H

#include <stddef.h>//size_t

#include "rec.h"//record_d

size_t freadp(const char * path);
size_t fwritep(const char * path);

#endif
