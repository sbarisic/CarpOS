#pragma once
#include "CarpSDK.h"

typedef struct {
	ushort attributes;
	byte winA, winB;
	ushort granularity;
	ushort winsize;
	ushort segmentA, segmentB;
	uint realFctPtr;
	ushort pitch;
	ushort Xres, Yres;
	byte Wchar, Ychar, planes, bpp, banks;
	byte memory_model, bank_size, image_pages;
	byte reserved0;
	byte red_mask, red_position;
	byte green_mask, green_position;
	byte blue_mask, blue_position;
	byte rsv_mask, rsv_position;
	byte directcolor_attributes;
	uint physbase;
	uint reserved1;
	ushort reserved2;
} vbe_info_t;

typedef struct {
	uint mod_start;
	uint mod_end;
	uint cmdline;
	uint pad;
} multiboot_module;

typedef struct {
	u32 flags;
	u32 low_mem;
	u32 high_mem;
	u32 boot_device;
	u32 cmdline;
	u32 mods_count;
	u32 mods_addr;
	struct {
		u32 num;
		u32 size;
		u32 addr;
		u32 shndx;
	} elf_sec;
	ulong mmap_length;
	ulong mmap_addr;
	ulong drives_length;
	ulong drives_addr;
	ulong config_table;
	ulong boot_loader_name;
	ulong apm_table;
	ulong vbe_control_info;
	ulong vbe_mode_info;
	ulong vbe_mode;
	ulong vbe_interface_seg;
	ulong vbe_interface_off;
	ulong vbe_interface_len;
} multiboot_info;

class CPU {
public:
	static void Init();

	static char CPUName[0x40];
	static ushort* VideoMemory;
};	

class Kernel {
public:
	NORETURN static void Init();
	static void CarpScreenOfDeath();
	static void Terminate();

	template<typename T> static void Print(T A);

	template<typename TA, typename TB>
	static void Print(TA A, TB B) {
		Print(A); Print(B);
	}

	template<typename TA, typename TB, typename TC>
	static void Print(TA A, TB B, TC C) {
		Print(A); Print(B); Print(C);
	}

	template<typename TA, typename TB, typename TC, typename TD>
	static void Print(TA A, TB B, TC C, TD D) {
		Print(A); Print(B); Print(C); Print(D);
	}

	template<typename TA, typename TB, typename TC, typename TD, typename TE>
	static void Print(TA A, TB B, TC C, TD D, TE E) {
		Print(A); Print(B); Print(C); Print(D); Print(E);
	}

	template<typename TA, typename TB, typename TC, typename TD, typename TE, typename TF>
	static void Print(TA A, TB B, TC C, TD D, TE E, TF F) {
		Print(A); Print(B); Print(C); Print(D); Print(E); Print(F);
	}

	/*static void Scroll();
	static void ClearLine(int L);
	static void ClearScreen();*/
};

/*void print(const char* Str);
void print(int i, int base = 16);*/

NORETURN void KMain();
void main(multiboot_info* Info);
extern multiboot_info* Info;