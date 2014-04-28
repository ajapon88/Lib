#include "stdafx.h"
#include "lib/utility/utility.h"

namespace {
static const unsigned char BOM[] = {0xEF, 0xBB, 0xBF};
}

namespace lib {
namespace utility {
	
bool isInt(const char *str)
{
	if (*str == '+' || *str == '-')  str++;

	if (*str == '\0')  return false;
	if (*str == '0' && *(str+1) != '\0')  return false;

	while (*str != '\0') {
		if (!isdigit(*str))  return false;
		str++;
	}
	return true;
}

bool isFloat(const char *str)
{
	if (*str == '+' || *str == '-')  str++;
	if (*str == '\0')  return false;
	if (*str == '0' && isdigit(*(str+1)))  return false;

	int state = 0;
	while (*str != '\0') {
		switch (state) {
		case 0:	// 実数部
			if (*str == '.') {
				str++;
				if (!isdigit(*str))  return false;
				state = 1;
				continue;
			} else if (*str == 'e' || *str == 'E') {
				str++;
				if (*str == '+' || *str == '-')  str++;
				if (*str == '\0')  return false;
				state = 2;
				continue;
			} else if (!isdigit(*str)) {
				return false;
			}
			break;
		case 1:	// 小数部
			if (*str == 'e' || *str == 'E') {
				str++;
				if (*str == '+' || *str == '-')  str++;
				if (*str == '\0')  return false;
				state = 2;
				continue;
			} else if (!isdigit(*str)) {
				return false;
			}
			break;
		case 2:	// 指数部
			if (!isdigit(*str))  return false;
			break;
		}
		str++;
	}
	return true;
}

bool isBin(const char *str) {
	if (*str == '\0')  return false;
	while(*str != '\0') {
		if (*str != '0' && *str != '1')  return false;
		str++;
	}
	return true;
}

bool isOct(const char *str) {
	if (*str == '\0')  return false;
	if (*str != '0')  return false;
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
	if (strlen(str) < 3)  return false;
	if (*str != '0' || *(str+1) != 'x')  return false;
	str += 2;
	while(*str != '\0') {
		if (!isdigit(*str) &&
			(*str < 'a' && 'f' < *str) &&
			(*str < 'A' && 'F' < *str)) {
			return false;
		}
		str++;
	}
	return true;
}

int a2i(const char *str)
{
	return atoi(str);
}

double a2f(const char *str)
{
	return atof(str);
}

int a2iBin(const char *str)
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

int a2iOct(const char *str)
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

int a2iHex(const char *str)
{
	int res = 0;
	if (strlen(str) < 3)  return 0;
	if (*str != '0' || *(str+1) != 'x')  return 0;
	str += 2;
	while(*str != '\0') {
		if (isdigit(*str))  res = res*16 + *str-'0';
		else if ('a' <= *str && *str <= 'f')  res = res*16 + 10 + *str-'a';
		else if ('A' <= *str && *str <= 'F')  res = res*16 + 10 + *str-'A';
		else return res;
		str++;
	}
	return res;
}

bool isBOM(const char *str)
{
	return memcmp(str, BOM, sizeof(BOM)) == 0;
}

const char *skipBOM(const char *str)
{
	if (memcmp(str, BOM, sizeof(BOM)) == 0)  return (str + sizeof(BOM));
	return str;
}
char *skipBOM(char *str)
{
	if (memcmp(str, BOM, sizeof(BOM)) == 0)  return (str + sizeof(BOM));
	return str;
}

void expansionEscape(std::string *dst, const char *src)
{
	*dst = "";
	while(*src != '\0') {
		if (*src == '\\') {
			src++;
			if (*src == '\0')  break;
			switch(*src) {
				case '\\': *dst += '\\'; break;
				case '\'': *dst += '\''; break;
				case '\"': *dst += '\"'; break;
				case 'r': *dst += '\r'; break;
				case 'n': *dst += '\n'; break;
				case 't': *dst += '\t'; break;
				default:
					*dst += '\\';
					continue;
			}
		} else {
			*dst +=	*src;
		}
		src++;
	}
}

void contractionEscape(std::string *dst, const char *src)
{
	*dst = "";
	while(*src != '\0') {
		switch(*src) {
			case '\\': *dst += "\\\\"; break;
			case '\'': *dst += "\\\'"; break;
			case '\"': *dst += "\\\""; break;
			case '\r': *dst += "\\r"; break;
			case '\n': *dst += "\\n"; break;
			case '\t': *dst += "\\t"; break;
			default:
				*dst += *src;
		}
		src++;
	}
}



std::string trim(const char* str, const char* trim_str)
{
	std::string tmp(str);
	return trim(tmp, trim_str);
}

std::string trim(const std::string& str, const char* trim_str)
{
	std::string res;
	std::string::size_type pos0, pos1;
	pos0 = str.find_first_not_of(trim_str);
	if (pos0 != std::string::npos) {
		pos1 = str.find_last_not_of(trim_str)+1;
		res = str.substr(pos0, pos1);
	} else {
		res = str;
	}

	return res;
}

char encodeBase64Char(unsigned int c)
{
	if (c < 0x1A)  return 'A'+c;
	if (c < 0x34)  return 'a'+(c-0x1A);
	if (c < 0x3E)  return '0'+(c-0x34);
	if (c == 0x3E)  return '+';
	if (c == 0x3F)  return '/';
	ASSERT(false);
	return '?';
}

std::string encodeBase64(const char* str)
{
	std::string res;
	int bit = 0;
	while(*str != '\0') {
		uint16_t t = (str[0]<<8) + str[1];
		int b = 10-bit;
		bit = (16-b)%8;
		if (b <= 8)  str++;
		res += encodeBase64Char(0x3F&(t>>b));
	}
	int f = res.length()%4;
	if (f == 1)  res += "===";
	else if (f == 2)  res += "==";
	else if (f == 3)  res += "=";

	return res;
}

unsigned char decodeBase64Char(char c)
{
	if ('A' <= c && c <= 'Z')  return c-'A';
	if ('a' <= c && c <= 'z')  return 0x1A + c-'a';
	if ('0' <= c && c <= '9')  return 0x34 + c-'0';
	if (c == '+')  return 0x3E;
	if (c == '/')  return 0x3F;
	return 0x00;
}

std::string decodeBase64(const char* str)
{
	std::string res;

	int len = strlen(str);
	ASSERT(len%4 == 0);
	if (len%4 != 0)  return res;

	while(*str != '\0') {
		char buf[4];
		buf[0] = decodeBase64Char(str[0]);
		buf[1] = decodeBase64Char(str[1]);
		buf[2] = decodeBase64Char(str[2]);
		buf[3] = decodeBase64Char(str[3]);

		buf[0] = (buf[0]<<2) + (buf[1]>>4);
		buf[1] = (buf[1]<<4) + (buf[2]>>2);
		buf[2] = (buf[2]<<6) + buf[3];
		buf[3] = '\0';

		res += buf;

		str += 4;
	}

	return res;
}


std::string wchar2string(wchar_t* str)
{
	std::string t;
	while(*str != '\0')
	{
		if (*str&0xFF00) {
			t += (char)(*str>>8);
			t += (char)(*str&0xFF);
		} else {
			t += (char)(*str&0xFF);
		}
		str++;
	}
	return t;
}

std::string char2string(TCHAR* str)
{
	std::string t(str);
	return t;
}


} // namespace utility
} // namespace lib
