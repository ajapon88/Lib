#include <stdio.h>
#include <time.h>
#include <lib/memory/memory.h>
#include <lib/utility/utility.h>
#include <lib/format/json.h>
#include <lib/file/file.h>

void testMemory();
void testUtility();
void testJson();

using lib::memory::MemoryManager;

int main()
{
	printf("-----StartLibTest-----\n\n");
	testMemory();
	testUtility();
	testJson();
	
	printf("\n-----MemoryLeakCheck-----\n");
	MemoryManager::printInfo();
	printf("-------------------------\n");
	getchar();
	return 0;
}

void testMemory()
{
	printf("-----MomoryTest-----\n");
	MemoryManager::printInfo();
	{
		char *test = NEW("TestNew1") char;
		printf("-----HeapInfo New TestNew1-----\n");
		MemoryManager::printInfo();
		char *test32 = NEW("TestNew32") char[32];
		printf("-----HeapInfo New TestNew32-----\n");
		MemoryManager::printInfo();
		char *test256 = NEW("TestNew256") char[256];
		printf("-----HeapInfo New TestNew256-----\n");
		MemoryManager::printInfo();
		SAFE_DELETE(test);
		printf("-----HeapInfo Delete TestNew-----\n");
		MemoryManager::printInfo();
		SAFE_DELETE(test256);
		printf("-----HeapInfo Delete TestNew256-----\n");
		MemoryManager::printInfo();
		SAFE_DELETE(test32);
		printf("-----HeapInfo Delete TestNew32-----\n");
		MemoryManager::printInfo();
		printf("\n");
	}
	{
		printf("-----HeapTest-----\n");
		enum {
			HEAP_TEST1 = MemoryManager::HEAP_USER,
			HEAP_TEST2,
		};
		MemoryManager::createHeap(HEAP_TEST1, "TestHeap1");
		MemoryManager::createHeap(HEAP_TEST2, "TestHeap2");
		char *test = NEW("TestNew") char;
		char *test1 = NEW("TestNew1", HEAP_TEST1) char;
		char *test2 = NEW("TestNew2", HEAP_TEST2) char;
		MemoryManager::printInfo();
		SAFE_DELETE(test);
		SAFE_DELETE(test1);
		SAFE_DELETE(test2);
		MemoryManager::removeHeap(HEAP_TEST1);
		MemoryManager::removeHeap(HEAP_TEST2);
		printf("\n");
	}
	{
		printf("-----HeapPoolTest-----\n");
		enum {
			HEAP_POOL_TEST = MemoryManager::HEAP_USER,
		};
		MemoryManager::createHeapPool(HEAP_POOL_TEST, "TestHeapPool", 1024);
		char *test1 = NEW("TestNew1", HEAP_POOL_TEST) char;
		char *test2 = NEW("TestNew2", HEAP_POOL_TEST) char;
		MemoryManager::printInfo();
		SAFE_DELETE(test1);
		char *test3 = NEW("TestNew3", HEAP_POOL_TEST) char;
		MemoryManager::printInfo();
		SAFE_DELETE(test2);
		SAFE_DELETE(test3);
		MemoryManager::removeHeap(HEAP_POOL_TEST);
		printf("\n");
	}
	{
		printf("-----HeapPoolSpeedTest-----\n");
		enum {
			HEAP_NORMAL = MemoryManager::HEAP_USER,
			HEAP_POOL,
		};
		MemoryManager::createHeap(HEAP_NORMAL, "HeapNormal");
		MemoryManager::createHeapPool(HEAP_POOL, "HeapPool", 1024);
		clock_t t0;
		t0 = clock();
		for (int i = 0; i < 1024*1024; i++) {
			char *buff = NEW("Normal", HEAP_NORMAL) char[256];
			SAFE_DELETE(buff);
		}
		printf("Normal:%f\n", (double)(clock()-t0)/CLOCKS_PER_SEC);
		t0 = clock();
		for (int i = 0; i < 1024*1024; i++) {
			char *buff = NEW("Pool", HEAP_POOL) char[256];
			SAFE_DELETE(buff);
		}
		printf("Pool  :%f\n", (double)(clock()-t0)/CLOCKS_PER_SEC);
		MemoryManager::removeHeap(HEAP_NORMAL);
		MemoryManager::removeHeap(HEAP_POOL);
		printf("\n");
	}
	MemoryManager::printInfo();
	printf("\n");
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
