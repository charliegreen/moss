#include "common.h"
#include "console.h"
#include "statusbar.h"
#include "heap.h"

// ================================================================================ "stdlib"

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
    for(size_t i=0; i<n; i++){
	s1[i] = s2[i];
	if(!s2[i])
	    break;
    }
}

void memset(void*address, uint8_t byte, size_t len){
    uint32_t base = (uint32_t)address;
    for(uint32_t addr = base; addr < base+len; addr++)
	*(uint8_t*)addr = byte;
}

void*memcpy(void*dst, const void*src, size_t n){
    for(size_t i = 0; i<n; i++)
	((uint8_t*)dst)[i] = ((uint8_t*)src)[i];
    return dst;
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
size_t snprintf(char*buf, size_t n, const char*fmt, ...){
    // TODO write snprintf body
}
*/

int32_t printf(const char*fmt, ...){
    asm volatile("cli");
#define PRINTF_ERR(err,msg) do{			\
	statusbar_clearMsg();			\
	statusbar_printMsg("printf(): -");	\
	statusbar_printNum(-err,10);		\
	statusbar_printMsg("; "msg);		\
	return err;				\
    }while(0)
    
    uint32_t*args = (uint32_t*)&fmt;
    size_t argi = 1;		// index into args
    size_t written = 0;

    for(size_t fmti = 0; fmt[fmti]; fmti++){
	if(fmt[fmti] == '%'){	// -------------------------------- FOUND FORMAT REQUEST
	    // Note: this list should match the switch below
	    static const char formatSpecifiers[] = { 'd', 'x', 'b', '%', 's' };
	    char info[6];
	    info[5] = 0;	// null terminated
	    char spec = 0;
	    size_t infolen;

	    // buffer up any formatting information before the specifier (eg in %2d or something)
	    for(size_t i=0; i<sizeof(info)-1; i++){
		if(!fmt[fmti+1+i])
		    PRINTF_ERR(-1, "reached end of format string before found format specifier");

		// Check if we found our specifier character; if so, "double-break" via goto
		for(size_t j=0; j < sizeof(formatSpecifiers); j++)
		    if(fmt[fmti+1+i] == formatSpecifiers[j]){
			info[i] = 0;
			spec = formatSpecifiers[j];
			goto gathered_info; // Yeah, yeah. This is actually cleaner, trust me.
		    }

		info[i] = fmt[fmti+1+i]; // copy over info character
	    }
	    
	gathered_info:
	    infolen = strlen(info);
	    switch(spec){
	    case 0:
		PRINTF_ERR(-2, "Data before format specifier was too long");
		break;

	    case '%':
		putchar('%');
		written++;
		break;
		
	    case 'd':
	    case 'x':
	    case 'b':{
		uint8_t base;
		if(spec == 'b'){
		    // %b defaults to binary
		    base = infolen ? atoi(info) : 2;
		}else
		    base = spec=='d' ? 10 : 16;
		
		if((infolen != 0 && spec != 'b') || base == 0)
		    PRINTF_ERR(-4, "Bad information");
		console_printNum(args[argi++], base);
		break;
	    }

	    case 's':{
		uint32_t minLen = atoi(info); // atoi("not numbers") is 0
		if(infolen != 0 && minLen == 0)
		    PRINTF_ERR(-4, "Bad information");
		
		const char*s = (const char*)args[argi++];
		size_t slen  = strlen(s);
		while(minLen > slen){
		    putchar(' ');
		    slen++;
		}
		console_print(s);
		break;
	    }

	    default:
		PRINTF_ERR(-3, "Format specifier NYI");
		break;
	    }

	    fmti += infolen+1;	// skip over info and format specifier
	}else if(fmt[fmti] == '' && fmt[fmti+1] && fmt[fmti+2]){
				// -------------------------------- COLOR CODE
	    // Copy the color code over and let console_print figure it out for us
	    char copy[4];
	    memcpy(copy, &fmt[fmti], 3);
	    copy[3] = 0;
	    console_print(copy);
	    fmti+=2;
	}else{	  // -------------------------------- SIMPLE CHARACTER
	    putchar(fmt[fmti]);
	    written++;
	}
    }
    asm volatile("sti");
    return written;
#undef PRINTF_ERR
}

uint32_t atoi(const char*buf){ // TODO make this take more complicated numbers than \s+[0-9]*
    while(*buf == ' ')	       // skip leading spaces
	buf++;

    uint32_t numDigits = 0;
    while(buf[numDigits]>='0' && buf[numDigits]<='9')
	numDigits++;

    uint32_t result = 0;
    for(size_t i=0; i<numDigits; i++)
	result += pow(10, numDigits-i-1)*(buf[i]-'0');

    return result;
}

uint32_t pow(uint32_t a, uint32_t b){
    uint32_t ret = 1;
    for(; b; b--)
	ret *= a;
    return ret;
}

// ================================================================================ malloc wrappers

uint32_t kmalloc  (uint32_t size){ return heap_allocate(size, false, 0); }
uint32_t kamalloc (uint32_t size){ return heap_allocate(size, true,  0); }
uint32_t kpmalloc (uint32_t size, uint32_t phys){
    return heap_allocate(size, false, phys);
}
uint32_t kapmalloc(uint32_t size, uint32_t phys){
    return heap_allocate(size, true,  phys);
}

// ================================================================================ Misc utils

void halt(void){
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

// Prints CR0 in a readable fashion. For further information, see the AMD64 Architecture
// Programmer's Manual Volume 2: System Programming entry on CR0, beginning on page 42.
// You can find this entry on page 90 of this PDF:
// http://developer.amd.com/wordpress/media/2012/10/24593_APM_v21.pdf
void printCR0(void){
    uint32_t cr0 = __getCR0();
    console_print("[CR0:");
    if(cr0 & 1<<31) console_print(" PG"); // paging enabled
    if(cr0 & 1<<30) console_print(" CD"); // cache disabled
    if(cr0 & 1<<29) console_print(" NW"); // not writethrough
    if(cr0 & 1<<18) console_print(" AM"); // alignment mask
    if(cr0 & 1<<16) console_print(" WP"); // write protect
    if(cr0 & 1<< 5) console_print(" NE"); // numeric error
    if(cr0 & 1<< 4) console_print(" ET"); // extension type
    if(cr0 & 1<< 3) console_print(" TS"); // task switched
    if(cr0 & 1<< 2) console_print(" EM"); // emulation
    if(cr0 & 1<< 1) console_print(" MP"); // monitor coprocessor
    if(cr0 & 1<< 0) console_print(" PE"); // protection enabled
    console_print("]");
}
