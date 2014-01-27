#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "common.h"

void keyboard_initialize(void);
uint8_t keyboard_scanByte(void);
char keyboard_getc(void);

#endif
