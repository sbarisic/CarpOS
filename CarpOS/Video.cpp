#include "Video.h"
#include "Kernel.h"
#include <string.h>

#define TO_COLOR(R, G, B) (B | (G << 8) | (R << 16))

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
uint* Video::TextMem = NULL;
ushort Video::Width = 0;
ushort Video::Height = 0;
ushort Video::BitsPerPixel = 0;
uint* Video::Font = NULL;
uint Video::CharW = 0;
uint Video::CharH = 0;
bool Video::Initialized = false;

void Video::Init() {
	Width = 800;
	Height = 600;
	BitsPerPixel = 32;

	if (Info->vbe_mode_info) {
		vbe_info_t* VBEInfo = (vbe_info_t*)Info->vbe_mode_info;
		BitsPerPixel = VBEInfo->bpp;
		Width = VBEInfo->Xres;
		Height = VBEInfo->Yres;
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

	TextMem = Mem + (Width * Height * (BitsPerPixel / 8));
}

void Video::DisplayText() {
	for (int i = 0; i < Width * Height * (BitsPerPixel / 8); i++) {
		if (TextMem[i] != 0)
			Mem[i] = TextMem[i];
	}
}

void Video::SetPixel(int Idx, byte R, byte G, byte B) {
	if (!Initialized)
		return;
	Mem[Idx] = TO_COLOR(R, G, B);
}

void Video::SetPixel(int X, int Y, byte R, byte G, byte B) {
	SetPixel(Y * Width + X, R, G, B);
}

uint GetCharColor(uint X, uint Y, char C) {
	return Video::Font[(((C / 16) * Video::CharH + Y) * 16 * Video::CharW) + (C % 16) * Video::CharW + X];
}

void Video::DrawImage(uint* Img) {
	memcpy(Mem, Img, Width * Height * (BitsPerPixel / 8));
}

/*void Video::ClearLine(int i) {
	uint TextWidth = Width * (BitsPerPixel / 8);
	memset(TextMem + TextWidth * i * CharH, 0xFFFFFF, TextWidth * CharH);
}*/

void Video::ClearScreen() {
	memset(TextMem, NULL, Width * Height * (BitsPerPixel / 8));
}

void Video::ScrollText() {
	for (int i = 0; i < Width * Height; i++)
		TextMem[i] = TextMem[i + Width * CharH];
}

void Video::SetChar(int X, int Y, char C) {
	if (!Initialized)
		return;
	const int FontWidth = 16;

	if (C > 128)
		C = 4;

	for (int CX = 0; CX < CharW; CX++)
		for (int CY = 0; CY < CharH; CY++) {
			uint Clr = GetCharColor(CX, CY, C);
			if (Clr & 0xFF != 0)
				TextMem[(Y * CharH + CY) * Width + (X * CharW + CX)] = Clr;
		}
}