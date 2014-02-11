#include "desctab.h"
#include "common.h"
#include "statusbar.h"
#include "console.h"

//================================
// All local file prototypes, in order of occurrence:
static void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit,
			 uint8_t access, uint8_t gran);
static void init_gdt(void);

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel,
			 uint8_t flags);
static void init_idt(void);
static bool doesIntHaveErrCode(uint8_t num);

static bool isr0 (registers_t regs);
static bool isr1 (registers_t regs);
static bool isr13(registers_t regs);

//================================
// GDT
//================================

gdt_entry_t _gdt[5];
gdt_ptr_t _gdt_ptr;

extern void __flush_gdt(uint32_t gdt);

// Set the value of one GDT entry.
static void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit,
			 uint8_t access, uint8_t gran)
{
    _gdt[num].base_low    = (base & 0xFFFF);
    _gdt[num].base_middle = (base >> 16) & 0xFF;
    _gdt[num].base_high   = (base >> 24) & 0xFF;

    _gdt[num].limit_low   = (limit & 0xFFFF);
    _gdt[num].granularity = (limit >> 16) & 0x0F;

    _gdt[num].granularity |= gran & 0xF0;
    _gdt[num].access      = access;
}

static void init_gdt(void){
    _gdt_ptr.limit = sizeof(gdt_entry_t)*5 - 1;
    _gdt_ptr.base  = (uint32_t)&_gdt;

    gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment

    __flush_gdt((uint32_t)&_gdt_ptr);
}

//================================
// IDT
//================================

idt_entry_t _idt[256];
idt_ptr_t _idt_ptr;
isr_t _interrupt_handlers[256];

extern void __flush_idt(uint32_t idt);

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel,
			 uint8_t flags)
{
    _idt[num].offset_1 = base & 0xFFFF;
    _idt[num].offset_2 = (base >> 16) & 0xFFFF;

    _idt[num].selector = sel;
    _idt[num].zero = 0;
    // We must uncomment the OR below when we get to using user-mode.
    // It sets the interrupt gate's privilege level to 3.
    _idt[num].type_attr = flags /* | 0x60 */;
}

static void init_idt(void){
    _idt_ptr.limit = sizeof(idt_entry_t) * 256 -1;
    _idt_ptr.base  = (uint32_t)&_idt;

    memset(&_idt, 0, sizeof(idt_entry_t)*256);

    idt_set_gate( 0, (uint32_t)__isr0 , 0x08, 0x8E);
    idt_set_gate( 1, (uint32_t)__isr1 , 0x08, 0x8E);
    idt_set_gate( 2, (uint32_t)__isr2 , 0x08, 0x8E);
    idt_set_gate( 3, (uint32_t)__isr3 , 0x08, 0x8E);
    idt_set_gate( 4, (uint32_t)__isr4 , 0x08, 0x8E);
    idt_set_gate( 5, (uint32_t)__isr5 , 0x08, 0x8E);
    idt_set_gate( 6, (uint32_t)__isr6 , 0x08, 0x8E);
    idt_set_gate( 7, (uint32_t)__isr7 , 0x08, 0x8E);
    idt_set_gate( 8, (uint32_t)__isr8 , 0x08, 0x8E);
    idt_set_gate( 9, (uint32_t)__isr9 , 0x08, 0x8E);
    idt_set_gate(10, (uint32_t)__isr10, 0x08, 0x8E);
    idt_set_gate(11, (uint32_t)__isr11, 0x08, 0x8E);
    idt_set_gate(12, (uint32_t)__isr12, 0x08, 0x8E);
    idt_set_gate(13, (uint32_t)__isr13, 0x08, 0x8E);
    idt_set_gate(14, (uint32_t)__isr14, 0x08, 0x8E);
    idt_set_gate(15, (uint32_t)__isr15, 0x08, 0x8E);
    idt_set_gate(16, (uint32_t)__isr16, 0x08, 0x8E);
    idt_set_gate(17, (uint32_t)__isr17, 0x08, 0x8E);
    idt_set_gate(18, (uint32_t)__isr18, 0x08, 0x8E);
    idt_set_gate(19, (uint32_t)__isr19, 0x08, 0x8E);
    idt_set_gate(20, (uint32_t)__isr20, 0x08, 0x8E);
    idt_set_gate(21, (uint32_t)__isr21, 0x08, 0x8E);
    idt_set_gate(22, (uint32_t)__isr22, 0x08, 0x8E);
    idt_set_gate(23, (uint32_t)__isr23, 0x08, 0x8E);
    idt_set_gate(24, (uint32_t)__isr24, 0x08, 0x8E);
    idt_set_gate(25, (uint32_t)__isr25, 0x08, 0x8E);
    idt_set_gate(26, (uint32_t)__isr26, 0x08, 0x8E);
    idt_set_gate(27, (uint32_t)__isr27, 0x08, 0x8E);
    idt_set_gate(28, (uint32_t)__isr28, 0x08, 0x8E);
    idt_set_gate(29, (uint32_t)__isr29, 0x08, 0x8E);
    idt_set_gate(30, (uint32_t)__isr30, 0x08, 0x8E);
    idt_set_gate(31, (uint32_t)__isr31, 0x08, 0x8E);

    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);

    idt_set_gate(IRQ0,  (uint32_t)__irq0,  0x08, 0x8E);
    idt_set_gate(IRQ1,  (uint32_t)__irq1,  0x08, 0x8E);
    idt_set_gate(IRQ2,  (uint32_t)__irq2,  0x08, 0x8E);
    idt_set_gate(IRQ3,  (uint32_t)__irq3,  0x08, 0x8E);
    idt_set_gate(IRQ4,  (uint32_t)__irq4,  0x08, 0x8E);
    idt_set_gate(IRQ5,  (uint32_t)__irq5,  0x08, 0x8E);
    idt_set_gate(IRQ6,  (uint32_t)__irq6,  0x08, 0x8E);
    idt_set_gate(IRQ7,  (uint32_t)__irq7,  0x08, 0x8E);
    idt_set_gate(IRQ8,  (uint32_t)__irq8,  0x08, 0x8E);
    idt_set_gate(IRQ9,  (uint32_t)__irq9,  0x08, 0x8E);
    idt_set_gate(IRQ10, (uint32_t)__irq10, 0x08, 0x8E);
    idt_set_gate(IRQ11, (uint32_t)__irq11, 0x08, 0x8E);
    idt_set_gate(IRQ12, (uint32_t)__irq12, 0x08, 0x8E);
    idt_set_gate(IRQ13, (uint32_t)__irq13, 0x08, 0x8E);
    idt_set_gate(IRQ14, (uint32_t)__irq14, 0x08, 0x8E);
    idt_set_gate(IRQ15, (uint32_t)__irq15, 0x08, 0x8E);

    desctab_registerIntHandler(0 , isr0 );
    desctab_registerIntHandler(1 , isr1 );
    desctab_registerIntHandler(13, isr13);
    
    __flush_idt((uint32_t)&_idt_ptr);
}

// This is the function that handles all interrupt service routines.
void desctab_isrHandler(registers_t regs){
    statusbar_clearMsg();
    statusbar_printMsg("90Interrupt #");
    statusbar_printNum(regs.int_no, 10);
    if(doesIntHaveErrCode(regs.int_no)){
	statusbar_printMsg(" E:0x");
	statusbar_printNum(regs.err_code, 16);
    }

    statusbar_setInterruptFlag(UNHANDLED);

    bool handled;
    if(_interrupt_handlers[regs.int_no] == NULL){
	handled = false;
    }else
	handled = _interrupt_handlers[regs.int_no](regs);

    if(handled)
	statusbar_setInterruptFlag(HANDLED);
    else{
	console_printWarn("Interrupt #");
	console_printNum(regs.int_no, 10);
	console_print(" raised but unhandled.");
	if(doesIntHaveErrCode(regs.int_no)){
	    console_print(" Error code: 0x");
	    console_printNum(regs.err_code,16);
	}
	console_putChar('\n');
	//console_printWarn("04THIS DAMAGES MY CALM.\n");
	hang();
    }
}

// This is the function that handles all interrupt requests.
void desctab_irqHandler(registers_t regs){
    bool handled = false;
    if(_interrupt_handlers[regs.int_no] != NULL)
	handled = _interrupt_handlers[regs.int_no](regs);

    if(!handled){
	console_printWarn("IRQ");
	console_printNum(regs.int_no-32,10);
	console_print(" raised but unhandled!\n");
	return;
    }
    
    // Now that it's handled, we send an EOI (end of interrupt) to the PICs
    if(regs.int_no >= 40){
	// If this interrupt involved the slave, reset the slave
	outb(0xA0, 0x20);
    }
    
    // Send reset signal to master.
    outb(0x20, 0x20);
}

static bool doesIntHaveErrCode(uint8_t num){
    return (num <= 14 && num >= 10) || num == 8;
}

void desctab_registerIntHandler(uint8_t n, isr_t handler){
    _interrupt_handlers[n] = handler;
}

//================================
// INTERRUPT SERVICE ROUTINES
//================================

static bool isr0(__UNUSED registers_t regs){
    console_printErr("Division by zero exception raised!\n");
    asm volatile("int $1");
    hang();
    return false;
}

static bool isr1 (registers_t regs){
    console_printInfo("Dumping state. Printing passed registers:");

    console_print("\n\teax: 0x");
    console_printNum(regs.eax,16);
    console_print("\tebx: 0x");
    console_printNum(regs.ebx,16);
    console_print("\tecx: 0x");
    console_printNum(regs.ecx,16);
    console_print("\tedx: 0x");
    console_printNum(regs.edx,16);
	
    console_print("\n\tss:  0x");
    console_printNum(regs.ss,16);
    console_print("\tds:  0x");
    console_printNum(regs.ds,16);
    console_print("\tcs:  0x");
    console_printNum(regs.cs,16);

    console_print("\n\tedi: 0x");
    console_printNum(regs.edi,16);
    console_print("\tesi: 0x");
    console_printNum(regs.esi,16);

    console_print("\tebp: 0x");
    console_printNum(regs.ebp,16);
    console_print("\tesp: 0x");
    console_printNum(regs.esp,16);

    console_print("\n\teip: 0x");
    console_printNum(regs.eip,16);
	
    console_print("\teflags: 0x");
    console_printNum(regs.eflags,16);

    console_print("\tuseresp: 0x");
    console_printNum(regs.useresp,16);

    console_putChar('\n');
    return true;
}

static bool isr13(registers_t regs){
    console_printErr("GPF: Error code: 0x");
    console_printNum(regs.err_code,16);

    // print the error code, broken down into human-readable values
    // format: [reserved=FOO, index=BAR, table=BAZ; external/internal]
    console_print(": [reserved=");
    console_printNum((regs.err_code&0xFFFF0000) >> 16, 16);
    console_print(", index=0x");
    console_printNum((regs.err_code&0xFFF8) >> 3, 16);
    console_print(", table=");
	
    uint8_t tbl = (regs.err_code&0x6) >> 1;
    switch(tbl){
    case 0: console_print("GDT"); break;
    case 1:
    case 3: console_print("IDT"); break;
    case 2: console_print("LDT"); break;
    }

    if(regs.err_code & 1)
	console_print("; external]\n");
    else
	console_print("; internal]\n");
	
    console_print("Offending address (CS:EIP) = 0x");
    console_printNum(regs.cs,16);
    console_print(":0x");
    console_printNum(regs.eip,16);
    console_putChar('\n');
	
    hang();
    return false;
}

//================================
// OTHER
//================================

void desctab_initialize(void){
    init_gdt();
    init_idt();
}
