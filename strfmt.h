#ifndef __INCL_STRFMT__
#define __INCL_STRFMT__
#include <stdint.h>
#include <stdio.h>

void str_3digit(int16_t val, char *outstr);
void str_3digitL(int16_t val, char *outstr);
void str_4digit(int16_t val, char *outstr);
void str_4digitL(int16_t val, char *outstr);
void str_5digit(int16_t val, char *outstr);

typedef void (*__strfmt)(int16_t, char*);
#endif
