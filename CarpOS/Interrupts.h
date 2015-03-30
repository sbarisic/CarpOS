#pragma once
#include "CarpSDK.h"

#define IDTSIZE 256

typedef struct {
	ushort offset0_15;
	ushort select;
	byte unused;
	byte type;
	ushort offset16_31;
} IDTEntry;

typedef struct {
	ushort limit;
	uint base;
} IDTPtr;

typedef struct {
	unsigned int gs, fs, es, ds; // pushed the segs last
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax; // pushed by 'pusha'
	unsigned int int_no, err_code; // our 'push byte #' and ecodes do this
	unsigned int eip, cs, eflags, useresp, ss; // pushed by the processor automatically
} Regs;

#define PIC1 0x20
#define PIC2 0xA0
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1 + 1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2 + 1)

#define INTGATE 0x8E
#define TRAPGATE 0xEF0

#define ISR(i) EXTERN void isr_##i ();

// Default crap
ISR(0) ISR(1) ISR(2) ISR(3)
ISR(4) ISR(5) ISR(6) ISR(7) ISR(8)
ISR(9) ISR(10) ISR(11) ISR(12)
ISR(13) ISR(14) ISR(15) ISR(16)
ISR(17) ISR(18) ISR(19) ISR(20)
ISR(21) ISR(22) ISR(23) ISR(24)
ISR(25) ISR(26) ISR(27) ISR(28)
ISR(29) ISR(30) ISR(31)

// IRQs
ISR(32) ISR(33) ISR(34) ISR(35)
ISR(36) ISR(37) ISR(38) ISR(39)
ISR(40) ISR(41) ISR(42) ISR(43)
ISR(44) ISR(45) ISR(46) ISR(47)

// Syscall
ISR(80)

extern uint TickCount;
void InterruptsInit();