#include "common.h"
#include "console.h"
#include "statusbar.h"
#include "heap.h"

size_t strlen(const char*str){
    size_t ret = 0;
    while(str[ret] != 0)
	ret++;
    return ret;
}

// strlen, but checks for color escape sequences
size_t strlenesc(const char*str){
    size_t ret = 0;
    for(size_t i=0; str[i]!=0; i++)
	// depending on lazy evaluation to verify we have room
	if(str[i] == '' && str[i+1]!=0 && str[i+2]!=0){
	    uint8_t fg = ctox(str[i+1]);
	    uint8_t bg = ctox(str[i+2]);
	    if(fg<16 && bg<16){
		i+=2;
	    }else
		ret+=3;
	}else
	    ret++;
    return ret;
}

void strncpy(char*s1, const char*s2, size_t n){
    for(size_t i=0; i<n; i++)
	s1[i] = s2[i];
}

void memset(void*address, uint8_t byte, size_t len){
    uint32_t base = (uint32_t)address;
    for(uint32_t addr = base; addr < base+len; addr++)
	*(uint8_t*)addr = byte;
}

uint8_t ctox(char c){
    if(c >= '0' && c <= '9')
	return c-'0';
    else if(c >= 'A' && c <= 'F')
	return c-'A'+10;
    else if(c >= 'a' && c <= 'f')
	return c-'a'+10;
    else
	return 0xFF;
}

/*
void printNum(uint32_t num, uint32_t radix){
    if(num==0){
	console_putChar('0');
	return;
    }else if(radix==1){
	for(size_t i=0; i<num; i++)
	    console_putChar('0');
	return;
    }

    uint16_t log = 0;
    for(uint32_t foo=num; foo>0; log++)
	foo/=radix;
	
    uint8_t digits[log];

    uint32_t exp = 1;
    for(size_t i=0; i<log; i++){
	digits[i] = (num/exp)%radix;
	exp*=radix;
    }

    for(size_t i=log; i>0; i--)
	console_putChar((digits[i-1]<10)  ?
			(digits[i-1]+'0') :
			(digits[i-1]+'A'-10));
}
*/

void hlt(void){
    statusbar_setRunningFlag(BLOCKING);
    asm("hlt");
    statusbar_setRunningFlag(RUNNING);
}

void hang(void){
    console_printInfo("40Forced halt\n");
    statusbar_setRunningFlag(HALTED);
    asm("jmp label_hang");
}

void outb(uint16_t port, uint8_t value){
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

uint8_t inb(uint16_t port){
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

uint16_t inw(uint16_t port){
    uint16_t ret;
    asm volatile("inw %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

uint8_t byteBCDToSanity(uint8_t bcd){
    return (bcd/16)*10 + (bcd & 0xF);
}

uint32_t kmalloc  (uint32_t size){ return heap_allocate(size, false, 0); }
uint32_t kamalloc (uint32_t size){ return heap_allocate(size, true,  0); }
uint32_t kpmalloc (uint32_t size, uint32_t phys){
    return heap_allocate(size, false, phys);
}
uint32_t kapmalloc(uint32_t size, uint32_t phys){
    return heap_allocate(size, true,  phys);
}
