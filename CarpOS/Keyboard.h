#pragma once
#include "CarpSDK.h"

class Keyboard {
public:
	static bool CapsLock;

	static void OnKey(byte Scancode);
	static bool IsBusy();

	static void SetLED(byte LED);
	static void OutCommand(byte Cmd);
	static void OutData(byte Data);
	static byte InCommand();
	static byte InData();
};