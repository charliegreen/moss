#ifndef CMOS_H
#define CMOS_H

#include "common.h"

#define CMOS_REG_SEC  0x00
#define CMOS_REG_MIN  0x02
#define CMOS_REG_HOUR 0x04
#define CMOS_REG_WKDY 0x06
#define CMOS_REG_DOM  0x07
#define CMOS_REG_MON  0x08
#define CMOS_REG_YEAR 0X09
#define CMOS_REG_CTRY 0x32
#define CMOS_REG_STA  0x0A
#define CMOS_REG_STB  0x0B

bool cmos_getUpdateInProgressFlag(void);
uint8_t cmos_getRegister(uint8_t reg);
void cmos_readTimeFromRTC(void);
void cmos_printTime(void);

#endif
