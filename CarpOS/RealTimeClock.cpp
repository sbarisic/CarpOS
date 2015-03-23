#include "RealTimeClock.h"
#include <string.h>
#define CURRENT_YEAR 2015

enum {
	cmos_address = 0x70,
	cmos_data    = 0x71
};

int century_register = 0x00;

int get_update_in_progress_flag() {
	__outbyte(cmos_address, 0x0A);
	return (__inbyte(cmos_data) & 0x80);
}

unsigned char get_RTC_register(int reg) {
	__outbyte(cmos_address, reg);
	return __inbyte(cmos_data);
}

byte RTC::Day = 0;
byte RTC::Hour = 0;
byte RTC::Minute = 0;
byte RTC::Month = 0;
byte RTC::Second = 0;
uint RTC::Year = 0;

void RTC::Update() {  
	unsigned char century;
	unsigned char last_Second;
	unsigned char last_Minute;
	unsigned char last_Hour;
	unsigned char last_Day;
	unsigned char last_Month;
	unsigned char last_Year;
	unsigned char last_century;
	unsigned char registerB;

	// Note: This uses the "read registers until you get the same values twice in a row" technique
	//       to avoid getting dodgy/inconsistent values due to RTC updates

	while (get_update_in_progress_flag());                // Make sure an update isn't in progress
	Second = get_RTC_register(0x00);
	Minute = get_RTC_register(0x02);
	Hour = get_RTC_register(0x04);
	Day = get_RTC_register(0x07);
	Month = get_RTC_register(0x08);
	Year = get_RTC_register(0x09);
	if(century_register != 0) {
		century = get_RTC_register(century_register);
	}

	do {
		last_Second = Second;
		last_Minute = Minute;
		last_Hour = Hour;
		last_Day = Day;
		last_Month = Month;
		last_Year = Year;
		last_century = century;

		while (get_update_in_progress_flag());           // Make sure an update isn't in progress
		Second = get_RTC_register(0x00);
		Minute = get_RTC_register(0x02);
		Hour = get_RTC_register(0x04);
		Day = get_RTC_register(0x07);
		Month = get_RTC_register(0x08);
		Year = get_RTC_register(0x09);
		if(century_register != 0) {
			century = get_RTC_register(century_register);
		}
	} while( (last_Second != Second) || (last_Minute != Minute) || (last_Hour != Hour) ||
		(last_Day != Day) || (last_Month != Month) || (last_Year != Year) ||
		(last_century != century) );

	registerB = get_RTC_register(0x0B);

	// Convert BCD to binary values if necessary

	if (!(registerB & 0x04)) {
		Second = (Second & 0x0F) + ((Second / 16) * 10);
		Minute = (Minute & 0x0F) + ((Minute / 16) * 10);
		Hour = ( (Hour & 0x0F) + (((Hour & 0x70) / 16) * 10) ) | (Hour & 0x80);
		Day = (Day & 0x0F) + ((Day / 16) * 10);
		Month = (Month & 0x0F) + ((Month / 16) * 10);
		Year = (Year & 0x0F) + ((Year / 16) * 10);
		if(century_register != 0) {
			century = (century & 0x0F) + ((century / 16) * 10);
		}
	}

	// Convert 12 Hour clock to 24 Hour clock if necessary

	if (!(registerB & 0x02) && (Hour & 0x80)) {
		Hour = ((Hour & 0x7F) + 12) % 24;
	}

	// Calculate the full (4-digit) Year

	if(century_register != 0) {
		Year += century * 100;
	} else {
		Year += (CURRENT_YEAR / 100) * 100;
		if(Year < CURRENT_YEAR) Year += 100;
	}
}

const char* RTC::GetTime() {
	char Time[10] = { 0 };
	char SHour[5] = { 0 };
	char SMinute[5] = { 0 };
	char SSecond[5] = { 0 };

	Update();

	itoa(Hour, SHour, 10);
	itoa(Minute, SMinute, 10);
	itoa(Second, SSecond, 10);
	if (Hour < 10)
		strcat(Time, "0");
	strcat(Time, SHour);
	strcat(Time, ":");
	if (Minute < 10)
		strcat(Time, "0");
	strcat(Time, SMinute);
	strcat(Time, ":");
	if (Second < 10)
		strcat(Time, "0");
	strcat(Time, SSecond);

	return Time;
}