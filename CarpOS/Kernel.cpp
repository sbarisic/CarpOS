#include "CarpSDK.h"
#include "Kernel.h"
#include "GDT.h"
#include "Interrupts.h"
#include "RealTimeClock.h"
#include "Paging.h"
#include "Memory.h"
#include "Video.h"

#include <string.h>
#include <intrin.h>

#define LOAD_ADDR (0x00101000)
#define DATA_ADDR (0)
#define BSS_ADDR (0)
#define KERNEL_START LOAD_ADDR
#define KERNEL_END 0xA00000

EXTERN void multiboot_entry() {

#define FLAG_AOUT_KLUDGE (1 << 16)
#define FLAG_3 (1 << 3)
#define FLAG_VIDEO (1 << 2)
#define FLAG_MEMORY_INFO (1 << 1)
#define FLAG_PAGE_ALIGN (1 << 0)

#define FLAGS (FLAG_AOUT_KLUDGE | FLAG_MEMORY_INFO | FLAG_PAGE_ALIGN | FLAG_VIDEO)

	ASM {
		//multiboot_header:
		dd(0x1BADB002);				// magic
		dd(FLAGS);					// flags
		dd(-(0x1BADB002 + FLAGS));	// checksum
		dd(LOAD_ADDR);				// header_addr
		dd(LOAD_ADDR);				// load_addr
		dd(DATA_ADDR);				// load_end_addr
		dd(BSS_ADDR);				// bss_end_addr
		dd(LOAD_ADDR + 0x30);		// entry_addr
		dd(0);						// mode_type
		dd(800);					// width
		dd(600);					// height
		dd(32);						// depth
ENTRY:

		jmp KMain;
	}
}

multiboot_info* Info;
uint StackSize;
void* StackPtr;
//uint KERNEL_END;

NAKED NORETURN void KMain() {
	ASM {
		mov edx, multiboot_entry;
		mov edx, 0x2BADB002;
		cmp eax, edx;
		jne not_multiboot;
		cli;
		mov Info, ebx;
		mov Memory::PlacementAddr, KERNEL_END;
		mov StackSize, KB(10);
		push 0;
		popf;
	}

	StackPtr = (void*)((uint)Memory::KAlloc(StackSize) + StackSize);

	ASM {
		mov esp, StackPtr;
		jmp Kernel::Init;
not_multiboot:
		ret;
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
	Video::DrawImage((Pixel*)((multiboot_module*)Info->mods_addr)[0].mod_start);

	static int i = 0;
	for (; *Str; Str++, i++) {
		if (*Str == '\n') {
			Video::ScrollText();
			Kernel::Scroll();
			i = -1;
		} else {
			Video::SetChar(i, 74, *Str);
			CPU::VideoMemory[24 * 80 + i] = (unsigned char)*Str | 0x700;
		}
	}

	Video::DisplayText();
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

void Kernel::Scroll() {
	for (int i = 1; i < 25; i++) {
		ClearLine(i - 1);
		memcpy(CPU::VideoMemory + 80 * (i - 1), CPU::VideoMemory + 80 * i, sizeof(ushort) * 80);
	}
	ClearLine(24);
}

template<>
void Kernel::Print(bool B) {
	if (B)
		print("true");
	else
		print("false");
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

NAKED NORETURN void Kernel::Init() {
	CPU::Init();
	Video::Init();

	if (Video::Initialized && Info->mods_count > 0) {
		Video::Font = (Pixel*)((multiboot_module*)Info->mods_addr)[2].mod_start;
		Video::CharW = Video::CharH = 8;
		Video::ClearText();
		Video::DrawImage((Pixel*)((multiboot_module*)Info->mods_addr)[0].mod_start);
		//Terminate();
	}

	Print("CarpOS initializing!\n");
	Print("Multiboot entry @ ", (uint)&multiboot_entry, "\n");

	if (Video::Initialized) 
		Print("Video memory @ ", (uint)Video::Mem, "\n");
	else
		Print("WARNING: Video not initialized!\n");

	Print("CPU: ", CPU::CPUName, "\n");
	Print("Mem lower: ", Info->low_mem, "kb; ", Info->low_mem / 1024, "mb\n");
	Print("Mem upper: ", Info->high_mem, "kb; ", Info->high_mem / 1024, "mb\n");
	Print((char*)Info->cmdline, "\n");

	Kernel::Print("Initializing GDT\n");
	GDTInit();

	Print("Initializing Interrupts\n");
	InterruptsInit();

	Print("Initializing Paging\n");
	Paging::Init(Info->high_mem * 1024);

	Print("Initializing Memory\n");
	Memory::Init((void*)(Memory::PlacementAddr + 0x10000));

	Print("Done!\n");
	Terminate();
}

void Kernel::CarpScreenOfDeath() {
	if (Video::Initialized) {
		Video::DrawImage((Pixel*)((multiboot_module*)Info->mods_addr)[1].mod_start);
		Video::DisplayText();
	}
	Terminate();
}

void Kernel::Terminate() {
	while (true);

	ASM {
		/*cli;
		hlt;*/
		jmp $;
	}
}

void Kernel::Wait(uint Ticks) {
	uint CurTick = TickCount;
	while (TickCount < CurTick + Ticks)
		;
}