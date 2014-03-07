#ifndef __LIB_MEMORY_H__
#define __LIB_MEMORY_H__

#include <lib/memory/heap.h>

void* operator new(size_t size);
void* operator new(size_t size, const char *tag);
void* operator new(size_t size, const char *tag, lib::memory::Heap *heap);
void* operator new[](size_t size);
void* operator new[](size_t size, const char *tag);
void* operator new[](size_t size, const char *tag, lib::memory::Heap *heap);
void operator delete(void* p);
void operator delete(void* p, const char *tag);
void operator delete(void* p, const char *tag, lib::memory::Heap *heap);
void operator delete[](void* p);
void operator delete[](void* p, const char *tag);
void operator delete[](void* p, const char *tag, lib::memory::Heap *heap);

#endif	// __LIB_MEMORY_H__