#ifndef KERNEL_H
#define KERNEL_H

#define KERNEL_INIT(task, etc) do{ console_print("Initializing "#task"..");\
	task ## _initialize();\
	console_print("20ok70 ");		\
	etc;					\
	console_print("\n"); }while(0)

void kernel_main(uint32_t a, uint32_t magic);

#endif
