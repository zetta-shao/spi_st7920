
#include "swspi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

//void swspi_initial(swspi_t *d);
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

void swspi_hal_delay_us(uint32_t us) { usleep(us); }
void swspi_hal_delay_ms(uint32_t ms) { usleep(ms*1000); }

int file_write(const char *filename, const void *data, const int dwByteSize) {
	int res = -1, _d = open(filename, O_WRONLY | O_NONBLOCK);
	if( _d < 0) { printf("%s open fail\n", filename); res = 0; }
	else res = write(_d, data, dwByteSize);
	close(_d);
	return res;
}

int gpio_init(const int gpio_num) {
    char gpio_str[4];
    sprintf(gpio_str,"%d",gpio_num);
    return file_write(SYSFS_GPIO_PATH SYSFS_GPIO_EXPORT_FN,gpio_str,4);
}

int gpio_deinit(int gpio_num) {
    char gpio_str[4];
    sprintf(gpio_str,"%d",gpio_num);
    return file_write(SYSFS_GPIO_PATH SYSFS_GPIO_UNEXPORT_FN,gpio_str,4);
}

int gpio_set_direction(int gpio_num,const char* dir) {
    char path_str[40];
    sprintf(path_str,"%s/gpio%d%s",SYSFS_GPIO_PATH,gpio_num,SYSFS_GPIO_DIRECTION);
    return file_write(path_str,dir,40);
}

int gpio_set_value(int gpio_num, const char *value) {
    char path_str[40];
    sprintf(path_str,"%s/gpio%d%s",SYSFS_GPIO_PATH,gpio_num,SYSFS_GPIO_VALUE);
    return file_write(path_str,value,40);
}

void swspi_hal_gpio_out(spi_gpio_t *d, uint8_t val) {
	(void)d; gpio_set_direction(d->pin,"output");
}

uint8_t swspi_hal_gpio_in(spi_gpio_t *d) {
	(void)d; gpio_set_direction(d->pin,"input"); return 0;
}

void swspi_hal_gpio_mode(spi_gpio_t *d, uint8_t val) { (void)d; (void)val; }

int swspi_hal_transmit(void *hWND, uint8_t *pWrite, uint16_t datasize) {
    int res = -1;
    struct spi_ioc_transfer xfer[1] = { 0 };
    xfer->tx_buf = (__u64)pWrite;
    xfer->rx_buf = (__u64)0;
    xfer->len = (int32_t)datasize;
    res = ioctl(*(int32_t*)hWND, SPI_IOC_MESSAGE(1), xfer);
    if(res < 0) { printf("spi write err %d\n", res); }
    return res;
}

int swspi_hal_receive(void *hWND, uint8_t *pRead, uint16_t datasize) {
    int res = -1;
    struct spi_ioc_transfer xfer[1] = { 0 };
    xfer->tx_buf = (__u64)0;
    xfer->rx_buf = (__u64)pRead;
    xfer->len = (int32_t)datasize;
    res = ioctl(*(int32_t*)hWND, SPI_IOC_MESSAGE(1), xfer);
    if(res < 0) { printf("spi read err %d\n", res); }
    return res;
}

int swspi_hal_transmit_receive(void *hWND, uint8_t *pRead, uint8_t *pWrite, uint16_t datasize) {
    int res = -1;
    struct spi_ioc_transfer xfer[1] = { 0 };
    xfer->tx_buf = (__u64)pWrite;
    xfer->rx_buf = (__u64)pRead;
    xfer->len = (int32_t)datasize;
    res = ioctl(*(int32_t*)hWND, SPI_IOC_MESSAGE(1), xfer);
    if(res < 0) { printf("spi read/write err %d\n", res); }
    return res;
}

void swspi_hal_init(swspi_t *d, spi_gpio_t *clk, spi_gpio_t *mosi, spi_gpio_t *miso) {
	(void)d; (void)clk; (void)mosi; (void)miso;
    char *devname = (char*)clk;
    d->CLK.pin = open(devname, O_RDWR);
    d->CLK.port = (void*)&(d->CLK.pin);
    if(d->CLK.pin < 0) { printf("spi init error %ld\n", d->CLK.pin); exit(0); }
    ioctl(d->CLK.pin, SPI_IOC_WR_MAX_SPEED_HZ, 125000);
}

void swspi_hal_setmode(swspi_t *d, uint8_t val) {
    val &= 3;
    ioctl(*(int32_t*)d->CLK.port, SPI_IOC_WR_MODE, &val);
}

void swspi_hal_setspeed(swspi_t *d, uint32_t val) {
    ioctl(*(int32_t*)d->CLK.port, SPI_IOC_WR_MAX_SPEED_HZ , &val);
}

void swspi_hal_spiclose(swspi_t *d) {
    close(*(int32_t*)d->CLK.port); d->CLK.pin = 65535; d->CLK.port = NULL;
}