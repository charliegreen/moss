#include "common.h"
#include "console.h"

uint16_t*_consoleBuffer;
size_t _row, _col;
uint8_t _cursorColor;
vga_color_t _color;

// local prototypes
static uint16_t make_vgaentry(char c, vga_color_t color);
static void newline(void);
static void scroll(void);

static uint16_t make_vgaentry(char c, vga_color_t color){
    uint16_t c16 = c;
    uint16_t color16 = color;
    return c16 | color16 << 8;
}

static void newline(void){
    console_putCharXY(' ', _color, _col, _row);
    _col=0;
    _row++;
    if(_row >= CONSOLE_HEIGHT)
	scroll();
}

static void scroll(void){
    size_t i;
    // moving lines up
    for(i=0; i<(CONSOLE_HEIGHT-1)*CONSOLE_WIDTH; i++)
	_consoleBuffer[i] = _consoleBuffer[i+CONSOLE_WIDTH];

    // clearing the next row
    for(; i<CONSOLE_HEIGHT*CONSOLE_WIDTH; i++)
	_consoleBuffer[i] = make_vgaentry(' ', _color);

    _row--;
}

void console_initialize(void){
    _consoleBuffer = CONSOLE_BUFFER + CONSOLE_WIDTH;
    _row = _col = 0;
    _color = console_makeColor(COLOR_LIGHT_GREY, COLOR_BLACK);
    _cursorColor = console_makeColor(COLOR_BLACK, COLOR_WHITE);
    
    for(size_t y = 0; y < CONSOLE_HEIGHT; y++)
	for(size_t x = 0; x < CONSOLE_WIDTH; x++)
	    console_putCharXY(' ', _color, x, y);
}

void console_setColor(vga_color_t color){
    _color=color;
}

vga_color_t console_getColor(void){
    return _color;
}

uint8_t console_makeColor(vga_color_t fg, vga_color_t bg){
    return fg | bg << 4;
}

void console_putCharXY(char c, uint8_t color, size_t x, size_t y){
    // x, y are size_t, so they must be positive
    if(y >= CONSOLE_HEIGHT || x > CONSOLE_WIDTH)
	return;
    const size_t index = y*CONSOLE_WIDTH + x;
    _consoleBuffer[index] = make_vgaentry(c,color);
}

void console_putChar(char c){
    switch(c){
    case '\n':
	newline();
	break;

    case '\t':{
	uint8_t diff = CONSOLE_TAB_WIDTH - _col % CONSOLE_TAB_WIDTH;
	for(uint8_t i=0; i<diff; i++)
	    console_putChar(' ');
	break;
    }
	
    default:{
	console_putCharXY(c, _color, _col, _row);
	_col++;
	if(_col == CONSOLE_WIDTH)
	    newline();	
	break;
    }
    }

    // draw the cursor
    //uint8_t color = console_makeColor(COLOR_BLACK, COLOR_WHITE);
    console_putCharXY(' ', _cursorColor, _col, _row);
}

void console_print(const char*data){
    size_t datalen = strlen(data);
    for(size_t i=0; i<datalen; i++)
	if(data[i]=='' && i+2<datalen){
	    uint8_t fg = ctox(data[i+1]);
	    uint8_t bg = ctox(data[i+2]);
	    if(fg<16 && bg<16){
		console_setColor(console_makeColor(fg, bg));
		i+=2;
	    }
	    
	}else
	    console_putChar(data[i]);
}

void console_printToLoc(const char*data, uint16_t*loc, size_t n){
    if(loc<CONSOLE_BUFFER || loc>CONSOLE_BUFFER+CONSOLE_WIDTH*CONSOLE_HEIGHT)
	return;

    size_t numBytesPrinted = 0;
    size_t datalen = strlen(data);
    for(size_t i=0; i<datalen; i++)
	if(data[i]=='' && i+2<datalen){
	    uint8_t fg = ctox(data[i+1]);
	    uint8_t bg = ctox(data[i+2]);
	    if(fg<16 && bg<16){
		console_setColor(console_makeColor(fg, bg));
		i+=2;
	    }
	    
	}else if(numBytesPrinted<n){
	    *loc = make_vgaentry(data[i], _color);
	    loc++;
	    numBytesPrinted++;
	}
}

void console_printNum(uint32_t num, uint32_t radix){
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
	
    char digits[log+1];
    digits[log]=0;

    uint32_t exp = 1;
    for(size_t i=log; i>0; i--){
	digits[i-1] = (num/exp)%radix;
	if(digits[i-1]<10)
	    digits[i-1]+='0';
	else
	    digits[i-1]+='a'-10;
	exp*=radix;
    }

    console_print(digits);
}

//================================
// Miscellaneous functions:

void console_displayColors(void){
    console_printInfo("Displaying all available colors:\n");
    for(size_t i=0; i<16; i++){
	for(size_t j=0; j<16; j++){
	    console_setColor(console_makeColor(i,j));
	    console_putChar('a');
	}
	console_putChar('\n');
    }
}

void console_displayCharacters(void){
    console_printInfo("Displaying all available characters:");
    for(size_t i=0; i<128; i++){
	if(i%16==0)
	    console_putChar('\n');

	char c = i&0xFF;
	console_putCharXY(c, _color, _col++, _row);
    }

    console_putChar('\n');
}

void console_backspace(void){
    // erase the cursor
    console_putCharXY(' ', _color, _col, _row);

    if(_col != 0)
	_col--;
    else if(_row != 0){
	_col=CONSOLE_WIDTH-1;
	_row--;
    }

    // redraw the cursor
    console_putCharXY(' ', _cursorColor, _col, _row);
}

void console_toggleCursorColor(void){
    if((_cursorColor & 0xf) == COLOR_WHITE) // if fg==white
	_cursorColor = console_makeColor(COLOR_BLACK, COLOR_WHITE);
    else
	_cursorColor = console_makeColor(COLOR_WHITE, COLOR_BLACK);
    console_putCharXY(' ', _cursorColor, _col, _row);
}
