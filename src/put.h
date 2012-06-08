/**
Provides input and output.
**/
#ifndef PUT_H
#define PUT_H

extern FILE * input_stream;
extern FILE * output_stream;

int fwritel(FILE * stream, size_t size, const unsigned char * ptr);

#endif
