#include <stdlib.h>
#include <string.h>
#include <CLI_type.h>
#include <pinmux_cfg.h>
#include <driver/pinctrl.h>
#include <driver/taishan/pinctrl/taishan_pinctrl.h>
#include <driver/gpio.h>
#include "mdio_gpio.h"

extern gpio_dev_t g_gpio_safety;
extern gpio_dev_t g_gpio_ap;
#define GPIO_INSTANCE(pin)  \
    (((pin) < TAISHAN_SAFETY_PIN_NUM) ? &g_gpio_safety : &g_gpio_ap)

extern pinctrl_t g_pinctrl_safety;
extern pinctrl_t g_pinctrl_ap;
#define PINCTRL_INSTANCE(pin)  \
    (((pin) < TAISHAN_SAFETY_PIN_NUM) ? &g_pinctrl_safety : &g_pinctrl_ap)

void gpio_demo_init(uint32_t pin)
{
    ASSERT(pin < TAISHAN_PIN_NUM);

    pinctrl_t *pinctrl;

    pinctrl = PINCTRL_INSTANCE(pin);

    pinctrl_config(pinctrl, pin, PINCTRL_CONFIG_FUNCTION, PIN_MUX_ALT0);
    pinctrl_config(pinctrl, pin, PINCTRL_CONFIG_DRIVE, PIN_PUSH_PULL);
}

void gpio_demo_dir(uint32_t pin,uint32_t dir)
{
    ASSERT(pin < TAISHAN_PIN_NUM);

    gpio_dev_t *gpio_dev;

    gpio_dev = GPIO_INSTANCE(pin);

    gpio_config(gpio_dev, pin, dir);
}

void gpio_demo_set(uint32_t pin, uint32_t level)
{
    ASSERT(pin < TAISHAN_PIN_NUM);

    gpio_dev_t *gpio_dev;

    gpio_dev = GPIO_INSTANCE(pin);

    gpio_set(gpio_dev, pin, level);
}

uint32_t gpio_demo_get(uint32_t pin)
{
    gpio_dev_t *gpio_dev;

    ASSERT(pin < TAISHAN_PIN_NUM);

    gpio_dev = GPIO_INSTANCE(pin);

    return gpio_get(gpio_dev, pin);
}