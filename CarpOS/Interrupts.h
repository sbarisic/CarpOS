#include "CarpSDK.h"

#define IDTSIZE 1

typedef struct {
	ushort offset0_15;
	ushort select;
	ushort type;
	ushort offset16_31;
} IDTEntry;

typedef struct {
	ushort limit;
	uint base;
} IDTPtr;

void InterruptsInit();