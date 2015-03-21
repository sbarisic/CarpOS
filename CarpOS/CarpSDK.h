#pragma once

//#pragma comment(lib, "CarpLib.lib")

//#define __STDC__ 1
#define NULL 0

#define _PDCLIB_restrict

#define EXTERN extern "C"
#define EXPORT EXTERN __declspec(dllexport) // exported from DLL
#define IMPORT EXTERN __declspec(dllimport) // imported from DLL
#define NAKED __declspec(naked) // no prolog or epilog code added
#define NORETURN __declspec(noreturn)
#define ASM __asm

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