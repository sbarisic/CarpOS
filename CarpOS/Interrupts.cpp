#include "Kernel.h"
#include "Interrupts.h"
#include "Keyboard.h"
#include "Paging.h"
#include <string.h>
#include <intrin.h>

IDTEntry IDT[IDTSIZE];
IDTPtr IDTP;
uint TickCount;

const char* IntNames[] = {
	"Div by zero",
	"Debug",
	"Non maskable interrupt",
	"Breakpoint",
	"Overflow",
	"Out of bounds",
	"Invalid opcode",
	"No coprocessor",
	"Double fault",
	"Coprocessor seg overrun",
	"Invalid task state segment",
	"Segment not present",
	"Stack fault",
	"General protection fault",
	"Page fault",
	"Unknown interrupt",
	"Math fault",
	"Alignment check",
	"Machine check",
	"SIMD Floating-point"
	"Reserved"
};

void IDTInitDesc(ushort Select, void* Handler, byte Type, int IDTN) {
	uint Offset = (uint)Handler;
	IDTEntry* Entry = &IDT[IDTN];
	Entry->offset0_15 = (Offset & 0xFFFF);
	Entry->select = Select;
	Entry->unused = 0;
	Entry->type = Type;
	Entry->offset16_31 = (ushort)((Offset & 0xFFFF0000) >> 16);
}

EXTERN void empty_handler(Regs* R) {
	if (R->int_no < 32) {
		Kernel::Print("\n");
		if (R->int_no < 20)
				Kernel::Print(IntNames[R->int_no]);
			else
				Kernel::Print(IntNames[20]);
		Kernel::Print("; INT ", R->int_no, " ");

		if (R->int_no == 14) {
			uint PFAddress;
			ASM {
				mov eax, cr2;
				mov PFAddress, eax;
			}
			Kernel::Print("@ ", PFAddress, ";\n");

			bool Present = (R->err_code & 0x1) != 0;
			bool RW = (R->err_code & 0x2) != 0;
			bool UserMode = (R->err_code & 0x4) != 0;
			//bool Reserved = (R->err_code & 0x8);

			if (UserMode)
				Kernel::Print("User ");
			else
				Kernel::Print("Kernel ");

			Kernel::Print("tried to ");
			if (RW)
				Kernel::Print("write ");
			else
				Kernel::Print("read ");

			if (Present)
				Kernel::Print("causing a protection fault");
			else
				Kernel::Print("on a non-present page entry");
			Kernel::Print("\n");
		}

		Kernel::CarpScreenOfDeath();
	} else if (R->int_no >= 32 && R->int_no < 48) {
		int IRQ = R->int_no - 32;
		if (IRQ >= 8)
			__outbyte(PIC2, 0x20);
		__outbyte(PIC1, 0x20);

		if (IRQ == 0) {
			TickCount++;
			Kernel::PrintTime();
		} else if (IRQ == 1) {
			Keyboard::OnKey(Keyboard::InData());
		} else {
			Kernel::Print("IRQ: ", IRQ, "\n");
		}
	} else if (R->int_no == 80) {
		Kernel::Print("SYSCALL\n");
	} else {
		Kernel::Print("INT: ", R->int_no, "\n");
	}
}

void IRQRemap() {
	__outbyte(PIC1, 0x11);
	__outbyte(PIC2, 0x11);
	__outbyte(PIC1_DATA, 0x20);
	__outbyte(PIC2_DATA, 0x28);
	__outbyte(PIC1_DATA, 0x04);
	__outbyte(PIC2_DATA, 0x02);
	__outbyte(PIC1_DATA, 0x01);
	__outbyte(PIC2_DATA, 0x01);
	__outbyte(PIC1_DATA, 0x0);
	__outbyte(PIC2_DATA, 0x0);
}

void IRQSetMask(byte IRQLine) {
	ushort Port;
	if (IRQLine < 8)
		Port = PIC1_DATA;
	else {
		Port = PIC2_DATA;
		IRQLine -= 8;
	}
	__outbyte(Port, __inbyte(Port) | (1 << IRQLine));
}

void IRQClearMask(byte IRQLine) {
	ushort Port;
	if (IRQLine < 8)
		Port = PIC1_DATA;
	else {
		Port = PIC2_DATA;
		IRQLine -= 8;
	}
	__outbyte(Port, __inbyte(Port) & ~(1 << IRQLine));
}

void InterruptsInit() {
	TickCount = 0;

	IDTP.limit = (sizeof(IDTEntry) * IDTSIZE) - 1;
	IDTP.base = (uint)&IDT;
	memset(&IDT, NULL, sizeof(IDTEntry) * IDTSIZE);

#define DEFAULT_ISR(n) IDTInitDesc(0x8, &isr_##n, INTGATE, n)
	// Default
	DEFAULT_ISR(0); DEFAULT_ISR(1); DEFAULT_ISR(2); DEFAULT_ISR(3);
	DEFAULT_ISR(4); DEFAULT_ISR(5); DEFAULT_ISR(6); DEFAULT_ISR(7);
	DEFAULT_ISR(8);
	DEFAULT_ISR(9); DEFAULT_ISR(10); DEFAULT_ISR(11); DEFAULT_ISR(12);
	DEFAULT_ISR(13); DEFAULT_ISR(14); DEFAULT_ISR(15); DEFAULT_ISR(16);
	DEFAULT_ISR(17); DEFAULT_ISR(18); DEFAULT_ISR(19); DEFAULT_ISR(20);
	DEFAULT_ISR(21); DEFAULT_ISR(22); DEFAULT_ISR(23); DEFAULT_ISR(24);
	DEFAULT_ISR(25); DEFAULT_ISR(26); DEFAULT_ISR(27); DEFAULT_ISR(28);
	DEFAULT_ISR(29); DEFAULT_ISR(30); DEFAULT_ISR(31);
	// Syscall
	DEFAULT_ISR(80);
	// IRQs
	DEFAULT_ISR(32); DEFAULT_ISR(33); DEFAULT_ISR(34); DEFAULT_ISR(35);
	DEFAULT_ISR(36); DEFAULT_ISR(37); DEFAULT_ISR(38); DEFAULT_ISR(39);
	DEFAULT_ISR(40); DEFAULT_ISR(41); DEFAULT_ISR(42); DEFAULT_ISR(43);
	DEFAULT_ISR(44); DEFAULT_ISR(45); DEFAULT_ISR(46); DEFAULT_ISR(47);
#undef DEFAULT_ISR

	IRQRemap();
	for (byte i = 0; i < 16; i++)
		IRQClearMask(i);
	__outbyte(0x70, (1 << 7) | (0x0));

	ASM {
		lidt IDTP;
		sti;
	}
}