#include <string.h>

void reverse(char* str, int length) {
	int start = 0;
	int end = length - 1;
	while (start < end) {
		char Tmp = *(str + start);
		*(str + start) = *(str + end);
		*(str + end) = Tmp;
		start++;
		end--;
	}
}

char* itoa(int num, char* str, int base) {
	unsigned int unum = 0;
	int i = 0;
	bool isNegative = false;
 
	if (num == 0) {
		str[i++] = '0';
		str[i] = '\0';
		return str;
	}
 
	if (num < 0) {
		if (base == 10) {
			isNegative = true;
			num = -num;
		}
	}
 
	unum = (unsigned int)num;
	while (unum != 0) {
		unsigned int rem = unum % base;
		str[i++] = (rem > 9) ? (rem - 10) + 'A' : rem + '0';
		unum = unum / base;
	}
 
	if (isNegative)
		str[i++] = '-';
	str[i] = '\0';
	reverse(str, i);
	return str;
}