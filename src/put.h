/**
Provides input and output.
**/
#ifndef PUT_H
#define PUT_H

#include "record.h"

size_t freadp(FILE * stream, record_t * record,);
size_t fwritep(FILE * stream, record_t * record);

#endif
