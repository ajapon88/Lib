#include "stdafx.h"
#include <lib/memory/memory.h>
#include <lib/memory/heap.h>


void* operator new(size_t size)
{
	return operator new(size, NULL, lib::memory::HeapFactory::getDefaultHeap());
}

void* operator new(size_t size, const char *tag)
{
	return operator new(size, tag, lib::memory::HeapFactory::getDefaultHeap());
}

void* operator new(size_t size, const char *tag, lib::memory::Heap *heap)
{
	return heap->allocate(size, tag);
}

void* operator new[](size_t size)
{
	return operator new(size, NULL, lib::memory::HeapFactory::getDefaultHeap());
}

void* operator new[](size_t size, const char *tag)
{
	return operator new(size, tag, lib::memory::HeapFactory::getDefaultHeap());
}

void* operator new[](size_t size, const char *tag, lib::memory::Heap *heap)
{
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
