
#include "CarpSDK.h"

#include "Kernel.h"
#include "GDT.h"

NAKED NORETURN void __multiboot_entry__() {
	ASM {
multiboot_header:
		dd(0x1BADB002); // magic
		dd(1 << 16); // flags
		dd(-(0x1BADB002 + (1 << 16))); // checksum
		dd(0x00101000); // header_addr
		dd(0x00101000); // load_addr
		dd(0x0010200F); // load_end_addr
		dd(0x0010200F); // bss_end_addr
		dd(0x00101030); // entry_addr
		dd(0x00000000); // mode_type
		dd(0x00000000); // width
		dd(0x00000000); // height
		dd(0x00000000); // depth

kernel_entry:
		mov esp, KERNEL_STACK;
		xor ecx, ecx;
		push ecx;
		popf;
		push ebx;
		push eax;
		call main;
		jmp $;
	}
}

ushort* const VidMem = (ushort*)0xB8000;

void clear_screen() {
	for (int i = 0; i < 80 * 35; i++)
		VidMem[i] = 0;
}

void print(char* Str) {
	int i = 0;
	for (char* Ch = Str, i = 0; *Ch; Ch++, i++)
		VidMem[i] = (unsigned char)*Ch | 0x700;
}

void main(ulong Magic, multiboot_info* Info) {
	if (Magic != 0x2BADB002) {
		print("Invalid magic");
		return;
	}

	//GDTInit();

	clear_screen();
	print((char*)Info->boot_loader_name);

	//print("DONE!     ");
}