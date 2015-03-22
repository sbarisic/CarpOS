#include "Interrupts.h"

IDTEntry IDT[IDTSIZE];
IDTPtr IDTP;

void IDTInitDesc(ushort Select, uint Offset, ushort Type, IDTEntry* Entry) {
	Entry->offset0_15 = (Offset & 0xFFFF);
	Entry->select = Select;
	Entry->type = Type;
	Entry->offset16_31 = (ushort)((Offset & 0xFFFF0000) >> 16);
}

void InterruptsInit() {
	IDTP.limit = sizeof(IDTEntry) * IDTSIZE - 1;
	IDTP.base = (uint)&IDT;

	//IDTInitDesc(0x8,

	ASM lidt IDTP;
}