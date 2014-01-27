#include "keyboard.h"
#include "common.h"
#include "console.h"
#include "desctab.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_COMM_PORT 0x64
#define KEYBOARD_STAT_PORT 0x64

#define KEYBOARD_READ_READY  0x1
#define KEYBOARD_WRITE_READY 0x2

#define KEYBOARD_COMM_ACK  0xFA
#define KEYBOARD_COMM_NACK 0xFE

//================================
// Fields

// _keymap[scancode] == character (when no modifiers are pressed)
// special entries or empty entries are 0
const char _keymap[] = {
    0   , '', '1', '2' , '3' , '4', '5', '6' ,
    '7' , '8' , '9', '0' , '-' , '=', 0  , '\t',
    'q' , 'w' , 'e', 'r' , 't' , 'y', 'u', 'i' ,
    'o' , 'p' , '[', ']' , '\n', 0  , 'a', 's' ,
    'd' , 'f' , 'g', 'h' , 'j' , 'k', 'l', ';' ,
    '\'', '`' , 0  , '\\', 'z' , 'x', 'c', 'v' ,
    'b' , 'n' , 'm', ',' , '.' , '/', 0  , '*' ,
    0   , ' ' , 0  , 0   , 0   , 0  , 0  , 0   ,
    0   , 0   , 0  , 0   , 0   , 0  , 0  , '7' ,
    '8' , '9' , '-', '4' , '5' , '6', '+', '1' ,
    '2' , '3' , '6', 0   , 0   , 0  , 0  , 0
};

const char _keymapShifted[] = {
    0   , 0   , '!', '@' , '#' , '$', '%', '^' ,
    '&' , '*' , '(', ')' , '_' , '+', 0  , 0   ,
    'Q' , 'W' , 'E', 'R' , 'T' , 'Y', 'U', 'I' ,
    'O' , 'P' , '{', '}' , 0   , 0  , 'A', 'S' ,
    'D' , 'F' , 'G', 'H' , 'J' , 'K', 'L', ':' ,
    '"' , '~' , 0  , '|' , 'Z' , 'X', 'C', 'V' ,
    'B' , 'N' , 'M', '<' , '>' , '?', 0  , 0   ,
    0   , 0   , 0  , 0   , 0   , 0  , 0  , 0   ,
    0   , 0   , 0  , 0   , 0   , 0  , 0  , 0   ,
    0   , 0   , 0  , 0   , 0   , 0  , 0  , 0   ,
    0   , 0   , 0  , 0   , 0   , 0  , 0  , 0
};

bool _leftShift, _rightShift, _capsLock;

// [ascii] & 0xF0 >> 4 (3 bits) indexes this array. The other 4 index shifts.
//uint16_t _keysPressed[] = { 0,0,0,0,0,0,0,0 };

//================================
// All local prototypes
static void drainInput(void);
static bool irq1(registers_t regs);
//static bool isCommAcked(void);
//static void flushOutput(void);
//static void flushInput(void);

//================================
// Local function definitions

static void drainInput(void){
    uint8_t b = inb(KEYBOARD_STAT_PORT);
    while(b & KEYBOARD_READ_READY){
	b = inb(KEYBOARD_STAT_PORT);
	inb(KEYBOARD_DATA_PORT);
    }
}

static bool irq1(__UNUSED registers_t regs){
    /*
    uint8_t s,d;
    do{
	s = inb(KEYBOARD_STAT_PORT);
    }while((s & KEYBOARD_READ_READY) == 0);

    while(s & KEYBOARD_READ_READY){
	d = inb(KEYBOARD_DATA_PORT);
	s = inb(KEYBOARD_STAT_PORT);
	console_printNum(d, 16);
    }
    */
    
    char c = keyboard_getc();
    if(c)
	console_putChar(c);
    return true;
}

/*
static bool isCommAcked(void){
    return inb(KEYBOARD_DATA_PORT) == KEYBOARD_COMM_ACK;
}

// Flush keyboard's buffer so we can write
static void flushOutput(void){
    while(true)
	if(! (inb(KEYBOARD_STAT_PORT) & KEYBOARD_WRITE_READY))
	    break;
}

// Flush keyboard's buffer so we can read
static void flushInput(void){
    while(true)
	if(! (inb(KEYBOARD_STAT_PORT) & KEYBOARD_READ_READY))
	    break;
}
*/

//================================
// Global function definitions

void keyboard_initialize(void){
    desctab_registerIntHandler(IRQ1, irq1);
    drainInput();
}

uint8_t keyboard_scanByte(void){
    uint8_t b;
    do{
	b = inb(KEYBOARD_STAT_PORT);
    }while((b & KEYBOARD_READ_READY) == 0);
    return inb(KEYBOARD_DATA_PORT);
}

// NOTE: this assumes keyboard scan code set 1
char keyboard_getc(void){
    const char*keymap = _keymap;
    char ret = 0;
    uint8_t scancode = keyboard_scanByte();
    
    if(scancode>=sizeof(_keymap) || _keymap[scancode]==0){
	// our key requires special treatment
	switch(scancode){
	case 0x36:
	    _rightShift=true;
	    break;
	case 0x2a:
	    _leftShift=true;
	    break;
	case 0xb6:
	    _rightShift=false;
	    break;
	case 0xaa:
	    _leftShift=false;
	    break;
	case 0x3a:
	    _capsLock=!_capsLock;
	    break;
	case 0x0e:
	    console_backspace();
	    break;

	/* Need to deal with 2-byte scancodes without spinning the CPU. TODO
	case 0xe0:{ // prelude to two-byte scancodes
	    uint8_t scancode2 = inb(KEYBOARD_DATA_PORT); //keyboard_scanByte();
	    console_print("40e8");
	    console_printNum(scancode2, 16);
	    console_print("70");
	    break;
	}
	*/

	default:
	    if(~scancode&0x80){
		console_print("40");
		console_printNum(scancode, 16);
		console_print("70");
	    }
	    break;
	}
	return 0;
	
    }else{
	if(_leftShift || _rightShift)
	    keymap = _keymapShifted;

	// we use the shifted keymap if we're typing a letter and _capsLock
	char c = _keymapShifted[scancode];
	if('A'<=c && c<='Z' && _capsLock)
	    keymap = _keymapShifted;
	
	ret = keymap[scancode];
    }

    return ret;
}
