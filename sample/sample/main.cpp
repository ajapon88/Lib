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
		printf ("%s��\n", p);
		if (lib::utility::isInt(p)) {
			printf("  ����:%d\n", lib::utility::a2i(p));
		}
		if (lib::utility::isFloat(p)) {
			printf("  ����:%f\n", lib::utility::a2f(p));
		}
		if (lib::utility::isBin(p)) {
			printf("  2�i��:%d\n", lib::utility::a2b(p));
		}
		if (lib::utility::isOct(p)) {
			printf("  8�i��:%d\n", lib::utility::a2o(p));
		}
		if (lib::utility::isHex(p)) {
			printf("  16�i��:%d\n", lib::utility::a2h(p));
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
			printf("�p�[�X�G���[(%s): %d�s��,'%s'�t��\n", file.getFileName(), json_reader.getErrorLineNo(), json_reader.getLastParsePos());
		} else {
			std::string out;
			printf("�p�[�X����(%s): %s\n", file.getFileName(), json_reader.dump(&out));
		}
	}
}
