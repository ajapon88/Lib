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
	strncpy(m_name, name, NAMELENGTH-1);
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
	printf("HeapInfo:%s\n", m_name);
	while(header) {
		if (header->tag) {
			printf("  - tag=\"%s\", size=%d\n", header->tag, header->size);
		} else {
			printf("  - no_tag, size=%d\n", header->size);
		}
		header = header->next;
	}
}

Heap HeapFactory::s_defaultHeap("DefaultHeap");
std::vector<Heap*> HeapFactory::s_heapList;

void HeapFactory::createHeap(unsigned int index, const char *name)
{
	if (index >= s_heapList.size()) {
		s_heapList.resize(index+1, NULL);
	}
	ASSERT(!s_heapList[index]);
	s_heapList[index] = NEW("HeapFactory::createHeap") Heap(name);
}

Heap *HeapFactory::getHeap(int index)
{
	if (index == HEAP_DEFAULT) {
		return &s_defaultHeap;
	}
	return s_heapList[index];
}

void HeapFactory::printInfo()
{
	s_defaultHeap.printInfo();
	int size = s_heapList.size();
	for (int i = 0; i < size; i++) {
		if (s_heapList[i])  s_heapList[i]->printInfo();
	}
}

}
}