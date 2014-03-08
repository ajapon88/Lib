#include <stdio.h>
#include <lib/memory/memory.h>
#include <lib/utility/utility.h>
#include <lib/format/json.h>
#include <lib/file/file.h>

void testMemory();
void testUtility();
void testJson();

int main()
{
	printf("-----StartLibTest-----\n\n");

	testMemory();
	testUtility();
	testJson();
	
	printf("\n-----MemoryLeakCheck-----\n");
	lib::memory::HeapFactory::printInfo();
	printf("-------------------------\n");
	getchar();
	return 0;
}

void testMemory()
{
	printf("-----MomoryTest-----\n");
	lib::memory::HeapFactory::printInfo();
	char *test = NEW("TestNew1") char;
	printf("-----HeapInfo New TestNew1-----\n");
	lib::memory::HeapFactory::printInfo();
	char *test32 = NEW("TestNew32") char[32];
	printf("-----HeapInfo New TestNew32-----\n");
	lib::memory::HeapFactory::printInfo();
	char *test256 = NEW("TestNew256") char[256];
	printf("-----HeapInfo New TestNew256-----\n");
	lib::memory::HeapFactory::printInfo();
	SAFE_DELETE(test);
	printf("-----HeapInfo Delete TestNew-----\n");
	lib::memory::HeapFactory::printInfo();
	SAFE_DELETE(test256);
	printf("-----HeapInfo Delete TestNew256-----\n");
	lib::memory::HeapFactory::printInfo();
	SAFE_DELETE(test32);
	printf("-----HeapInfo Delete TestNew32-----\n");
	lib::memory::HeapFactory::printInfo();
	printf("--------------------\n\n");
}

void testUtility()
{
	printf("-----UtilityTest-----\n");
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
	for (int i = 0; i < ARRAY_SIZE(str); i++) {
		const char *p = str[i];
		printf ("%sは\n", p);
		if (lib::utility::isInt(p)) {
			printf("  整数:%d\n", lib::utility::a2i(p));
		}
		if (lib::utility::isFloat(p)) {
			printf("  実数:%f\n", lib::utility::a2f(p));
		}
		if (lib::utility::isBin(p)) {
			printf("  2進数:%d\n", lib::utility::a2iBin(p));
		}
		if (lib::utility::isOct(p)) {
			printf("  8進数:%d\n", lib::utility::a2iOct(p));
		}
		if (lib::utility::isHex(p)) {
			printf("  16進数:%d\n", lib::utility::a2iHex(p));
		}
		printf("\n");
	}
	printf("\n");
}

void testJson()
{
	printf("-----JsonTest-----\n");
	// Json
	lib::file::FileList filelist;
	lib::file::getFileList(&filelist, "../", "json", true);
	
	filelist.reset();
	while(filelist.nextFile()) {
		lib::file::FileData file(filelist.getFileName());

		lib::format::JsonDocument document;
		document.parse(file.getData());
		if (document.isParseError()) {
			printf("パースエラー(%s): %d行目,'%s'付近\n", file.getFileName(), document.getErrorLineNo(), document.getLastParsePos());
		} else {
			std::string out;
			printf("パース成功(%s): %s\n", file.getFileName(), document.dump(&out));
		}
	}
	printf("\n");
}
