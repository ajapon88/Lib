#ifndef __LIB_MEMORY_HEAP_H__
#define __LIB_MEMORY_HEAP_H__
//#include "stdafx.h"
#include <stdint.h>
#include <vector>
#include <lib/memory/allocator.h>

namespace lib {
namespace memory {

const uint32_t ALLOCATE_SIGNATURE = 0xDEADC0DE;
const uint32_t ALLOCATE_ENDMARK = 0xDEADC0DF;

class Heap {
protected:
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
	virtual ~Heap();
	
	virtual void *allocate(size_t size, const char *tag = NULL);
	static void deallocate(void *p);
	
	void printInfo() const;

protected:
	virtual void deallocate(AllocateHeader *allocate_header);

	char m_name[NAMELENGTH];
    AllocateHeader *m_pHeadAllocate;
};

} // namespace memory
} // namespace lib

#endif	// __LIB_MEMORY_HEAP_H__
