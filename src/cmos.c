#include "cmos.h"
#include "common.h"
#include "console.h"

#define PORT_SELECT 0x70
#define PORT_IO     0x71

uint8_t _second, _minute, _hour, _day, _month, _year, _century;
uint8_t _statA, _statB;

bool cmos_getUpdateInProgressFlag(void){
    outb(PORT_SELECT, 0xA);
    return inb(PORT_IO) & 0x80;
}

uint8_t cmos_getRegister(uint8_t reg){
    outb(PORT_SELECT, reg);// | 0x80); // uncomment to disable NMIs
    return inb(PORT_IO);
}

void cmos_readTimeFromRTC(void){
    while(cmos_getUpdateInProgressFlag());
    _second  = cmos_getRegister(CMOS_REG_SEC);
    _minute  = cmos_getRegister(CMOS_REG_MIN);
    _hour    = cmos_getRegister(CMOS_REG_HOUR);
    _day     = cmos_getRegister(CMOS_REG_DOM);
    _month   = cmos_getRegister(CMOS_REG_MON);
    _year    = cmos_getRegister(CMOS_REG_YEAR);

    // century register may not exist here; need to check. TODO
    _century = cmos_getRegister(CMOS_REG_CTRY);

    _statB  = cmos_getRegister(CMOS_REG_STB);
    if(!(_statB & 0x04)){
	// RTC is using BCD values.... convert them to be sane
	_second  = byteBCDToSanity(_second);
	_minute  = byteBCDToSanity(_minute);
	_hour    = byteBCDToSanity(_hour);
	_day     = byteBCDToSanity(_day);
	_month   = byteBCDToSanity(_month);
	_year    = byteBCDToSanity(_year);
	_century = byteBCDToSanity(_century);
    }
}

void cmos_printTime(void){
    console_printNum(_century, 10);
    console_printNum(_year, 10);
    console_putChar('/');
    console_printNum(_month, 10);
    console_putChar('/');
    console_printNum(_day, 10);
    console_putChar(' ');
    
    console_printNum(_hour, 10);
    console_print(":");
    console_printNum(_minute, 10);
    console_print(":");
    console_printNum(_second, 10);
    console_print("  ");
}
