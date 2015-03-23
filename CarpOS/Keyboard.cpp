#include "Keyboard.h"
#include "Kernel.h"
#include <intrin.h>
#include <string.h>

byte kbdus[128] = {
	0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
	0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
	0,	/* Alt */
  ' ',	/* Space bar */
	0,	/* Caps lock */
	0,	/* 59 - F1 key ... > */
	0,   0,   0,   0,   0,   0,   0,   0,
	0,	/* < ... F10 */
	0,	/* 69 - Num lock*/
	0,	/* Scroll Lock */
	0,	/* Home key */
	0,	/* Up Arrow */
	0,	/* Page Up */
  '-',
	0,	/* Left Arrow */
	0,
	0,	/* Right Arrow */
  '+',
	0,	/* 79 - End key*/
	0,	/* Down Arrow */
	0,	/* Page Down */
	0,	/* Insert Key */
	0,	/* Delete Key */
	0,   0,   0,
	0,	/* F11 Key */
	0,	/* F12 Key */
	0,	/* All other keys are undefined */
};

bool Keyboard::CapsLock = false;

void Keyboard::OnKey(byte Scancode) {
	if (Scancode & 0x80) {
		//print("RELEASED\n");
	} else {
		char Key[2] = { 0, 0 };

		if (Scancode == 0x3A) { // Caps lock
			CapsLock = !CapsLock;
			SetLED((CapsLock ? 1 : 0) << 2);
			return;
		}

		Key[0] = (CapsLock ? -32 : 0) + kbdus[Scancode];
		print(Key);
	}
}

void Keyboard::OutCommand(byte Cmd) {
	__outbyte(0x64, Cmd);
}

void Keyboard::OutData(byte Data) {
	__outbyte(0x60, Data);
}

byte Keyboard::InCommand() {
	return __inbyte(0x64);
}

byte Keyboard::InData() {
	return __inbyte(0x60);
}

void Keyboard::SetLED(byte LED) {
	while (IsBusy());
	OutData(0xED);
	InData();
	OutData(LED);
	InData();
}

bool Keyboard::IsBusy() {
	return InCommand() & 2;
}