#include "stdafx.h"
#include <lib/memory/memory_manager.h>
#include <lib/memory/heap.h>
#include <lib/memory/heap_pool.h>

namespace lib {
namespace memory {
Heap MemoryManager::s_defaultHeap("DefaultHeap");
std::vector<Heap*, SystemAllocator<Heap*> > MemoryManager::s_heapList;

void MemoryManager::createHeap(unsigned int index, const char *name)
{
	if (index >= s_heapList.size()) {
		s_heapList.resize(index+1, NULL);
	}
	ASSERT(!s_heapList[index]);
	s_heapList[index] = NEW("HeapFactory::createHeap") Heap(name);
}
void MemoryManager::createHeapPool(unsigned int index, const char *name,size_t pool_size)
{
	if (index >= s_heapList.size()) {
		s_heapList.resize(index+1, NULL);
	}
	ASSERT(!s_heapList[index]);
	s_heapList[index] = NEW("HeapFactory::createHeapPool") HeapPool(name, pool_size);
}

void MemoryManager::removeHeap(unsigned int index)
{
	SAFE_DELETE(s_heapList[index]);
}

Heap *MemoryManager::getHeap(int index)
{
	if (index == HEAP_DEFAULT) {
		return &s_defaultHeap;
	}
	return s_heapList[index];
}

void MemoryManager::printInfo()
{
	DEBUG_PRINT("-----HeapInfo-----\n");
	s_defaultHeap.printInfo();
	int size = s_heapList.size();
	for (int i = 0; i < size; i++) {
		if (s_heapList[i])  s_heapList[i]->printInfo();
	}
	DEBUG_PRINT("------------------\n");
}

} // namespace memory
} // namespace lib
