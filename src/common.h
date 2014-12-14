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
void*memcpy(void*dst, const void*src, size_t n);
uint8_t ctox(char c);
uint32_t atoi(const char*buf);
uint32_t pow(uint32_t a, uint32_t b);

/* Documentation on *printf:
   %s, %Ns	Print a string. If N, pad the string with spaces to have a minimum
   		length of N.
   %d		Print an integer in base 10.
   %x		Print an integer in base 16.
   %b, %Nb	Print an integer; N specifies the base. If N isn't specified,
   		%b defaults to binary.
 */
size_t snprintf(char*buf, size_t n, const char*fmt, ...); // TODO
int32_t printf(const char*fmt, ...);

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
