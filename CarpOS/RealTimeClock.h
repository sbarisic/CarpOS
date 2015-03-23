#pragma once
#include "CarpSDK.h"

class RTC {
public:
	static byte Second;
	static byte Minute;
	static byte Hour;
	static byte Day;
	static byte Month;
	static uint Year;

	static void Update();
	static const char* GetTime();
};