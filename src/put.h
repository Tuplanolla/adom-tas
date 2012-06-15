/**
Provides input and output.
**/
#ifndef PUT_H
#define PUT_H

#include "record.h"

size_t freadp(record_t * record, const char * path);
size_t fwritep(record_t * record, const char * path);

#endif
