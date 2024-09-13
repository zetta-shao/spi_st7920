#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include "gpiodef.h"
#include "st7920.h"
#include "lcd_fonts.h"

#define IO_DATA_CMD		(2)
#define IO_RESET		(3)
#define SPI_DEVICE          	"/dev/spidev0.0"

#define SYSFS_GPIO_PATH             "/sys/class/gpio"
#define SYSFS_GPIO_EXPORT_FN        "/export"
#define SYSFS_GPIO_UNEXPORT_FN      "/unexport"
#define SYSFS_GPIO_VALUE            "/value"
#define SYSFS_GPIO_DIRECTION        "/direction"

#define DIR_OUT                     "out"

#define VALUE_HIGH                  "1"
#define VALUE_LOW                   "0"

#define MODE_CMD		VALUE_LOW
#define MODE_DATA		VALUE_HIGH

int main(int argc, char** argv) {
    swspi_t sspi1 = { 0 };
    st7920_t d = { 0 };
    swspi_HWinit(&sspi1, SPI_DEVICE);
    swspi_setmode(&sspi1, 3);

	st7920_init(&d, &sspi1, NULL, &Font_8x16);
    fontdraw_setColor(&d.d, 1);
    if(0) {
        st7920_string(&d, 0, 0, "st7910 test");
        st7920_string(&d, 0, 1, "st7921 test");
        st7920_string(&d, 0, 2, "st7932 test");
        st7920_string(&d, 0, 3, "st7943 test");
    }
    if(0) {
        char str[20] = { 0 }, *p;
        int x, y, z;
        fontdraw_setpos(&d.d, 0, 0);
        for(y=0, x=32; y<64; y+=16){
            for(z=x+16,p=str; x<z; x++,p++) *p=(char)x;
            *p=0; 
            //st7920_cursor(&d, 0, y);
            fontdraw_setpos(&d.d, 0, y);
            //st7920_strin2(&d, str, &Font_8x16, 1);
            fontdraw_stringC(&d.d, str, 1);
        }
        st7920_update(&d);
        //sleep(1);
        //for(y=48, x=95; y>=0; y-=16){
        //    for(z=x-15,p=str; x>=z; x--,p++) *p=(char)x;
        //    *p=0; 
        //    st7920_cursor(&d, 0, y);
        //    st7920_strin2(&d, str, &Font_8x16, 1);
        //}
        //st7920_update(&d);
    }
    if(0) {
        int x,y,z;
        st7920_cursor(&d, 0, 0);
        for(y=0; y<16; y++) {
                fontdraw_1bit(&d.d, y, y, 1);
                fontdraw_1bit(&d.d, 16-y, y, 1);
                fontdraw_1bit(&d.d, y, y+16, 1);
                fontdraw_1bit(&d.d, 16-y, y+16, 1);
                fontdraw_1bit(&d.d, y, y+32, 1);
                fontdraw_1bit(&d.d, 16-y, y+32, 1);
                //fontdraw_1bit(&d.d, x, y+48, 1);
                //fontdraw_1bit(&d.d, 16-x, y+48, 1);
        }
        st7920_update(&d);
    }
    if(1) {
        int8_t np;
        char str[8];
        fontdraw_setpos(&d.d, 16, 0);
        fontdraw_stringFont(&d.d, "Volt", 1, &Font_6x8);
        fontdraw_setpos(&d.d, 56, 0);
        fontdraw_stringFont(&d.d, "Cunt", 1, &Font_6x8);
        fontdraw_setpos(&d.d, 96, 0);
        fontdraw_stringFont(&d.d, "Watt", 1, &Font_6x8);
        fontdraw_setpos(&d.d, 0, 8);
        fontdraw_string(&d.d, "1 ");

        np = str_3digitL(156, str);
        fontdraw_setpos(&d.d, 16, 8);
        fontdraw_stringC(&d.d, str, np);
        fontdraw_setpos(&d.d, 48, 16);
        fontdraw_charFont(&d.d, 'v', np, &Font_6x8);

        np = str_3digitL(-2330, str);
        fontdraw_setpos(&d.d, 56, 8);
        fontdraw_stringC(&d.d, str, np);
        fontdraw_setpos(&d.d, 88, 16);
        fontdraw_charFont(&d.d, 'a', np, &Font_6x8);

        np = str_3digitL(1527, str);
        fontdraw_setpos(&d.d, 96, 8);
        fontdraw_stringC(&d.d, str, np);
        //fontdraw_charFont(&d.d, 'w', np, &Font_6x8);

        st7920_update(&d);
    }
    swspi_spifree(&sspi1);
    return 0;
}
