#pragma once
#include "CarpSDK.h"

#define dd(x) \
	ASM _emit	(x)			& 0xff	\
	ASM _emit	(x) >> 8	& 0xff	\
	ASM _emit	(x) >> 16	& 0xff	\
	ASM _emit	(x) >> 24	& 0xff 

#define dw(x) \
	ASM _emit (x) & 0xff \
	ASM _emit (x) >> 8 & 0xff

#define KERNEL_START	100400
#define KERNEL_STACK	0x103fff
#define KERNEL_LENGTH	0x200F

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

NORETURN void KMain();
void main(multiboot_info* Info);