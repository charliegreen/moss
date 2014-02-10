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

// local prototypes go here:
static void kernel_welcome(void);

static void kernel_welcome(void){
    console_printInfo("Booted 20");
    console_print(OS_NAME);
    console_print("70, by 50Charlie Green\n");
    //console_printInfo("20Moss70 began development on Jul 26 2013.\n");
    console_printInfo("This version was built on ");
    console_print(__DATE__);
    console_print(" at ");
    console_print(__TIME__);
    console_print(" PST.\n");
    //console_printInfo("It's kind of really super duper exciting!\n");
}

void kernel_main(__UNUSED uint32_t a, __UNUSED uint32_t magic){
    desctab_initialize();
    
    console_initialize();
    console_print("Initialized console\n");
    
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

    console_printInfo("Heap memory begins at 0x");
    console_printNum(heap_getHeapAddress(), 16);
    console_print("\n");
    
    KERNEL_INIT(paging,);
    
    //paging_initialize();
    //console_printOk("Initialized paging\n");
    
    kernel_welcome();
    
    // enable IRQs
    asm volatile ("sti");
    
    //================================
    // DONE INITIALIZING OS
    //================================

    console_printInfo("Now testing page fault handler....\n");
    uint8_t*ptr = (uint8_t*)0xA0000000;
    __UNUSED uint8_t hcf = *ptr;
    console_printInfo("This should not print!!\n");
    
    while(true)
	hlt();

    //================================
    console_printInfo("40Now halting70\n");
    statusbar_setRunningFlag(HALTED);
}
