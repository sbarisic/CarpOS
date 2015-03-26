#include "Paging.h"
#include "Kernel.h"
#include <string.h>

#define INDEX_FROM_BIT(a) (a / (8 * 4))
#define OFFSET_FROM_BIT(a) (a % (8 * 4))

uint PlacementAddr = 0xB00000;
uint* Frames;
uint NFrames;

uint page_alloc(uint Size, bool Align = false, uint* Phys = NULL) {
	if (Align && (PlacementAddr & 0xFFFFF000)) {
		PlacementAddr &= 0xFFFFF000;
		PlacementAddr += 0x1000;
	}
	if (Phys)
		*Phys = PlacementAddr;
	uint Tmp = PlacementAddr;
	PlacementAddr += Size;
	return Tmp;
}

void SetFrame(uint FrameAddr) {
	uint Frame = FrameAddr / 0x1000;
	uint Idx = INDEX_FROM_BIT(Frame);
	uint Offset = OFFSET_FROM_BIT(Frame);
	Frames[Idx] |= (0x1 << Offset);
}

void ClearFrame(uint FrameAddr) {
	uint Frame = FrameAddr / 0x1000;
	uint Idx = INDEX_FROM_BIT(Frame);
	uint Offset = OFFSET_FROM_BIT(Frame);
	Frames[Idx] &= ~(0x1 << Offset);
}

uint FirstFrame() {
	for (uint i = 0; i < INDEX_FROM_BIT(NFrames); i++) {
		if (Frames[i] != 0xFFFFFFFF)
			for (uint j = 0; j < 32; j++) {
				if (!(Frames[i] & (0x1 << j)))
					return i * 4 * 8 + j;
			}
	}
	return -1;
}

void AllocFrame(Page* Pg, bool IsKernel, bool IsWritable) {
	if (!Pg || Pg->Frame != 0)
		return;

	uint Idx = FirstFrame();
	if (Idx == (uint)-1)
		ASM int 1;
	SetFrame(Idx * 0x1000);
	Pg->Present = 1;
	Pg->Writable = IsWritable ? 1 : 0;
	Pg->User = IsKernel ? 0 : 1;
	Pg->Frame = Idx;
}

void FreeFrame(Page* Pg) {
	uint Frame;
	if (!(Frame = Pg->Frame))
		return;
	ClearFrame(Frame);
	Pg->Frame = NULL;
}

Page* GetPage(uint Addr, bool Make, PageDirectory* Dir) {
	Addr /= 0x1000;
	uint TableIdx = Addr / 1024;
	if (Dir->Tables[TableIdx])
		return &Dir->Tables[TableIdx]->Pages[Addr % 1024];
	else if (Make) {
		uint Tmp;
		Dir->Tables[TableIdx] = (PageTable*)page_alloc(sizeof(PageTable), true, &Tmp);
		memset(Dir->Tables[TableIdx], NULL, 0x1000);
		Dir->TablesPhysical[TableIdx] = Tmp | 0x7;
		return &Dir->Tables[TableIdx]->Pages[Addr % 1024];
	} else
		return NULL;
}

PageDirectory* Paging::CurrentDirectory = NULL;
PageDirectory* Paging::KernelDirectory = NULL;

void Paging::Init(uint MemLen) {
	NFrames = MemLen / 0x1000;
	Frames = (uint*)page_alloc(INDEX_FROM_BIT(NFrames));
	memset(Frames, 0, INDEX_FROM_BIT(NFrames));

	KernelDirectory = (PageDirectory*)page_alloc(sizeof(PageDirectory), true);
	memset(KernelDirectory, 0, sizeof(PageDirectory));

	int i = 0;
	while (i < PlacementAddr) {
		AllocFrame(GetPage(i, true, KernelDirectory), false, false);
		i += 0x1000;
	}

	SetPageDir(KernelDirectory);
	Enable();
}

void Paging::SetPageDir(PageDirectory* PageDir) {
	CurrentDirectory = PageDir;
	void* Phys = &PageDir->TablesPhysical;
	ASM {
		mov eax, Phys;
		mov cr3, eax;
	}
}

void Paging::Enable() {
	uint _cr0;
	ASM {
		mov eax, cr0;
		mov _cr0, eax;
	}
	_cr0 |= 0x80000000;
	ASM {
		mov eax, _cr0;
		mov cr0, eax;
	}
}

void Paging::Disable() {	
	uint _cr0;
	ASM {
		mov eax, cr0;
		mov _cr0, eax;
	}
	_cr0 &= ~0x80000000;
	ASM {
		mov eax, _cr0;
		mov cr0, eax;
	}
}