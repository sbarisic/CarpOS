#pragma once
#include <intrin.h>

//#pragma comment(lib, "CarpLib.lib")

//#define __STDC__ 1
#ifndef NULL
#define NULL 0
#endif

#define _PDCLIB_restrict

#define ALIGN(n) __declspec(align(n))
#define EXTERN extern "C"
#define EXPORT EXTERN __declspec(dllexport) // exported from DLL
#define IMPORT EXTERN __declspec(dllimport) // imported from DLL
#define NAKED __declspec(naked) // no prolog or epilog code added
#define NORETURN __declspec(noreturn)


// Assembler crap
#define ASM __asm
#define BREAKPOINT ASM xchg bx, bx

#define db(x) ASM _emit (x)

#define dw(x) \
	ASM _emit (x) & 0xff \
	ASM _emit ((x) >> 8) & 0xff

#define dd(x) \
	ASM _emit	(x)			& 0xff	\
	ASM _emit	((x) >> 8)	& 0xff	\
	ASM _emit	((x) >> 16)	& 0xff	\
	ASM _emit	((x) >> 24)	& 0xff 

//#define jmpf(seg, off) db(0xea); dd(off); dw(seg)
#define __iowait() __outbyte(0x80, 0)

typedef unsigned __int8 u8;
typedef unsigned __int16 u16;
typedef unsigned __int32 u32;
typedef unsigned __int64 u64;

typedef __int16 char16_t;
typedef __int32 char32_t;

typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned char byte;