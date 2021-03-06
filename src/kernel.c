#include "common.h"
#include "kernel.h"
#include "console.h"
#include "statusbar.h"
#include "desctab.h"
#include "keyboard.h"
#include "timer.h"
#include "cmos.h"
#include "paging.h"
#include "heap.h"

// global variables:
extern uint32_t _kernelStart;	// defined in linker.ld
extern uint32_t _kernelEnd;

void kernel_main(__UNUSED uint32_t a, __UNUSED uint32_t magic){
    desctab_initialize();
    
    console_initialize();
    console_print("Initialized console\n");

    /*
    console_print("Stack at 0x");
    uint32_t esp;
    asm volatile("movl %%esp, %0" : "=r"(esp));
    console_printNum(esp, 16);

    uint32_t sb = __get_stack_bottom();
    uint32_t st = __get_stack_top();
    
    console_print("\nStack bottom: 0x");
    console_printNum(sb, 16);
    console_print("\nStack top: 0x");
    console_printNum(st, 16);
    console_print("\ndiff: 0x");
    console_printNum(st-sb, 16);
    console_putChar('\n');
    */
    
    // initialize various parts of the kernel, and print progress to screen
    KERNEL_INIT(statusbar,);
    KERNEL_INIT(keyboard,);
    KERNEL_INIT(timer, {
	    console_print("(");
	    console_printNum(TIMER_PIT_FREQUENCY, 10);
	    console_print("Hz)");
	});

    {				// Initialize periodic timer handlers
	console_print("    Registering CMOS RTC update timer..");
	timer_registerHandler(&cmos_readTimeFromRTC, 1000);
	console_print("20ok70\n");
    }
    
    cmos_readTimeFromRTC();
    console_printOk("Read time from CMOS: ");
    cmos_printTime();
    console_putChar('\n');
    
    KERNEL_INIT(paging,);
    
    //================================
    // Print the welcome message!
    console_printInfo("Booted 20");
    console_print(OS_NAME);
    console_print("70, by 50Charlie Green\n");
    //console_printInfo("20Moss70 began development on Jul 26 2013.\n");
    console_printInfo("This version was built on ");
    console_print(__DATE__);
    console_print(" at ");
    console_print(__TIME__);
    console_print(" PST.\n");
    
    asm volatile ("sti");	// enable IRQs
    
    //================================
    // DONE INITIALIZING OS
    //================================

    // Print some information on memory usage, for debugging paging
    console_print("\n");
    console_printInfo("Kernel goes from 0x");
    console_printNum((uint32_t)&_kernelStart, 16);
    console_print(" to 0x");
    console_printNum((uint32_t)&_kernelEnd, 16);
    console_print(", with length 0x");
    console_printNum((uint32_t)&_kernelEnd-(uint32_t)&_kernelStart, 16);
    console_print("=");
    console_printNum((uint32_t)&_kernelEnd-(uint32_t)&_kernelStart, 10);
    console_print("\n");
    console_printInfo("Heap allocated up to: 0x");
    console_printNum(heap_getHeapAddress(), 16);
    console_print("\n");
    
    while(true)
	halt();

    //================================
    console_printInfo("40Now halting70\n");
    statusbar_setRunningFlag(HALTED);
}
