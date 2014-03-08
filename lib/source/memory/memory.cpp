#include "stdafx.h"
#include <lib/memory/memory.h>
#include <lib/memory/heap.h>

#ifdef __USE_LIB_MEMORY_SYSTEM

void* operator new(size_t size)
{
	return operator new(size, NULL, lib::memory::HeapFactory::HEAP_DEFAULT);
}

void* operator new(size_t size, const char *tag)
{
	return operator new(size, tag, lib::memory::HeapFactory::HEAP_DEFAULT);
}

void* operator new(size_t size, const char *tag, int heap_index)
{
	lib::memory::Heap *heap = lib::memory::HeapFactory::getHeap(heap_index);
	ASSERT_MES(heap, "[Memory] Error: Heap not exist. index=%d\n", heap_index);
	return heap->allocate(size, tag);
}

void* operator new[](size_t size)
{
	return operator new(size, NULL, lib::memory::HeapFactory::HEAP_DEFAULT);
}

void* operator new[](size_t size, const char *tag)
{
	return operator new(size, tag, lib::memory::HeapFactory::HEAP_DEFAULT);
}

void* operator new[](size_t size, const char *tag, int heap_index)
{
	lib::memory::Heap *heap = lib::memory::HeapFactory::getHeap(heap_index);
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

void operator delete(void* p, const char *tag, lib::memory::Heap *heap)
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

void operator delete[](void* p, const char *tag, lib::memory::Heap *heap)
{
	lib::memory::Heap::deallocate(p);
}

#endif