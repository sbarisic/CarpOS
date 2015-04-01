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

Pixel* Video::Mem = NULL;
Pixel* Video::TextMem = NULL;
ushort Video::Width = 0;
ushort Video::Height = 0;
ushort Video::BitsPerPixel = 0;
ushort Video::BytesPerPixel = 0;
Pixel* Video::Font = NULL;
uint Video::CharW = 0;
uint Video::CharH = 0;
uint Video::Padding = 0;
uint Video::BytesPerLine = 0;
bool Video::Initialized = false;

void Video::Init() {
	Initialized = false;
	Width = 800;
	Height = 600;
	BitsPerPixel = 24;
	Padding = 0;

	if (Info->flags & (1 << 11) != 0) {
		vbe_info_t* VBEInfo = (vbe_info_t*)Info->vbe_mode_info;	
		Mem = (Pixel*)VBEInfo->physbase;
		if (Mem != NULL) {
			BitsPerPixel = VBEInfo->bpp;
			Width = VBEInfo->Xres;
			Height = VBEInfo->Yres;

			BytesPerLine = VBEInfo->pitch;
			BytesPerPixel = BitsPerPixel / 8;
			Padding = (VBEInfo->pitch / BytesPerPixel) - Width;
			Initialized = true;
		}
	} else if(BGAVersion() == VBE_DISPI_ID5) {
		BGAWrite(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
		BGAWrite(VBE_DISPI_INDEX_XRES, Width);
		BGAWrite(VBE_DISPI_INDEX_YRES, Height);
		BGAWrite(VBE_DISPI_INDEX_BPP, BitsPerPixel);
		BGAWrite(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);

		// TODO: Proper way
		Mem = (Pixel*)0xE0000000;

		Initialized = true;
	} else {
		Kernel::Print("Can not initialize video!\n");
		Kernel::CarpScreenOfDeath();
	}

	TextMem = Mem + (Width * Height * BytesPerPixel);
}

void Video::DisplayText() {
	if (!Initialized)
		return;
	DrawImage(TextMem, true);
}

void Video::SetPixel(int Idx, Pixel P) {
	if (!Initialized)
		return;
	Mem[Idx] = P;
}

void Video::SetPixel(int X, int Y, Pixel P) {
	if (!Initialized)
		return;
	SetPixel(Y * Width + X, P);
}

Pixel GetCharColor(uint X, uint Y, char C) {
	return Video::Font[(((C / 16) * Video::CharH + Y) * 16 * Video::CharW) + (C % 16) * Video::CharW + X];
}

void Video::DrawScanline(uint L, Pixel* Line, bool DiscardBlack) {
	uint Offset = L * (Width + Padding);

	for (int i = 0; i < Width; i++) {
		Pixel P = Line[i];
		if (DiscardBlack && P.R == 0 && P.G == 0 && P.B == 0)
			continue;
		Mem[Offset + i] = P;
	}
}

void Video::DrawImage(Pixel* Img, bool DiscardBlack) {
	if (!Initialized)
		return;

	for (int i = 0; i < Height; i++) 
		DrawScanline(i, Img + i * Width, DiscardBlack);
}

/*void Video::ClearLine(int i) {
uint TextWidth = Width * (BitsPerPixel / 8);
memset(TextMem + TextWidth * i * CharH, 0xFFFFFF, TextWidth * CharH);
}*/

void Video::ClearText() {
	if (!Initialized)
		return;
	memset(TextMem, NULL, Width * Height * BytesPerPixel);
}

void Video::ScrollText() {
	if (!Initialized)
		return;
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
			Pixel Clr = GetCharColor(CX, CY, C);
			if (Clr.R != 0 || Clr.G != 0 || Clr.B != 0)
				TextMem[(Y * CharH + CY) * Width + (X * CharW + CX)] = Clr;
		}
}