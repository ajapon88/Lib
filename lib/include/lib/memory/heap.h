#ifndef __LIB_MEMORY_HEAP_H__
#define __LIB_MEMORY_HEAP_H__
//#include "stdafx.h"
#include <stdint.h>

namespace lib {
namespace memory {
class Heap {
	struct AllocateHeader {
		uint32_t signature;
		size_t size;
		const char *tag;
		lib::memory::Heap *heap;
		AllocateHeader *prev;
		AllocateHeader *next;
	};
public:
	enum {
		NAMELENGTH = 32,
	};
public:
	Heap(const char *name);
	~Heap();
	
	void *allocate(size_t size, const char *tag = NULL);
	static void deallocate(void *p);
	
	void PrintInfo() const;

private:
	void deallocate(AllocateHeader *allocate_header);

	char m_name[NAMELENGTH];
    AllocateHeader *m_pHeadAllocate;
};

class HeapFactory {
public:

	static Heap *getDefaultHeap() {
		return &m_DefaultHeap;
	}

private:
	static Heap m_DefaultHeap;
};

}
}

#endif	// __LIB_MEMORY_HEAP_H__