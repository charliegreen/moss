# moss

## about
Moss is an i386 custom RTOS I'm writing as a hobby! :D I started writing it on 26 Jul 2013, and have been working on it on and off ever since. Thanks to the help from [OSDev](http://wiki.osdev.org/Main_Page)!

## building
To build, you'll first need an [i586 cross-compiler](http://wiki.osdev.org/GCC_Cross-Compiler), and all of its handy doodads, in your PATH somewhere. You'll also need to install QEMU, or set up a similar virtual machine like Bochs, to run it (unless you want to boot on actual hardware, which I do not recommend; I'm still working out how to get it to fully boot on my laptop). The Makefile should take care of the rest for you.

## to do

Shorter term:

* Proper time and date (probably Epoch-style)
* Proper handling of all keypresses, including multi-byte scancodes
* Memory management
	* Paging
* General code cleanup

Longer term:

* Userspace
* Multitasking
* Filesystem
* Command line

## changelog

### Moss v0.4 (current revision; unstable):

* Rudimentary memory management
	* kmalloc() implemented as a allocation-by-placement-address pointer-incrementing system
	* various tools and such for determining memory layout
* Paging code (4-KiB page size; non-PAE)
	* working (and helpful!) page-fault handler
* Some functions helpful for debugging
* Performance boosts everywhere and random minor bugfixes

### Moss v0.3:

* Complete rewrite of all existing code (except multiboot)
* Removed Multiboot memory map reading compatability (to be added with memory management)
* Working interrupt handlers
	* interrupt handler registration system
		* Debugging capabilities, with interrupt #1
	* Working IRQs
		* Working PIT (Programmable Interval Timer)
			* Working interval scheduling handler system
		* Rudimentary keyboard input
			* (probably buggy) Non-busy-loops when waiting for keypresses
	* CMOS RTC reading (still need sanity checks and such)

### Moss v0.2:

* Multiboot memory map reading compatability
* Infrastructure for interrupt handlers
* Miscellaneous cleanup

### Moss v0.1:

* VGA driver
	* scrolling
	* colors
	* newlines
* integer-printing utility
* status bar
* actual booting code

## conventions
* variables are lowercase with underscores or camel case (<code>variableName</code>)
* fields (global variables, when we're using C) begin with an underscore (<code>_fieldName</code>)
* assembly functions begin with two underscores (<code>__assemblyFunction</code>)
* functions follow the form <code>filename_functionNameInCamelCase</code>
* macros follow the form <code>FILENAME_MACRO_NAME</code>
* constants follow the form <code>kConstantName</code>
