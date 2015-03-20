#pragma once
#include "CarpSDK.h"

#define GDTSIZE 6
#define GDTBASE 0x800

typedef struct {
	u16 limit;
	u32 base;
} GDTR;

typedef struct {
	u16 lim0_15;
	u16 base0_15;
	u8 base16_23;
	u8 access;
	u8 lim16_19;
	u8 other;
	u8 base24_31;
} GDTDesc;

void GDTInit();