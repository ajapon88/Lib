#ifndef __LIB_UTILITY_H__
#define __LIB_UTILITY_H__
#include "stdafx.h"

namespace lib {
namespace utility {
bool isInt(const char *str);
bool isFloat(const char *str);
bool isBin(const char *str);
bool isOct(const char *str);
bool isHex(const char *str);

int str2Int(const char *str);
float str2Float(const char *str);
int str2Bin(const char *str);
int str2Oct(const char *str);
int str2Hex(const char *str);
}
}

#endif	// __LIB_UTILITY_H__