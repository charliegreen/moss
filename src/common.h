#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define __UNUSED __attribute__((__unused__))
//#define IGNORE(arg) (*(uint8_t*)&arg+=0)
#define OS_NAME "Moss v0.3"
#define OS_NAME_MAX_LEN 12

#include "console.h"

// Halt and Catch Fire
#define HCF(s) do{ console_printErr("40HALT AND CATCH FIRE: "__FILE__": ");\
    console_printNum(__LINE__,10); \
    console_print(": "s"\n");	   \
    hang(); }while(0)

extern uint32_t __getCR0(void);
extern uint32_t __getCR1(void);
extern uint32_t __getCR2(void);
extern uint32_t __getCR3(void);
extern uint32_t __getCR4(void);

size_t strlen(const char*str);
size_t strlenesc(const char*str);
void strncpy(char*s1, const char*s2, size_t n);
void memset(void*address, uint8_t byte, size_t len);
uint8_t ctox(char c);
void printNum(uint32_t num, uint32_t radix);
void hlt(void);
void hang(void);

void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);

uint8_t byteBCDToSanity(uint8_t bcd);

uint32_t kmalloc(uint32_t size);
uint32_t kamalloc(uint32_t size);
uint32_t kpmalloc(uint32_t size, uint32_t phys);
uint32_t kapmalloc(uint32_t size, uint32_t phys);

#endif
