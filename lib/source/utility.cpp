#include "stdafx.h"
#include "utility.h"

namespace lib {
namespace utility {
	
bool isInt(const char *str)
{
	if (*str == '+' || *str == '-') {
		str++;
	}
	if (strlen(str) == 0) {
		return false;
	}
	while (*str != '\0') {
		if (*str < '0' || '9' < *str) {
			return false;
		}
		str++;
	}
	return true;
}

bool isFloat(const char *str)
{
	if (*str == '+' || *str == '-') {
		str++;
	}
	if (strlen(str) == 0) {
		return false;
	}
	int state = 0;
	while (*str != '\0') {
		switch (state) {
		case 0:
			if (*str == '.') {
				if (*(str+1) < '0' || '9' < *(str+1)) {
					return false;
				}
				state++;
			} else if (*str == 'e' || *str == 'E') {
				return isInt(str+1);
			} else if (*str < '0' || '9' < *str) {
				return false;
			}
			break;
		case 1:
			if (*str == 'e' || *str == 'E') {
				return isInt(str+1);
			} else if (*str < '0' || '9' < *str) {
				return false;
			}
			break;
		}
		str++;
	}
	return true;
}

bool isBin(const char *str) {
	if (strlen(str) == 0) {
		return false;
	}
	while(*str != '\0') {
		if (*str != '0' && *str != '1') {
			return false;
		}
		str++;
	}
	return true;
}

bool isOct(const char *str) {
	if (strlen(str) == 0) {
		return false;
	}
	if (*str != '0') {
		return false;
	}
	str++;
	while(*str != '\0') {
		if (*str < '0' || '7' < *str) {
			return false;
		}
		str++;
	}
	return true;
}

bool isHex(const char *str) {
	if (strlen(str) < 3) {
		return false;
	}
	if (*str != '0' || *(str+1) != 'x') {
		return false;
	}
	str += 2;
	while(*str != '\0') {
		if ((*str < '0' && '9' < *str) &&
			(*str < 'a' && 'f' < *str) &&
			(*str < 'A' && 'F' < *str)) {
			return false;
		}
		str++;
	}
	return true;
}

int str2Int(const char *str)
{
	return atoi(str);
}

float str2Float(const char *str)
{
	return atof(str);
}

int str2Bin(const char *str)
{
	int res = 0;
	while(*str != '\0') {
		if (*str != '0' && *str != '1') {
			break;
		}
		res = (res<<1) + *str-'0';
		str++;
	}
	return res;
}

int str2Oct(const char *str)
{
	int res = 0;
	while(*str != '\0') {
		if (*str < '0' || '7' < *str) {
			break;
		}
		res += res*8 + *str-'0';
		str++;
	}
	return res;
}

int str2Hex(const char *str)
{
	int res = 0;
	if (strlen(str) < 3) {
		return 0;
	}
	if (*str != '0' || *(str+1) != 'x') {
		return 0;
	}
	str += 2;
	while(*str != '\0') {
		if ('0' <= *str && *str <= '9')  res = res*16 + *str-'0';
		else if ('a' <= *str && *str <= 'f')  res = res*16 + 10 + *str-'a';
		else if ('A' <= *str && *str <= 'F')  res = res*16 + 10 + *str-'A';
		else return res;
		str++;
	}
	return res;
}

}
}
