#ifndef CONSOLE_H
#define CONSOLE_H

#define CONSOLE_WIDTH 80
#define CONSOLE_HEIGHT 24
#define CONSOLE_TAB_WIDTH 8

#define CONSOLE_BUFFER (uint16_t*)0xB8000

/* Hardware text mode color constants. */
typedef enum vga_color_enum{
    COLOR_BLACK = 0,
    COLOR_BLUE = 1,
    COLOR_GREEN = 2,
    COLOR_CYAN = 3,
    COLOR_RED = 4,
    COLOR_MAGENTA = 5,
    COLOR_BROWN = 6,
    COLOR_LIGHT_GREY = 7,
    COLOR_DARK_GREY = 8,
    COLOR_LIGHT_BLUE = 9,
    COLOR_LIGHT_GREEN = 10,
    COLOR_LIGHT_CYAN = 11,
    COLOR_LIGHT_RED = 12,
    COLOR_LIGHT_MAGENTA = 13,
    COLOR_LIGHT_BROWN = 14,
    COLOR_WHITE = 15,
} vga_color_t;

void console_initialize(void);
void console_setColor(vga_color_t color);
vga_color_t console_getColor(void);
uint8_t console_makeColor(vga_color_t fg, vga_color_t bg);

void console_putCharXY(char c, uint8_t color, size_t x, size_t y);
void console_putChar(char c);
void console_print(const char*data);
void console_printToLoc(const char*data, uint16_t*loc, size_t n);
void console_printNum(uint32_t num, uint32_t radix);

void console_displayColors(void);
void console_displayCharacters(void);
void console_backspace(void);

void console_toggleCursorColor(void);

// For convenience:
#define CONSOLE_STR_INFO "70[30info70] "
#define CONSOLE_STR_OK	 "70[20 ok 70] "
#define CONSOLE_STR_WARN "70[40warn70] "
#define CONSOLE_STR_ERR  "70[A0ERR!70] "

#define console_printInfo(data) console_print(CONSOLE_STR_INFO	data)
#define console_printOk(data)	console_print(CONSOLE_STR_OK	data)
#define console_printWarn(data)	console_print(CONSOLE_STR_WARN	data)
#define console_printErr(data)	console_print(CONSOLE_STR_ERR	data)

#define putchar(c) console_putChar(c)

#endif
