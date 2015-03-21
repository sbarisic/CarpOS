#include "CarpSDK.h"
#include "Kernel.h"
#include "GDT.h"
#include "Interrupts.h"

#include <string.h>

EXTERN void multiboot_entry() {
#define FLAGS ((1 << 16) | (1 << 2) | (1 << 1) | (1 << 0))
#define LOAD_ADDR (0x00101000)
#define DATA_SIZE (1024 * 8)
#define BSS_SIZE (1024 * 64)

	ASM {
		//multiboot_header:
		dd(0x1BADB002); // magic
		dd(FLAGS); // flags
		dd(-(0x1BADB002 + FLAGS)); // checksum
		dd(LOAD_ADDR); // header_addr
		dd(LOAD_ADDR); // load_addr
		dd(LOAD_ADDR + DATA_SIZE); // load_end_addr
		dd(LOAD_ADDR + BSS_SIZE); // bss_end_addr
		dd(0x00101030); // entry_addr
		dd(1); // mode_type
		dd(80); // width
		dd(25); // height
		dd(0); // depth
		call KMain;
		ret;
	}
}

void KMain() {
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
		cli;
		hlt;
		jmp $;

not_multiboot:
	}
}

ushort* const VidMem = (ushort*)0xB8000;

void clear_line(int l) {
	memset(VidMem + 80 * l, NULL, sizeof(ushort) * 80);
}

void clear_screen() {
	for (int i = 0; i < 25; i++)
		clear_line(i);
}

void print(const char* Str) {
	for (int i = 1; i < 25; i++) {
		clear_line(i - 1);
		memcpy(VidMem + 80 * (i - 1), VidMem + 80 * i, sizeof(ushort) * 80);
	}
	clear_line(24);

	for (int i = 0; *Str; Str++, i++)
		VidMem[24 * 80 + i] = (unsigned char)*Str | 0x700;
}

void main(multiboot_info* Info) {
	clear_screen();
	
	print("Initializing GDT");
	GDTInit();

	print("Initializing Interrupts");
	InterruptsInit();

	print("Hello Carp!");
	print("How are you, Carp?");
	print("Carp.");
}