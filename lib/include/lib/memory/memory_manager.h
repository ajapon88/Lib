#ifndef __LIB_MEMORY_MANAGER_H__
#define __LIB_MEMORY_MANAGER_H__

#include <lib/memory/allocator.h>
#include <vector>

namespace lib {
namespace memory {

class Heap;

CREATE_HEAP_ALLOCATOR(SystemAllocator, "SystemAllocator", MemoryManager::HEAP_DEFAULT);

class MemoryManager {
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
	static void createHeapPool(unsigned int index, const char *name, size_t pool_size);
	static void removeHeap(unsigned int index);
	static Heap *getHeap(int index);
	static void printInfo();

private:
	static Heap s_defaultHeap;
	static std::vector<Heap*, SystemAllocator<Heap*> > s_heapList;
};

} // namespace memory
} // namespace lib


#endif	// __LIB_MEMORY_MANAGER_H__
