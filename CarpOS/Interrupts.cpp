#include "Kernel.h"
#include "Interrupts.h"
#include "Keyboard.h"
#include <string.h>
#include <intrin.h>

IDTEntry IDT[IDTSIZE];
IDTPtr IDTP;

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

void num(int i) {
	print("0x");
	print(i);
	print(" - ");
	print(i, 10);
}

EXTERN void empty_handler(Regs* R) {
	if (R->int_no < 32) {
		if (R->int_no < 20)
			print(IntNames[R->int_no]);
		else
			print(IntNames[20]);
		print("; INT: ");
		num(R->int_no);
		print("; ERR: ");
		num(R->err_code);
		print("\n");

		ASM {
			cli;
			hlt;
			jmp $;
		}
	} else if (R->int_no >= 32 && R->int_no < 48) {
		int IRQ = R->int_no - 32;

		if (IRQ == 0) {
			print_time();
		} else if (IRQ == 1) {
			Keyboard::OnKey(Keyboard::InData());
		} else {
			print("IRQ: ");
			print(IRQ);
			print("\n");
		}

		__outbyte(0x20, 0x20);
		if (IRQ >= 8)
			__outbyte(0xA0, 0x20);

	} else if (R->int_no == 80) {
		print("SYSCALL\n");
	} else {
		print("INT: ");
		num(R->int_no);
		print("\n");
	}
}

void IRQRemap() {
	__outbyte(0x20, 0x11);
	__outbyte(0xA0, 0x11);
	__outbyte(0x21, 0x20);
	__outbyte(0xA1, 0x28);
	__outbyte(0x21, 0x04);
	__outbyte(0xA1, 0x02);
	__outbyte(0x21, 0x01);
	__outbyte(0xA1, 0x01);
	__outbyte(0x21, 0x0);
	__outbyte(0xA1, 0x0);
}

void InterruptsInit() {
	IDTP.limit = (sizeof(IDTEntry) * IDTSIZE) - 1;
	IDTP.base = (uint)&IDT;
	memset(&IDT, NULL, sizeof(IDTEntry) * IDTSIZE);

#define DEFAULT_ISR(n) IDTInitDesc(0x8, &isr_##n, INTGATE, n)
	// Default
	DEFAULT_ISR(0); DEFAULT_ISR(1); DEFAULT_ISR(2); DEFAULT_ISR(3);
	DEFAULT_ISR(4); DEFAULT_ISR(5); DEFAULT_ISR(6); DEFAULT_ISR(8);
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

	__outbyte(0x70, (1 << 7) | (0x0));

	ASM {
		lidt IDTP;
		sti;
	}
}