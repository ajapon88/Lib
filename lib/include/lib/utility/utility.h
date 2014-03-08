#ifndef __LIB_UTILITY_H__
#define __LIB_UTILITY_H__
#include "stdafx.h"

namespace lib {
namespace utility {
extern bool isInt(const char *str);
extern bool isFloat(const char *str);
extern bool isBin(const char *str);
extern bool isOct(const char *str);
extern bool isHex(const char *str);

extern int a2i(const char *str);
extern double a2f(const char *str);
extern int a2iBin(const char *str);
extern int a2iOct(const char *str);
extern int a2iHex(const char *str);

extern bool isBOM(const char *str);
extern const char *skipBOM(const char *str);
extern char *skipBOM(char *str);

template <class T> T clamp(const T &val, const T &min_val, const T &max_val)
{
	if (val < min_val) return min_val;
	if (max_val < val) return max_val;
	return val;
}

template <class T> T forseCast(void *p) {
	return static_cast<T>(p);
}
template <class T> T dynamicCast(void *p) {
	T cast = dynamic_cast<T>(p);
	ASSERT(cast);
	return cast;
}

} // namespace utility
} // namespace lib

#endif	// __LIB_UTILITY_H__