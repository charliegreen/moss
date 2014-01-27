#include "statusbar.h"
#include "common.h"
#include "console.h"

running_flag_t _runningFlag;
interrupt_flag_t _interruptFlag;
size_t _cursor;

void statusbar_initialize(void){
    uint16_t*addr = CONSOLE_BUFFER;
    console_printToLoc("[20", addr++, 1);
    for(size_t i=0; i<OS_NAME_MAX_LEN; i++)
	console_printToLoc(" ", addr+i, 1);
    
    console_printToLoc(OS_NAME, addr, OS_NAME_MAX_LEN);
    addr+=OS_NAME_MAX_LEN;
    console_printToLoc("70][", addr, 2);
    addr+=2;

    statusbar_setRunningFlag(RUNNING);
    statusbar_setInterruptFlag(NONE);
    addr+=STATUSBAR_NUM_FLAGS;

    console_printToLoc("70][", addr, 2);
    addr+=2;
    statusbar_clearMsg();
    addr+=STATUSBAR_MSG_MAX_LEN;
    console_printToLoc("]", addr, 1);

    _cursor = 0;
}

void statusbar_setRunningFlag(running_flag_t f){
    _runningFlag = f;
    vga_color_t oldColor = console_getColor();
    switch(_runningFlag){
    case RUNNING:
	console_printToLoc("30R", STATUSBAR_RUNNING_FLAG_LOC, 1);
	break;
    case BLOCKING:
	console_printToLoc("90B", STATUSBAR_RUNNING_FLAG_LOC, 1);
	break;
    case HALTED:
	console_printToLoc("40H", STATUSBAR_RUNNING_FLAG_LOC, 1);
	break;
    }
    console_setColor(oldColor);
}

void statusbar_setInterruptFlag(interrupt_flag_t f){
    _interruptFlag = f;
    vga_color_t oldColor = console_getColor();
    switch(_interruptFlag){
    case NONE:
	console_printToLoc(" ", STATUSBAR_INTERRUPT_FLAG_LOC, 1);
	break;
    case UNHANDLED:
	console_printToLoc("40U", STATUSBAR_INTERRUPT_FLAG_LOC, 1);
	break;
    case HANDLED:
	console_printToLoc("20H", STATUSBAR_INTERRUPT_FLAG_LOC, 1);
	break;
    }
    console_setColor(oldColor);
}

void statusbar_clearMsg(void){
    console_setColor(console_makeColor(COLOR_LIGHT_GREY, COLOR_BLACK));
    for(size_t i=0; i<STATUSBAR_MSG_MAX_LEN; i++)
	console_printToLoc(" ", STATUSBAR_MSG_BASE+i, 1);
    _cursor = 0;
}

void statusbar_printMsg(const char*data){
    size_t len = strlenesc(data);
    console_printToLoc(data,STATUSBAR_MSG_BASE+_cursor,
		       STATUSBAR_MSG_MAX_LEN-_cursor-1);
    if(_cursor+len >= STATUSBAR_MSG_MAX_LEN-1)
	_cursor = STATUSBAR_MSG_MAX_LEN-1;
    else
	_cursor += len;
}

void statusbar_printNum(uint32_t num, uint32_t radix){
    if(num==0){
	console_printToLoc("0", STATUSBAR_MSG_BASE+_cursor, 1);
	_cursor++;
	return;
	
    }else if(radix==1){
	for(size_t i=0; i<num; i++){
	    console_printToLoc("0", STATUSBAR_MSG_BASE+_cursor, 1);
	    _cursor++;
	}
	return;
    }

    uint16_t log = 0;
    for(uint32_t foo=num; foo>0; log++)
	foo/=radix;
	
    char digits[log+1];
    digits[log]=0;

    uint32_t exp = 1;
    for(size_t i=log; i>0; i--){
	digits[i-1] = (num/exp)%radix;
	if(digits[i-1]<10)
	    digits[i-1]+='0';
	else
	    digits[i-1]+='a'-10;
	exp*=radix;
    }

    console_printToLoc(digits, STATUSBAR_MSG_BASE+_cursor, log);
    _cursor+=log;
}
