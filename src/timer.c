#include "timer.h"
#include "common.h"
#include "desctab.h"
#include "console.h"
#include "statusbar.h"
#include "cmos.h"

#define TIMER_MAX_NUM_HANDLERS (0xffff)

timerHandler_t _timerHandlers[TIMER_MAX_NUM_HANDLERS];
uint32_t _timerHandlerIntervals[TIMER_MAX_NUM_HANDLERS];
uint32_t _numHandlers=0;

volatile uint32_t _tick=0; // at 100Hz, this will loop about every 32.7 years

void timer_registerHandler(timerHandler_t handler, uint32_t ms){
    uint32_t ticks = ms*TIMER_PIT_FREQUENCY/1000;
    
    if(_numHandlers+1 >= TIMER_MAX_NUM_HANDLERS)
	HCF("Attempted to register too many timer handlers");

    _timerHandlers[_numHandlers] = handler;
    _timerHandlerIntervals[_numHandlers] = ticks;
    _numHandlers++;
}

static bool irq0(__UNUSED registers_t regs){
    //statusbar_clearMsg();
    //statusbar_printNum(tick++, 10);
    _tick++;

    /*
    if(tick % (TIMER_PIT_FREQUENCY/2) == 0)
	console_toggleCursorColor();	
    
    if(tick % TIMER_PIT_FREQUENCY == 0){
	cmos_readTimeFromRTC();
	//cmos_printTime();
    }
    */

    for(uint32_t i=0; i<_numHandlers; i++)
	if(_tick % _timerHandlerIntervals[i] == 0)
	    _timerHandlers[i]();
    return true;
}

void timer_initialize(void){
    const uint32_t frequency = TIMER_PIT_FREQUENCY;
    
    // First, register our timer callback.
    desctab_registerIntHandler(IRQ0, &irq0);

    // The value we send to the PIT is the value to divide its input clock
    // (1193180 Hz) by to get our requested frequency.
    // Note the divisor is 16 bits, which makes our lowest frequency 19 Hz.
    uint16_t divisor = 1193180 / frequency;
    if(frequency<19){
	console_printWarn("Timer frequency set below 19Hz; effective frequency is ");
	console_printNum(1193180/divisor, 10);
	console_print("Hz!\n");
    }
    
    // Send the command byte.
    outb(0x43, 0x36);

    // Divisor has to be sent byte-wise, so split here into upper/lower bytes
    uint8_t l = (uint8_t)((divisor>>0) & 0xFF);
    uint8_t h = (uint8_t)((divisor>>8) & 0xFF);

    // Send the frequency divisor.
    outb(0x40, l);
    outb(0x40, h);
}

void timer_sleep(uint32_t ms){
    uint32_t numTicks = ms*TIMER_PIT_FREQUENCY/1000;
    uint32_t now = _tick;

    // Even if _tick overflows, our addition here will too, so we get the
    // proper and expected behavior (I think; I haven't tested this). TODO?
    while(_tick != now+numTicks)
	halt();
}
