# Declare constants used for creating a multiboot header.
	.set ALIGN,    1<<0        # align loaded modules on page boundaries
	.set MEMINFO,  1<<1             # provide memory map
	.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
	.set MAGIC,    0x1BADB002       # lets bootloader find the header
	.set CHECKSUM, -(MAGIC + FLAGS) # checksum, to prove we are multiboot

	.section .multiboot
	.align 4
	.long MAGIC
	.long FLAGS
	.long CHECKSUM

	.section .bootstrap_stack
stack_bottom:
	.skip 16384 # 16 KiB
stack_top:

.macro ISR_ERRCODE num
	.global __isr\num
__isr\num:
	cli
	push $\num
	jmp __isr_common_stub
.endm

.macro ISR_NOERRCODE num
	.global __isr\num
__isr\num:
	cli
	push $0
	push $\num
	jmp __isr_common_stub
.endm

ISR_NOERRCODE  0
ISR_NOERRCODE  1
ISR_NOERRCODE  2
ISR_NOERRCODE  3
ISR_NOERRCODE  4
ISR_NOERRCODE  5
ISR_NOERRCODE  6
ISR_NOERRCODE  7
ISR_ERRCODE    8
ISR_NOERRCODE  9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31
	
__isr_common_stub:
	pushal

	movw %ds, %ax
	push %eax	# Save the data segment descriptor

	/*
	# Load the kernel data segment descriptor
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	*/

	call desctab_isrHandler

	pop %eax
	/*
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	*/
	
	popal
	add $8, %esp
	iret

.macro IRQ num int_num
	.global __irq\num
__irq\num:
	cli
	push $0
	push $\int_num
	jmp __irq_common_stub
.endm

IRQ 0  32
IRQ 1  33
IRQ 2  34
IRQ 3  35
IRQ 4  36
IRQ 5  37
IRQ 6  38
IRQ 7  39
IRQ 8  40
IRQ 9  41
IRQ 10 42
IRQ 11 43
IRQ 12 44
IRQ 13 45
IRQ 14 46
IRQ 15 47

__irq_common_stub:
	pushal

	movw %ds, %ax
	push %eax	# Save the data segment descriptor

	/*
	# Load the kernel data segment descriptor
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	*/

	call desctab_irqHandler

	pop %ebx
	/*
	mov %bx, %ds
	mov %bx, %es
	mov %bx, %fs
	mov %bx, %gs
	*/
	
	popal
	add $8, %esp
	iret

# ================================================================
# All functions written in ASM live here
# ================================================================

# __flush_gdt takes a GDT descriptor as its argument and loads the GDT.
	.global __flush_gdt
	.type __flush_gdt, @function
__flush_gdt:
	movl 4(%esp), %eax
	lgdt (%eax)

	# 0x10 is the offset in the gdt to our data segment. Load all segment
	# data selectors..
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss

	#jmp 0x8:flush    # 0x8 is the offset to our code segment: Far jump!
	ljmp $0x8, $flush
flush:
	ret
	.size __flush_gdt, .-__flush_gdt

# __flush_idt takes an IDT descriptor as its argument and loads the IDT.
	.global __flush_idt
	.type __flush_idt, @function
__flush_idt:
	movl 4(%esp), %eax
	lidt (%eax)
	ret
	.size __flush_idt, .-__flush_idt

.macro GET_CRN i
	.global __getCR\i
	.type __getCR\i, @function
__getCR\i:
	movl %cr\i, %eax
	ret
	.size __getCR\i, .-__getCR\i
.endm

GET_CRN 0
GET_CRN 1
GET_CRN 2
GET_CRN 3
GET_CRN 4

# ================================================================
# _start is the entry point for our kernel
# ================================================================	
	.global _start, label_hang
	.type _start, @function
_start:
	movl $stack_top, %esp
	
	# multiboot puts the magic number 2BADB002 in %eax and
	# puts the address of the memory table thing (multiboot_info_t)
	# in %ebx, so we need to pass these to kernel_main on the stack.
	pushl %eax
	pushl %ebx
	# DO NOT MODIFY THE STACK AFTER THIS POINT.

	#cli
	call kernel_main

label_hang:
	cli
	hlt
	jmp hang
	.size _start, .-_start
