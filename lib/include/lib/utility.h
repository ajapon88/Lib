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

int a2i(const char *str);
double a2f(const char *str);
int a2b(const char *str);
int a2o(const char *str);
int a2h(const char *str);
}
}

#endif	// __LIB_UTILITY_H__