SECTION .text

global _GDTFlush
_GDTFlush:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp 0x8:_EndFlush
_EndFlush:
	ret