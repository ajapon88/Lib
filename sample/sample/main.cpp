#include <stdio.h>
#include <lib/utility.h>

int main()
{
	const char *str[] = {
		"1234567890",
		"-1234567.0",
		"0.123456",
		"012",
		"0xFF",
		"0010100101",
		".1234567890",
		"123.0e5",
		"-123.0e-5",
	};
	for (int i = 0; i < sizeof(str)/sizeof(str[0]); i++) {
		const char *p = str[i];
		printf ("%s‚Í\n", p);
		if (lib::utility::isInt(p)) {
			printf("  ®”:%d\n", lib::utility::str2Int(p));
		}
		if (lib::utility::isFloat(p)) {
			printf("  ŽÀ”:%f\n", lib::utility::str2Float(p));
		}
		if (lib::utility::isBin(p)) {
			printf("  2i”:%d\n", lib::utility::str2Bin(p));
		}
		if (lib::utility::isOct(p)) {
			printf("  8i”:%d\n", lib::utility::str2Oct(p));
		}
		if (lib::utility::isHex(p)) {
			printf("  16i”:%d\n", lib::utility::str2Hex(p));
		}
		printf("\n");
	}

	getchar();
	return 0;
}