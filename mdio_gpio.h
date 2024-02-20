#ifndef __MDIO_GPIO_H_
#define __MDIO_GPIO_H_

#include <stdio.h>

extern void gpio_demo_init(uint32_t pin);

extern void gpio_demo_dir(uint32_t pin,uint32_t dir);

extern uint32_t gpio_demo_get(uint32_t pin);

extern void gpio_demo_set(uint32_t pin, uint32_t level);

#endif