#include "stdafx.h"
#include <lib/memory/heap.h>

namespace lib {
namespace memory {

Heap::Heap(const char *name)
: m_pHeadAllocate(NULL) {
	strncpy(m_name, name, NAMELENGTH-1);
	m_name[NAMELENGTH-1] = '\0';
}

Heap::~Heap()
{
//	ASSERT(!m_pHeadAllocate);
	AllocateHeader *header = m_pHeadAllocate;
	while(header) {
		AllocateHeader *next = header->next;
		char *p = ((char*)header + sizeof(AllocateHeader));
		SAFE_DELETE(p);
		header = next;
	}
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
	if (!p)  return;

	AllocateHeader *allocate_header = (AllocateHeader*)((char*)p - sizeof(AllocateHeader));
	ASSERT_MES(allocate_header->signature == ALLOCATE_SIGNATURE, "[Heap] Error: Signature is mismatched.\n");

	uint32_t *endmark = (uint32_t*)((char*)p + allocate_header->size);
	ASSERT_MES(*endmark == ALLOCATE_ENDMARK, "[Heap] Error: Endmark is mismatched.\n");

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

void Heap::printInfo() const
{
	AllocateHeader *header = m_pHeadAllocate;
	DEBUG_PRINT("HeapInfo:%s\n", m_name);
	while(header) {
		if (header->tag) {
			DEBUG_PRINT("  - tag=\"%s\", size=%d, address=%x\n", header->tag, header->size, header);
		} else {
			DEBUG_PRINT("  - no_tag, size=%d, address=%x\n", header->size, header);
		}
		header = header->next;
	}
}

} // namespace memory
} // namespace lib
