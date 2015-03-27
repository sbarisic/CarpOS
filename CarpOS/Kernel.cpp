#include "CarpSDK.h"
#include "Kernel.h"
#include "GDT.h"
#include "Interrupts.h"
#include "RealTimeClock.h"
#include "Paging.h"

#include <string.h>
#include <intrin.h>

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
		call Kernel::Init;

		/*cli;
		hlt;*/
		jmp $;

not_multiboot:
	}
}

ushort* CPU::VideoMemory = NULL;
char CPU::CPUName[0x40] = { 0 };

void CPU::Init() {
	int CPUInfo[4] = { -1 };
	__cpuid(CPUInfo, 0x80000000);
	uint NexIDs = CPUInfo[0];

	for (uint i = 0x80000000; i <= NexIDs; ++i) {
		__cpuid(CPUInfo, i);
		if  (i == 0x80000002)
			memcpy(CPUName, CPUInfo, sizeof(CPUInfo));
		else if(i == 0x80000003)
			memcpy(CPUName + 16, CPUInfo, sizeof(CPUInfo));
		else if(i == 0x80000004)
			memcpy(CPUName + 32, CPUInfo, sizeof(CPUInfo));
	}

	int SpaceIdx = 0;
	while (CPUName[SpaceIdx] == ' ')
		SpaceIdx++;

	CPU::VideoMemory = (ushort*)0xB8000;
}

void print(const char* Str) {
	__outbytestring(0xE9, (byte*)Str, strlen(Str));

	static int i = 0;
	for (; *Str; Str++, i++) {
		if (*Str == '\n') {
			Kernel::Scroll();
			i = -1;
		} else
			CPU::VideoMemory[24 * 80 + i] = (unsigned char)*Str | 0x700;
	}
}

void print(int i, int base) {
	char buf[256];
	itoa(i, buf, base);
	print(buf);
}

void Kernel::ClearLine(int l) {
	memset(CPU::VideoMemory + 80 * l, NULL, sizeof(ushort) * 80);
}

void Kernel::ClearScreen() {
	for (int i = 0; i < 25; i++)
		ClearLine(i);
}

void Kernel::PrintTime() {
	PrintAt(72, 0, RTC::GetTime());
}

void Kernel::Scroll() {
	for (int i = 1; i < 25; i++) {
		ClearLine(i - 1);
		memcpy(CPU::VideoMemory + 80 * (i - 1), CPU::VideoMemory + 80 * i, sizeof(ushort) * 80);
	}
	ClearLine(24);
}

void Kernel::PrintAt(int x, int y, const char* Str) {
	for (int i = x; *Str; Str++, i++) {
		if (*Str == '\n') {
			Scroll();
			i = -1;
		} else 
			CPU::VideoMemory[24 * y + i] = (unsigned char)*Str | 0x700;
	}
}

template<>
void Kernel::Print(int A) {
	print("(0x");
	print(A, 16);
	print(" - ");
	print(A, 10);
	print(")");
}

template<>
void Kernel::Print(uint A) {
	Print((int)A);
}

template<>
void Kernel::Print(const char* A) {
	print(A);
}

template<>
void Kernel::Print(char* A) {
	print((const char*)A);
}

void Kernel::Init(multiboot_info* Info) {
	CPU::Init();
	ClearScreen();

	Print("CPU: ", CPU::CPUName, "\n");
	Print("Mem: ", Info->high_mem / 1024, "mb\n");
	Print((char*)Info->cmdline, "\n");

	Print("Initializing GDT\n");
	GDTInit();

	Print("Initializing Interrupts\n");
	InterruptsInit();

	Print("Initializing Paging\n");
	Paging::Init(Info->high_mem * 1024);
	
	CPU::VideoMemory = (ushort*)Paging::Map(0xABC000, (uint)CPU::VideoMemory, (80 * 25 * sizeof(short) * 8) / 4096);
	Print("DONE!\n");
}