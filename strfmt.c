#include "strfmt.h"
#include "lcd_fonts.h"

void str_3digit(int16_t val, char *outstr) {
#if defined (ICO_NUMBER_DOT) && defined(ICO_DOT_NUMBER)
    if(val < 1000) {
        sprintf(outstr, " %02d", val%100);
        outstr[0] = ((val/100)%10) + ICO_DOT_NUMBER;
    } else if(val < 10000) {
        sprintf(outstr, " %02d", (val%1000)/10);
        outstr[0] = ((val/1000)%10) + ICO_NUMBER_DOT;
    } else if(val < 65535) {
        sprintf(outstr, "%01d %01d", (val/10000)%10, (val%1000)/100);
        outstr[1] = ((val/1000)%10) + ICO_NUMBER_DOT;
    }
    outstr[3] = 0;
#else
    (void)val; (void)outstr;
#endif
}

void str_3digitL(int16_t val, char *outstr) { //3+1
	val = (val > 0) ? val : -val;
    if(val < 1000) {
        sprintf(outstr, ".%03d", val);
    } else if(val < 10000) {
        sprintf(outstr, "%01d.%02d", (val/1000)%10, (val%1000)/10);
    } else if(val < 65535) {
        sprintf(outstr, "%02d.%01d", (val/1000)%100, (val%1000)/100);
    }
    outstr[4] = 0;
}

void str_4digit(int16_t val, char *outstr) {
#ifdef ICO_NUMBER_DOT
    if(val < 10000) {
        sprintf(outstr, " %03d", val%1000);
        outstr[0] = ((val/1000)%10) + ICO_NUMBER_DOT;
    } else if(val < 65535) {
        sprintf(outstr, "%01d %02d", (val/10000)%10, (val%1000)/10);
        outstr[1] = ((val/1000)%10) + ICO_NUMBER_DOT;
    }
    outstr[4] = 0;
#else
    (void)val; (void)outstr;
#endif
}

void str_4digitL(int16_t val, char *outstr) { //4+1
    if(val < 10000) {
        sprintf(outstr, "%01d.%03d", (val/1000)%10, val%1000);
    } else if(val < 32767) {
        sprintf(outstr, "%02d.%02d", (val/1000)%100, (val%1000)/10);
    }
    outstr[5] = 0;
}


void str_5digit(int16_t val, char *outstr) {
#ifdef ICO_NUMBER_DOT
    if(val < 10000)
        sprintf(outstr, "  %03d", val%1000);
    else
        sprintf(outstr, "%01d %03d", (val/10000)%10, val%1000);

    outstr[1] = ((val/1000)%10) + ICO_NUMBER_DOT;
    outstr[5] = 0;
#else
    (void)val; (void)outstr;
#endif
}

typedef void (*__strfmt)(int16_t, char*);

