#ifndef __INCL_GPIODEF__
#define __INCL_GPIODEF__
#include <stddef.h>
#include <stdint.h>
// #define USE_HAL_DRIVER 1
#define HAL_MAX_DELAY      0xFFFFFFFFU

typedef struct {
	void		*port;
	uint32_t	pin;
} dummy_gpio_t;

//typedef dummy_gpio_t spi_gpio_t;

#endif
