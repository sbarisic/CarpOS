#include "Memory.h"
#include "Kernel.h"

#define VALID(E) (E != NULL && E->Magic == MEMORY_MAGIC)

//#define DEBUG
#ifdef DEBUG
#define PRINT Kernel::Print
#else
#define PRINT(...)
#endif

uint Memory::PlacementAddr;
MemEntry* Memory::Start = NULL;

void* Memory::KAlloc(uint Size, bool Align, uint* Phys) {
	if (Align && (PlacementAddr & 0xFFFFF000)) {
		PlacementAddr &= 0xFFFFF000;
		PlacementAddr += 0x1000;
	}

	if (Phys)
		*Phys = PlacementAddr;

	PlacementAddr += Size;
	return (void*)(PlacementAddr - Size);
}

MemEntry* Memory::Create(void* Addr, uint Size) {
	return CreateBetween(Addr, (void*)((uint)Addr + Size + sizeof(MemEntry)));
}

MemEntry* Memory::CreateBetween(void* Start, void* End) {
	uint Size = (uint)End - (uint)Start - sizeof(MemEntry);
	if (Size <= NULL) {
		PRINT("CreateBetween: Size is less than or equal to 0!\n");
		return NULL;
	}

	MemEntry* E = (MemEntry*)Start;
	*E = MemEntry();
	E->Magic = MEMORY_MAGIC;
	E->Free = true;
	E->Size = Size;
	E->Next = E->Prev = NULL;
	return E;
}

bool Memory::Split(MemEntry* Entry, uint Size) {
	if (!VALID(Entry) || (Entry->Size < sizeof(MemEntry) + Size)) {
		PRINT("Split: Memory not valid/Size not valid\n");
		return false;
	}
	if (Entry->Size == Size) {
		PRINT("Split: Entry->Size == Size\n");
		return true;
	}

	MemEntry* Rest = Create((void*)((uint)Entry + sizeof(MemEntry) + Size), Entry->Size - Size - sizeof(MemEntry));
	MemEntry* New = Create(Entry, Size);
	Rest->Prev = New;
	New->Next = Rest;

	PRINT("Split: Splitting!\n");
	return true;
}

bool Memory::Unify(MemEntry* Entry) {
	MemEntry* Prev = Entry;
	MemEntry* Next = Entry;

	if (!VALID(Entry)) {
		PRINT("Unify: Entry not valid\n");
		return false;
	}

	PRINT("sizeof(MEmEntry) = ", sizeof(MemEntry), "\n");
	PRINT("Prev: ", (uint)Prev, "\n");
	PRINT("Prev->Prev: ", (uint)Prev->Prev, "\n");
	PRINT("Prev->Prev->Size: ", Prev->Prev->Size, "\n");

	while (Prev->Prev != NULL && Prev->Prev->Free && ((uint)Prev->Prev + sizeof(MemEntry) + Prev->Prev->Size == (uint)Prev))
		Prev = Prev->Prev;
	while (Next->Next != NULL && Next->Next->Free && ((uint)Next + sizeof(MemEntry) + Next->Size == (uint)Next->Next))
		Next = Next->Next;

	if (Prev == Next) {
		PRINT("Unify: Previous == Next\n");
		return false;
	}

	MemEntry* EPrev = Prev->Prev;
	MemEntry* ENext = Next->Next;
	MemEntry* E = CreateBetween(Prev, (void*)((uint)Next + Next->Size + sizeof(MemEntry)));
	E->Prev = EPrev;
	E->Next = ENext;
	if (E->Prev != NULL)
		E->Prev->Next = E;
	if (E->Next != NULL)
		E->Next->Prev = E;

	PRINT("Unify: Unifying!\n");
	return true;
}

void Memory::Init(void* Start, void* End) {
	if (End == NULL) {
		Memory::Start = Create(Start, 1);
		Memory::Start->Free = false;
	} else
		Memory::Start = CreateBetween(Start, End);
}

void* Memory::Alloc(uint Size) {
	MemEntry* Cur = Start;
	while ((!Cur->Free || (Cur->Free && !Split(Cur, Size))) && VALID(Cur->Next))
		Cur = Cur->Next;

	MemEntry* E = NULL;

	if (VALID(Cur)) {
		if (Cur->Free)
			E = Cur;
		else {
			E = Create((void*)((uint)Cur + sizeof(MemEntry) + Cur->Size), Size);
			E->Prev = Cur;
			Cur->Next = E;
		}

		E->Free = false;
		return (void*)((uint)E + sizeof(MemEntry));
	}

	PRINT("Alloc: Current memory not valid!\n");
	return NULL;
}

void Memory::Free(void* Addr) {
	MemEntry* E = (MemEntry*)((uint)Addr - sizeof(MemEntry));
	if (!VALID(E)) {
		PRINT("Free: Cannot free invalid memory!\n");
		return;
	}
	E->Free = true;
	Unify(E);
}

uint Memory::SizeOf(void* Addr) {	
	MemEntry* E = (MemEntry*)((uint)Addr - sizeof(MemEntry));
	if (!VALID(E)) {
		PRINT("SizeOf: Cannot get size of invalid memory!\n");
		return 0;
	}
	return E->Size;
}