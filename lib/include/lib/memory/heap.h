#ifndef __LIB_MEMORY_HEAP_H__
#define __LIB_MEMORY_HEAP_H__
//#include "stdafx.h"
#include <stdint.h>
#include <vector>
#include <lib/memory/allocator.h>

namespace lib {
namespace memory {
class Heap {
	enum {
		NAMELENGTH = 32,
	};
	struct AllocateHeader {
		uint32_t signature;
		size_t size;
		const char *tag;
		lib::memory::Heap *heap;
		AllocateHeader *prev;
		AllocateHeader *next;
	};

private:
	Heap():m_pHeadAllocate(NULL){}

public:
	Heap(const char *name);
	~Heap();
	
	void *allocate(size_t size, const char *tag = NULL);
	static void deallocate(void *p);
	
	void printInfo() const;

private:
	void deallocate(AllocateHeader *allocate_header);

	char m_name[NAMELENGTH];
    AllocateHeader *m_pHeadAllocate;
};

CREATE_ALLOCATOR(SystemAllocator, "SystemAllocator");

class HeapFactory {
public:
	enum HeapIndex {
		HEAP_DEFAULT = -1,
		HEAP_USER	 = 0,
	};

public:
	static Heap *getDefaultHeap() {
		return &s_defaultHeap;
	}
	static void createHeap(unsigned int index, const char *name);
	static void removeHeap(unsigned int index);
	static Heap *getHeap(int index);
	static void printInfo();

private:
	static Heap s_defaultHeap;
	static std::vector<Heap*, SystemAllocator<Heap*> > s_heapList;
};

} // namespace memory
} // namespace lib

#endif	// __LIB_MEMORY_HEAP_H__