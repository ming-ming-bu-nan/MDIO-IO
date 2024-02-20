#ifndef __MDIO_OP_CMD_H_
#define __MDIO_OP_CMD_H_

#include <stdio.h>

extern int32_t mdio_read(uint8_t mdc_pin,uint8_t mdio_pin,int32_t phy, int32_t reg);

extern int32_t mdio_write(uint8_t mdc_pin,uint8_t mdio_pin, int32_t phy, int32_t reg, uint32_t val);

extern void ndelay(uint32_t nsecs);

extern void udelay(uint32_t usecs);

extern void mdelay(uint32_t msecs);

#endif