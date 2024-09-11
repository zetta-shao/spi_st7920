#include "gpiodef.h"
#include "st7920.h"
#include <stdint.h>
#include <stdio.h>
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
	st7920_init(&d, &sspi1, NULL, NULL);
    if(0) {
        st7920_string(&d, 0, 0, "st7910 test");
        st7920_string(&d, 0, 1, "st7921 test");
        st7920_string(&d, 0, 2, "st7932 test");
        st7920_string(&d, 0, 3, "st7943 test");
    }
    if(1) {
        st7920_cursor(&d, 0, 0);
        st7920_strin2(&d, "!!!!!!!!", &Font_11x18, 1);
        st7920_update(&d);
    }
    swspi_spifree(&sspi1);
    return 0;
}
