#ifndef STATUSBAR_H
#define STATUSBAR_H

#include "common.h"

void statusbar_initialize(void);

//================================
// FLAGS
//================================

#define STATUSBAR_NUM_FLAGS 2
/* Our flags are:
 *   R/B/H - Running, blocking, or halted [running flag]
 *    /U/H - <nothing>, unhandled/handled interrupt raised [interrupt flag]
 */

#define STATUSBAR_FLAG_BASE (uint16_t*)(CONSOLE_BUFFER+OS_NAME_MAX_LEN+3)
#define STATUSBAR_RUNNING_FLAG_LOC (STATUSBAR_FLAG_BASE)
#define STATUSBAR_INTERRUPT_FLAG_LOC (STATUSBAR_FLAG_BASE+1)

typedef enum running_flag{
    RUNNING, BLOCKING, HALTED
} running_flag_t;

typedef enum interrupt_flag{
    NONE, UNHANDLED, HANDLED
} interrupt_flag_t;

void statusbar_setRunningFlag(running_flag_t f);
void statusbar_setInterruptFlag(interrupt_flag_t f);

//================================
// MESSAGE AREA
//================================
#define STATUSBAR_MSG_BASE (uint16_t*)(STATUSBAR_FLAG_BASE+STATUSBAR_NUM_FLAGS+2)
#define STATUSBAR_MSG_MAX_LEN (CONSOLE_WIDTH-OS_NAME_MAX_LEN-STATUSBAR_NUM_FLAGS-6)

void statusbar_clearMsg(void);
void statusbar_printMsg(const char*data);
void statusbar_printNum(uint32_t num, uint32_t radix);

#endif
