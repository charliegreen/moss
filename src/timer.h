#ifndef TIMER_H
#define TIMER_H

#include "common.h"

#define TIMER_PIT_FREQUENCY 1000
typedef uint64_t time_t;
typedef void (*timerHandler_t)(void);

void timer_initialize(void);
void timer_registerHandler(timerHandler_t handler, uint32_t ms);
void timer_sleep(uint32_t ms);

#endif
