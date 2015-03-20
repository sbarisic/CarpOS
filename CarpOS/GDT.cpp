#include "GDT.h"
#include "string.h"

void InitGDTDesc(u32 Base, u32 Limit, u8 Access, u8 Other, GDTDesc* Desc) {
	Desc->lim0_15 = Limit & 0xFFFF;
	Desc->base0_15 = Base & 0xFFFF;
	Desc->base16_23 = (Base & 0xFF0000) >> 16;
	Desc->access = Access;
	Desc->lim16_19 = (Limit & 0xF0000) >> 16;
	Desc->other = Other & 0xF;
	Desc->base24_31 = (Base & 0xFF000000) >> 24;
}

GDTDesc KGDT[GDTSIZE];
GDTR KGDTR;

void GDTInit() {
	/// initialize gdt segments 
	InitGDTDesc(0x0, 0x0, 0x0, 0x0, &KGDT[0]);
	InitGDTDesc(0x0, 0xFFFFF, 0x9B, 0x0D, &KGDT[1]);    // code 
	InitGDTDesc(0x0, 0xFFFFF, 0x93, 0x0D, &KGDT[2]);    // data 
	InitGDTDesc(0x0, 0x0, 0x97, 0x0D, &KGDT[3]);        // stack 

	InitGDTDesc(0x0, 0xFFFFF, 0xFF, 0x0D, &KGDT[4]);    // ucode 
	InitGDTDesc(0x0, 0xFFFFF, 0xF3, 0x0D, &KGDT[5]);    // udata 
	InitGDTDesc(0x0, 0x0, 0xF7, 0x0D, &KGDT[6]);        // ustack 

	KGDTR.limit = GDTSIZE * 8;
	KGDTR.base = GDTBASE;

	memcpy((char*)KGDTR.base, (char*)KGDT, KGDTR.limit);

	ASM {
		lgdt KGDTR;
	}
}

//*/