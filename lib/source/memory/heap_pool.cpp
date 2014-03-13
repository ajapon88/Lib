#include "stdafx.h"

#include <lib/memory/heap_pool.h>

namespace lib {
namespace memory {
HeapPool::HeapPool(const char *name, size_t pool_size, unsigned int heap_index)
: Heap(name)
, m_poolSize(pool_size)
, m_pPool(NULL)
{
	m_pPool = NEW("HeapPool::Pool", heap_index) char[m_poolSize];
}

HeapPool::~HeapPool()
{
	SAFE_DELETE(m_pPool);
}
	
void *HeapPool::allocate(size_t size, const char *tag)
{
	size_t allocate_size = size + sizeof(AllocateHeader) + sizeof(uint32_t);

	char *pool = m_pPool;
	if (m_pHeadAllocate) {
		AllocateHeader *ah = m_pHeadAllocate;
		while (ah && pool - (char*)ah < allocate_size) {
			pool = (char*)ah + (sizeof(AllocateHeader) + sizeof(uint32_t) + ah->size);
			ah = ah->next;
		}
	}
	if ((m_pPool+m_poolSize)-pool < allocate_size) {
		DEBUG_PRINT("[HeapPool::allocate] Error: Heap pool size is insufficient. HeapName=\"%s\", tag=\"%s\"\n", m_name, tag);
		return NULL;
	}

	char *p = new((void*)pool) char[allocate_size];
	AllocateHeader *allocate_header = (AllocateHeader*)p;
	allocate_header->signature = ALLOCATE_SIGNATURE;
	allocate_header->size = size;
	allocate_header->tag = tag;
	allocate_header->prev = NULL;
	allocate_header->next = NULL;
	allocate_header->heap = this;

	uint32_t *endmark = (uint32_t*)(p + sizeof(AllocateHeader) + size);
	*endmark = ALLOCATE_ENDMARK;

	if (!m_pHeadAllocate) m_pHeadAllocate = allocate_header;
	else if (allocate_header < m_pHeadAllocate) {
		allocate_header->prev = NULL;
		allocate_header->next = m_pHeadAllocate;
		m_pHeadAllocate->prev = allocate_header;
		m_pHeadAllocate = allocate_header;
	} else {
		AllocateHeader *ah = m_pHeadAllocate;
		while (ah->next && ah->next < allocate_header) ah = ah->next;
		allocate_header->next = ah->next;
		ah->next = allocate_header;
		allocate_header->prev = ah;
		if (allocate_header->next)  allocate_header->next->prev = allocate_header;
	}

	void *allocate_mem = p + sizeof(AllocateHeader);
	
	return allocate_mem;
}

void HeapPool::deallocate(AllocateHeader *allocate_header)
{
	AllocateHeader *prev = allocate_header->prev;
	AllocateHeader *next = allocate_header->next;
	
	if (prev) prev->next = next;
	else m_pHeadAllocate = next;
	if (next) next->prev = prev;

//	delete(allocate_header, (void*)m_pPool);
}


} // namespace memory
} // namespace lib