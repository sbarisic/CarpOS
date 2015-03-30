#include "Video.h"
#include "Kernel.h"

void BGAWrite(ushort Reg, ushort Data) {
	__outword(VBE_DISPI_IOPORT_INDEX, Reg);
	__outword(VBE_DISPI_IOPORT_DATA, Data);
}

ushort BGARead(ushort Reg) {
	__outword(VBE_DISPI_IOPORT_INDEX, Reg);
	return __inword(VBE_DISPI_IOPORT_DATA);
}

ushort BGAVersion() {
	return BGARead(VBE_DISPI_INDEX_ID);
}

uint* Video::Mem = NULL;
ushort Video::Width = 0;
ushort Video::Height = 0;
ushort Video::BitsPerPixel = 0;
bool Video::Initialized = false;

void Video::Init() {
	Width = 800;
	Height = 600;
	BitsPerPixel = 32;

	if (Info->vbe_mode_info) {
		vbe_info_t* VBEInfo = (vbe_info_t*)Info->vbe_mode_info;
		BitsPerPixel = VBEInfo->bpp;
		Mem = (uint*)VBEInfo->physbase;

		Initialized = true;
	} else if(BGAVersion() == VBE_DISPI_ID5) {
		BGAWrite(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
		BGAWrite(VBE_DISPI_INDEX_XRES, Width);
		BGAWrite(VBE_DISPI_INDEX_YRES, Height);
		BGAWrite(VBE_DISPI_INDEX_BPP, BitsPerPixel);
		BGAWrite(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);

		// TODO: Proper way
		Mem = (uint*)0xE0000000;

		Initialized = true;
	} else {
		Kernel::Print("Can not initialize video!\n");
		Kernel::CarpScreenOfDeath();
	}
}

void Video::SetPixel(int Idx, byte R, byte G, byte B) {
	if (!Initialized)
		return;
	Mem[Idx] = B | (G << 8) | (R << 16);
}

void Video::SetPixel(int X, int Y, byte R, byte G, byte B) {
	SetPixel(Y * Width + X, R, G, B);
}