#include "CarpSDK.h"
#include "Kernel.h"
#include "GDT.h"

#define FLAGS ((1 << 16) | (1 << 2) | (1 << 1) | (1 << 0))
#define LOAD_ADDR (0x00101000)
#define DATA_SIZE (1024 * 8)
#define BSS_SIZE (1024 * 64)

EXTERN NORETURN void multiboot_entry() {
	ASM {
multiboot_header:
		dd(0x1BADB002); // magic
		dd(FLAGS); // flags
		dd(-(0x1BADB002 + FLAGS)); // checksum
		dd(LOAD_ADDR); // header_addr
		dd(LOAD_ADDR); // load_addr
		dd(LOAD_ADDR + DATA_SIZE); // load_end_addr
		dd(LOAD_ADDR + BSS_SIZE); // bss_end_addr
		dd(0x00101030); // entry_addr
		dd(1); // mode_type
		dd(40); // width
		dd(25); // height
		dd(0); // depth
		jmp KMain;
	}
}

NORETURN void KMain() {
	ASM {
		mov edx, multiboot_entry;
		mov edx, 0x2BADB002;
		cmp eax, edx;
		jne not_multiboot;

		mov esp, KERNEL_STACK;
		xor ecx, ecx;
		push ecx;
		popf;
		push ebx;
		//push eax;
		call main;
		jmp $;

not_multiboot:
	}
}

ushort* const VidMem = (ushort*)0xB8000;

void clear_screen() {
	for (int i = 0; i < 80 * 35; i++)
		VidMem[i] = 0;
}

int Y = 0;

void print(const char* Str) {
	for (int i = 0; *Str; Str++, i++)
		VidMem[Y * 80 + i] = (unsigned char)*Str | 0x700;
	Y++;
}

void main(multiboot_info* Info) {
	GDTInit();
	clear_screen();

	print("Hello Carp!");
	print("How are you, Carp?");
}