/**
@see put.c
@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef PUT_H
#define PUT_H

#include <stddef.h>//size_t

size_t put_fread(const char * path);
size_t put_fwrite(const char * path);

#endif
