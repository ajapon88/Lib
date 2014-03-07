#include "stdafx.h"
#include <lib/memory/heap.h>

namespace {
const uint32_t ALLOCATE_SIGNATURE = 0xDEADC0DE;
const uint32_t ALLOCATE_ENDMARK = 0xDEADC0DF;
}

namespace lib {
namespace memory {

Heap::Heap(const char *name)
: m_pHeadAllocate(NULL) {
	memcpy(m_name, name, NAMELENGTH-1);
	m_name[NAMELENGTH-1] = '\0';
}

Heap::~Heap()
{
}

void *Heap::allocate(size_t size, const char *tag)
{
	size_t allocate_size = size + sizeof(AllocateHeader) + sizeof(uint32_t);
	char *p = (char*)malloc(allocate_size);
	AllocateHeader *allocate_header = (AllocateHeader*)p;
	allocate_header->signature = ALLOCATE_SIGNATURE;
	allocate_header->size = size;
	allocate_header->tag = tag;
	allocate_header->prev = NULL;
	allocate_header->next = m_pHeadAllocate;
	allocate_header->heap = this;

	uint32_t *endmark = (uint32_t*)(p + sizeof(AllocateHeader) + size);
	*endmark = ALLOCATE_ENDMARK;

	if (m_pHeadAllocate)  m_pHeadAllocate->prev = allocate_header;
	m_pHeadAllocate = allocate_header;
	
	void *allocate_mem = p + sizeof(AllocateHeader);
	
	return allocate_mem;
}

void Heap::deallocate(void *p)
{
	AllocateHeader *allocate_header = (AllocateHeader*)((char*)p - sizeof(AllocateHeader));
	assert(allocate_header->signature == ALLOCATE_SIGNATURE);

	uint32_t *endmark = (uint32_t*)((char*)p + allocate_header->size);
	assert(*endmark == ALLOCATE_ENDMARK);

	allocate_header->heap->deallocate(allocate_header);
}

void Heap::deallocate(AllocateHeader *allocate_header)
{
	AllocateHeader *prev = allocate_header->prev;
	AllocateHeader *next = allocate_header->next;
	
	if (prev) prev->next = next;
	else m_pHeadAllocate = next;
	if (next) next->prev = prev;


	free(allocate_header);
}

void Heap::PrintInfo() const
{
	AllocateHeader *header = m_pHeadAllocate;
	while(header) {
		printf("-%s: tag=%s, size=%d\n", m_name, header->tag, header->size);
		header = header->next;
	}
}

Heap HeapFactory::m_DefaultHeap("DefaultHeap");


}
}