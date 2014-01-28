# moss

## about
Moss is an i386 custom RTOS I'm writing as a hobby! :D I started writing it on 26 Jul 2013, and have been working on it on and off ever since.

## to do

Shorter term:
	+Proper time and date (probably Epoch-style)
	+Proper handling of all keypresses, including multi-byte scancodes
	+Memory management
		+Paging
	+General code cleanup

	*version control system like GitHub or Bitbucket?

Longer term:
	+Userspace
	+Multitasking
	+Filesystem
	+Command line

## changelog

Moss v0.3:
	+Complete rewrite of all existing code (except multiboot)
	-Multiboot memory map reading compatability (to be added with memory
	 management)
	+Working interrupt handlers
		+interrupt handler registration system
		+Debugging capabilities, with interrupt #1
	+Working IRQs
		+Working PIT (Programmable Interval Timer)
			+Working interval scheduling handler system
		+Rudimentary keyboard input
			+Non-busy-loops when waiting for keypresses
			*Probably buggy
	+CMOS RTC reading
		*Still need sanity checks and such.

Moss v0.2:
	+Multiboot memory map reading compatability
	+Infrastructure for interrupt handlers
	+Miscellaneous cleanup

Moss v0.1:
	+VGA driver
		+scrolling
		+colors
		+newlines
	+integer-printing utility
	+status bar
	+actual booting code

Legend:
	+added
	-removed
	*note

## conventions
	variables are lowercase with scores
	fields begin with a score
	assembly functions begin with two scores
	functions follow the form filename_functionNameInCamelCase
	macros follow the form FILENAME_MACRO_NAME
	constants follow the form kConstantName
