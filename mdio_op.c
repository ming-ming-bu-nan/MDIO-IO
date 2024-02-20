
#include "mdio_op.h"
#include "mdio_gpio.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pinmux_cfg.h>
#include <driver/pinctrl.h>
#include <driver/gpio.h>

#define MDIO_READ           2
#define MDIO_WRITE          1
#define MDIO_C45            (1<<15)
#define MDIO_C45_ADDR       (MDIO_C45 | 0)
#define MDIO_C45_READ       (MDIO_C45 | 3)
#define MDIO_C45_WRITE      (MDIO_C45 | 1)
#define MDIO_DELAY          250
#define MDIO_READ_DELAY     350

void ndelay(uint32_t nsecs)
{
    volatile uint32_t count;
    count =  nsecs * 30 / 1000 + 1;
    while (count > 0)
        count--;
}

void udelay(uint32_t usecs)
{
    volatile uint32_t count;
    count =  usecs * 30 + 1;
    while (count > 0)
        count--;
}

void mdelay(uint32_t msecs)
{
    udelay(msecs * 1000);
}

/* MDIO must already be configured as output. */
static void mdio_send_bit(uint8_t mdc_pin,uint8_t mdio_pin, int32_t val)
{
    gpio_demo_set(mdio_pin, val);
    ndelay(MDIO_DELAY);
    gpio_demo_set(mdc_pin, 1);
    ndelay(MDIO_DELAY);
    gpio_demo_set(mdc_pin, 0);
}

/* MDIO must already be configured as input. */
static int32_t mdio_get_bit(uint8_t mdc_pin,uint8_t mdio_pin)
{
    ndelay(MDIO_DELAY);
    gpio_demo_set(mdc_pin, 1);
    ndelay(MDIO_READ_DELAY);
    gpio_demo_set(mdc_pin, 0);

    return gpio_demo_get(mdio_pin);
}

/* MDIO must already be configured as output. */
static void mdio_send_num(uint8_t mdc_pin,uint8_t mdio_pin, uint32_t val, int32_t bits)
{
    int32_t i;

    for (i = bits - 1; i >= 0; i--)
        mdio_send_bit(mdc_pin,mdio_pin, (val >> i) & 1);
}

/* MDIO must already be configured as input. */
static uint32_t mdio_get_num(uint8_t mdc_pin,uint8_t mdio_pin, int32_t bits)
{
    int32_t i;
    uint32_t ret = 0;

    for (i = bits - 1; i >= 0; i--) {
        ret <<= 1;
        ret |= mdio_get_bit(mdc_pin,mdio_pin);
    }

    return ret;
}

static void mdio_cmd(uint8_t mdc_pin,uint8_t mdio_pin, int32_t op, uint8_t phy, uint8_t reg)
{
    int32_t i;

    gpio_demo_dir(mdio_pin, GPIO_OUT);

    /* pre code */
    for (i = 0; i < 32; i++)
        mdio_send_bit(mdc_pin,mdio_pin, 1);

    /* send the start bit (00) */
    mdio_send_bit(mdc_pin,mdio_pin, 0);
    if (op & MDIO_C45)
        mdio_send_bit(mdc_pin,mdio_pin, 0);
    else
        mdio_send_bit(mdc_pin,mdio_pin, 1);

    /* op code addr:00  write:01  read:11  post_read:10 */
    mdio_send_bit(mdc_pin,mdio_pin, (op >> 1) & 1);
    mdio_send_bit(mdc_pin,mdio_pin, (op >> 0) & 1);

    /* PHY addr  ane device type  all is 5bits */
    mdio_send_num(mdc_pin,mdio_pin, phy, 5);
    mdio_send_num(mdc_pin,mdio_pin, reg, 5);
}

static int32_t mdio_cmd_addr(uint8_t mdc_pin,uint8_t mdio_pin,int32_t phy, int32_t reg)
{
    uint32_t dev_addr = (reg >> 16) & 0x1F;
    uint32_t sreg = reg & 0xFFFF;

    mdio_cmd(mdc_pin,mdio_pin, MDIO_C45_ADDR, phy, dev_addr);

    /* send the turnaround (10) */
    mdio_send_bit(mdc_pin,mdio_pin, 1);
    mdio_send_bit(mdc_pin,mdio_pin, 0);

    /* data/reg */
    mdio_send_num(mdc_pin,mdio_pin, sreg, 16);

    /* set mdio dir ,get mdio value */
    gpio_demo_dir(mdio_pin, GPIO_IN);
    mdio_get_bit(mdc_pin,mdio_pin);

    return dev_addr;
}

int32_t mdio_read(uint8_t mdc_pin,uint8_t mdio_pin,int32_t phy, int32_t reg)
{
    int32_t ret, i;

    reg = mdio_cmd_addr(mdc_pin, mdio_pin, phy, reg);
    mdio_cmd(mdc_pin, mdio_pin, MDIO_C45_READ, phy, reg);

    /* set mdio dir */
    gpio_demo_dir(mdio_pin, GPIO_IN);

    if (mdio_get_bit(mdc_pin, mdio_pin) != 0) {
        for (i = 0; i < 32; i++)
            mdio_get_bit(mdc_pin, mdio_pin);

        return 0xffff;
    }

    ret = mdio_get_num(mdc_pin, mdio_pin, 16);

    mdio_get_bit(mdc_pin, mdio_pin);

    return ret;
}

int32_t mdio_write(uint8_t mdc_pin,uint8_t mdio_pin, int32_t phy, int32_t reg, uint32_t val)
{
    reg = mdio_cmd_addr(mdc_pin, mdio_pin, phy, reg);
    mdio_cmd(mdc_pin, mdio_pin, MDIO_C45_WRITE, phy, reg);

    /* send the turnaround (10) */
    mdio_send_bit(mdc_pin, mdio_pin, 1);
    mdio_send_bit(mdc_pin, mdio_pin, 0);

    mdio_send_num(mdc_pin, mdio_pin, val, 16);

    gpio_demo_dir(mdio_pin, GPIO_IN);
    mdio_get_bit(mdc_pin, mdio_pin);

    return 0;
}