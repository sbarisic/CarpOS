#pragma once
#include "CarpSDK.h"
#define MEMORY_MAGIC 0x1A2B3C4D

typedef struct _MemEntry {
	uint Magic;
	bool Free;
	uint Size;
	struct _MemEntry* Prev;
	struct _MemEntry* Next;
} MemEntry;

class Memory {
private:
	static MemEntry* Create(void* Addr, uint Size);
	static MemEntry* CreateBetween(void* Start, void* End);
	static bool Split(MemEntry* Entry, uint Size);
	static bool Unify(MemEntry* Entry);

public:
	static uint PlacementAddr;
	static MemEntry* Start;

	static void Init(void* Start, void* End = NULL);

	static void* KAlloc(uint Size, bool Align = false, uint* Phys = NULL);
	static void* Alloc(uint Size);
	static void Free(void* Addr);
	static uint SizeOf(void* Addr);
};