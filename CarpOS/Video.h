#pragma once
#include "CarpSDK.h"
#include <intrin.h>

#define VBE_DISPI_ID0 0xB0C0
#define VBE_DISPI_ID1 0xB0C1
#define VBE_DISPI_ID2 0xB0C2
#define VBE_DISPI_ID3 0xB0C3
#define VBE_DISPI_ID4 0xB0C4
#define VBE_DISPI_ID5 0xB0C5

#define VBE_DISPI_IOPORT_INDEX	0x01CE
#define VBE_DISPI_IOPORT_DATA	0x01CF

#define VBE_DISPI_INDEX_ID			0
#define VBE_DISPI_INDEX_XRES		1
#define VBE_DISPI_INDEX_YRES		2
#define VBE_DISPI_INDEX_BPP			3
#define VBE_DISPI_INDEX_ENABLE		4
#define VBE_DISPI_INDEX_BANK		5
#define VBE_DISPI_INDEX_VIRT_WIDTH	6
#define VBE_DISPI_INDEX_VIRT_HEIGHT	7
#define VBE_DISPI_INDEX_X_OFFSET	8
#define VBE_DISPI_INDEX_Y_OFFSET	9

#define VBE_DISPI_DISABLED		0
#define VBE_DISPI_ENABLED		1
#define VBE_DISPI_LFB_ENABLED	0x40

typedef struct {
	byte B;
	byte G;
	byte R;
	byte A;
} Pixel;

class Video {
private:
	static ushort Width;
	static ushort Height;
	static ushort BitsPerPixel;
	static ushort BytesPerPixel;
	static uint Padding;
	static uint BytesPerLine;

public:
	static Pixel* Mem;
	static Pixel* TextMem;
	static Pixel* Font;
	static uint CharW;
	static uint CharH;

	static bool Initialized;

	static void Init();
	static void DisplayText();
	static void DrawImage(Pixel* Img, bool DiscardBlack = false);
	static void DrawScanline(uint L, Pixel* Line, bool DiscardBlack = false);
	//static void ClearLine(int i);
	static void ClearText();
	static void ScrollText();
	static void SetChar(int X, int Y, char C);
	static void SetPixel(int Idx, Pixel P);
	static void SetPixel(int X, int Y, Pixel P);
};