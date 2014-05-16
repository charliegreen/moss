#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define __UNUSED __attribute__((__unused__))
#define OS_NAME "Moss v0.4"
#define OS_NAME_MAX_LEN 12

#include "console.h"

// Halt and Catch Fire
#define HCF(s) do{						\
	console_printErr("40HCF: "__FILE__": ");		\
	console_printNum(__LINE__,10);				\
	console_print(": "s"\n");				\
	hang();							\
    }while(0)

// ASSERT
#define ASSERT(expr) if(!(expr)){				\
	console_printErr("ASSERT FAILED: "__FILE__": ");	\
	console_printNum(__LINE__,10);				\
	console_print(": "#expr"\n");				\
    }


// Assembly functions
extern uint32_t __getCR0(void);
extern uint32_t __getCR1(void);
extern uint32_t __getCR2(void);
extern uint32_t __getCR3(void);
extern uint32_t __getCR4(void);

extern uint32_t __get_stack_top(void);
extern uint32_t __get_stack_bottom(void);

// `stdlib' functions
size_t strlen(const char*str);
size_t strlenesc(const char*str);
void strncpy(char*s1, const char*s2, size_t n);
void memset(void*address, uint8_t byte, size_t len);
uint8_t ctox(char c);

// Memory allocation wrappers
uint32_t kmalloc(uint32_t size);
uint32_t kamalloc(uint32_t size);
uint32_t kpmalloc(uint32_t size, uint32_t phys);
uint32_t kapmalloc(uint32_t size, uint32_t phys);

// Miscellaneous utility functions
void halt(void);
void hang(void);

void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);

uint8_t byteBCDToSanity(uint8_t bcd);
void printCR0(void);

#endif
