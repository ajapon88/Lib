#include <stdio.h>
#include <lib/utility.h>
#include <lib/json_parser.h>
#include <lib/file.h>

void testUtility();
void testJson();

int main()
{
	testUtility();
	testJson();
	getchar();
	return 0;
}


void testUtility()
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
		printf ("%sは\n", p);
		if (lib::utility::isInt(p)) {
			printf("  整数:%d\n", lib::utility::a2i(p));
		}
		if (lib::utility::isFloat(p)) {
			printf("  実数:%f\n", lib::utility::a2f(p));
		}
		if (lib::utility::isBin(p)) {
			printf("  2進数:%d\n", lib::utility::a2b(p));
		}
		if (lib::utility::isOct(p)) {
			printf("  8進数:%d\n", lib::utility::a2o(p));
		}
		if (lib::utility::isHex(p)) {
			printf("  16進数:%d\n", lib::utility::a2h(p));
		}
		printf("\n");
	}
}

void testJson()
{
	// Json
	lib::file::FileList filelist;
	lib::file::getFileList(&filelist, "../", "json", true);
	
	filelist.reset();
	while(filelist.nextFile()) {
		lib::file::FileData file(filelist.getFileName());

		lib::JsonReader json_reader;
		json_reader.parse(file.getData());
		if (json_reader.isParseError()) {
			printf("パースエラー(%s): %d行目,'%s'付近\n", file.getFileName(), json_reader.getErrorLineNo(), json_reader.getLastParsePos());
		} else {
			std::string out;
			printf("パース成功(%s): %s\n", file.getFileName(), json_reader.dump(&out));
		}
	}
}
