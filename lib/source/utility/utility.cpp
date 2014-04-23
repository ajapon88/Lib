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
		case 0:	// À”•”
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
		case 1:	// ¬”•”
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
		case 2:	// w”•”
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


} // namespace utility
} // namespace lib
