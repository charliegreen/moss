#ifndef DESCTAB_H
#define DESCTAB_H

#include "common.h"

void desctab_initialize(void);

//================================
// GDT
//================================

typedef struct{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
}__attribute__((__packed__)) gdt_entry_t;

typedef struct gdt_ptr_struct{
    uint16_t limit;    // The upper 16 bits of all selector limits.
    uint32_t base;     // The address of the first gdt_entry_t struct.
}__attribute__((__packed__)) gdt_ptr_t;

//================================
// IDT
//================================

#define IRQ0  32
#define IRQ1  33
#define IRQ2  34
#define IRQ3  35
#define IRQ4  36
#define IRQ5  37
#define IRQ6  38
#define IRQ7  39
#define IRQ8  40
#define IRQ9  41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

/* The special, CPU-dedicated interrupts are shown below:
   
   0 - Division by zero exception
   1 - Debug exception
   2 - Non maskable interrupt
   3 - Breakpoint exception
   4 - 'Into detected overflow'
   5 - Out of bounds exception
   6 - Invalid opcode exception
   7 - No coprocessor exception
   8 - Double fault (pushes an error code)
   9 - Coprocessor segment overrun
   10 - Bad TSS (pushes an error code)
   11 - Segment not present (pushes an error code)
   12 - Stack fault (pushes an error code)
   13 - General protection fault (pushes an error code)
   14 - Page fault (pushes an error code)
   15 - Unknown interrupt exception
   16 - Coprocessor fault
   17 - Alignment check exception
   18 - Machine check exception
   19-31 - Reserved
*/

typedef struct{
    uint16_t offset_1; // offset bits 0..15
    uint16_t selector; // a code segment selector in GDT or LDT
    uint8_t zero;      // unused, set to 0
    uint8_t type_attr; // type and attributes, see below
    uint16_t offset_2; // offset bits 16..31
}__attribute__((__packed__)) idt_entry_t;

typedef struct{
    uint16_t limit;
    uint32_t base;
}__attribute__((__packed__)) idt_ptr_t;

typedef struct{
    uint32_t ds;		// Data segment selector
    uint32_t edi, esi, ebp, esp,
	ebx, edx, ecx, eax;	// Pushed by pusha.
    uint32_t int_no, err_code;  // Interrupt num & error code (if applicable)
    uint32_t eip, cs, eflags, useresp, ss; // Processor automatically pushes
} registers_t;

// handler returns true if the interrupt was successfully handled
typedef bool (*isr_t)(registers_t);

/* the ISR wrappers */
extern void __isr0(void);
extern void __isr1(void);
extern void __isr2(void);
extern void __isr3(void);
extern void __isr4(void);
extern void __isr5(void);
extern void __isr6(void);
extern void __isr7(void);
extern void __isr8(void);
extern void __isr9(void);
extern void __isr10(void);
extern void __isr11(void);
extern void __isr12(void);
extern void __isr13(void);
extern void __isr14(void);
extern void __isr15(void);
extern void __isr16(void);
extern void __isr17(void);
extern void __isr18(void);
extern void __isr19(void);
extern void __isr20(void);
extern void __isr21(void);
extern void __isr22(void);
extern void __isr23(void);
extern void __isr24(void);
extern void __isr25(void);
extern void __isr26(void);
extern void __isr27(void);
extern void __isr28(void);
extern void __isr29(void);
extern void __isr30(void);
extern void __isr31(void);

extern void __irq0(void);
extern void __irq1(void);
extern void __irq2(void);
extern void __irq3(void);
extern void __irq4(void);
extern void __irq5(void);
extern void __irq6(void);
extern void __irq7(void);
extern void __irq8(void);
extern void __irq9(void);
extern void __irq10(void);
extern void __irq11(void);
extern void __irq12(void);
extern void __irq13(void);
extern void __irq14(void);
extern void __irq15(void);

void desctab_registerIntHandler(uint8_t n, isr_t handler);
void desctab_isrHandler(registers_t regs);
void desctab_irqHandler(registers_t regs);

/* utility functions */
//void idt_initialize(void);
//IDTEntry create_idt_entry(uint32_t offset, uint16_t selector);
//uint16_t create_interrupt_selector(uint8_t privs, uint16_t index);
//void print_idt_entry(IDTEntry i);

#endif
