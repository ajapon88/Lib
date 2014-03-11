#ifndef __LIB_MEMORY_H__
#define __LIB_MEMORY_H__

#include <lib/define.h>
#include <lib/memory/heap.h>
#include <lib/memory/memory_manager.h>

#ifdef __USE_LIB_MEMORY_SYSTEM
void* operator new(size_t size);
void* operator new(size_t size, const char *tag);
void* operator new(size_t size, const char *tag, int heap_index);
void* operator new[](size_t size);
void* operator new[](size_t size, const char *tag);
void* operator new[](size_t size, const char *tag, int heap_index);
void operator delete(void* p);
void operator delete(void* p, const char *tag);
void operator delete(void* p, const char *tag, int heap_index);
void operator delete[](void* p);
void operator delete[](void* p, const char *tag);
void operator delete[](void* p, const char *tag, int heap_index);
#endif

#endif	// __LIB_MEMORY_H__