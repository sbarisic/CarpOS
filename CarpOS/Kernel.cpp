#include "CarpSDK.h"
#include "Kernel.h"
#include "GDT.h"
#include "Interrupts.h"
#include "RealTimeClock.h"

#include <string.h>
#include <intrin.h>

const char* cpuname() {
	char CPUBrand[0x40] = { 0 };

	int CPUInfo[4] = { -1 };
	__cpuid(CPUInfo, 0x80000000);
	uint NExIDs = CPUInfo[0];

	for (uint i = 0x80000000; i <= NExIDs; ++i) {
		__cpuid(CPUInfo, i);
		if  (i == 0x80000002)
			memcpy(CPUBrand, CPUInfo, sizeof(CPUInfo));
		else if( i == 0x80000003 )
			memcpy(CPUBrand + 16, CPUInfo, sizeof(CPUInfo));
		else if( i == 0x80000004 )
			memcpy(CPUBrand + 32, CPUInfo, sizeof(CPUInfo));
	}

	return CPUBrand;
}

EXTERN void multiboot_entry() {
#define FLAGS ((1 << 16) | (1 << 2) | (1 << 1) | (1 << 0))
#define LOAD_ADDR (0x00101000)
#define DATA_SIZE (1024 * 64)
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
		dd(LOAD_ADDR + 0x30); // entry_addr
		dd(1); // mode_type
		dd(80); // width
		dd(25); // height
		dd(0); // depth
		call KMain;
		ret;
	}
}

void KMain() {
	void* StackPtr = (void*)(LOAD_ADDR + BSS_SIZE + DATA_SIZE);

	ASM {
		mov edx, multiboot_entry;
		mov edx, 0x2BADB002;
		cmp eax, edx;
		jne not_multiboot;
		mov esp, [StackPtr];
		xor ecx, ecx;
		push ecx;
		popf;
		push ebx;
		call main;

		/*cli;
		hlt;*/
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

void print_time() {
	print_at(72, 0, RTC::GetTime());
}

void scroll() {
	for (int i = 1; i < 25; i++) {
		clear_line(i - 1);
		memcpy(VidMem + 80 * (i - 1), VidMem + 80 * i, sizeof(ushort) * 80);
	}
	clear_line(24);
}

void print_at(int x, int y, const char* Str) {
	for (int i = x; *Str; Str++, i++) {
		if (*Str == '\n') {
			scroll();
			i = -1;
		} else 
			VidMem[24 * y + i] = (unsigned char)*Str | 0x700;
	}
}

void print(const char* Str) {
	static int i = 0;
	for (; *Str; Str++, i++) {
		if (*Str == '\n') {
			scroll();
			i = -1;
		} else 
			VidMem[24 * 80 + i] = (unsigned char)*Str | 0x700;
	}
}

void print(int i, int base) {
	char buf[256];
	itoa(i, buf, base);
	print(buf);
}

void main(multiboot_info* Info) {
	clear_screen();
	print("CPU: ");
	print(cpuname());
	print("\n");
	print("Mem: ");
	print(Info->high_mem / 1024, 10);
	print("mb\n");
	print("Executing ");
	print((char*)Info->cmdline);
	print("\n");
	/*print("multiboot_entry @ 0x");
	print((int)&multiboot_entry);
	print("\n");*/

	//print_at(62, 0, "<INSERT_TIME_HERE>");

	print("Initializing GDT\n");
	GDTInit();

	print("Initializing Interrupts\n");
	InterruptsInit();

	ASM int 80;
}