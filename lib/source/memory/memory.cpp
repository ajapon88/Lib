#include "stdafx.h"
#include <lib/memory/memory.h>
#include <lib/memory/memory_manager.h>

#ifdef __USE_LIB_MEMORY_SYSTEM

using lib::memory::MemoryManager;

void* operator new(size_t size)
{
	return operator new(size, NULL, MemoryManager::HEAP_DEFAULT);
}

void* operator new(size_t size, const char *tag)
{
	return operator new(size, tag, MemoryManager::HEAP_DEFAULT);
}

void* operator new(size_t size, const char *tag, int heap_index)
{
	lib::memory::Heap *heap = MemoryManager::getHeap(heap_index);
	ASSERT_MES(heap, "[Memory] Error: Heap not exist. index=%d\n", heap_index);
	return heap->allocate(size, tag);
}

void* operator new[](size_t size)
{
	return operator new(size, NULL, MemoryManager::HEAP_DEFAULT);
}

void* operator new[](size_t size, const char *tag)
{
	return operator new(size, tag, MemoryManager::HEAP_DEFAULT);
}

void* operator new[](size_t size, const char *tag, int heap_index)
{
	lib::memory::Heap *heap = MemoryManager::getHeap(heap_index);
	ASSERT_MES(heap, "[Memory] Error: Heap not exist. index=%d\n", heap_index);
	return heap->allocate(size, tag);
}

void operator delete(void* p)
{
	lib::memory::Heap::deallocate(p);
}

void operator delete(void* p, const char *tag)
{
	lib::memory::Heap::deallocate(p);
}

void operator delete(void* p, const char *tag, int heap_index)
{
	lib::memory::Heap::deallocate(p);
}

void operator delete[](void* p)
{
	lib::memory::Heap::deallocate(p);
}

void operator delete[](void* p, const char *tag)
{
	lib::memory::Heap::deallocate(p);
}

void operator delete[](void* p, const char *tag, int heap_index)
{
	lib::memory::Heap::deallocate(p);
}

#endif