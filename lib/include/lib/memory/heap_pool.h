#ifndef __LIB_HEAP_POOL_H__
#define __LIB_HEAP_POOL_H__

#include <lib/memory/heap.h>
#include <lib/memory/memory_manager.h>

namespace lib {
namespace memory {

class HeapPool : public Heap
{
public:
	HeapPool(const char *name, size_t pool_size, unsigned int heap_index = MemoryManager::HEAP_DEFAULT);
	virtual ~HeapPool();
	
	virtual void *allocate(size_t size, const char *tag = NULL);

protected:
	virtual void deallocate(AllocateHeader *allocate_header);
	size_t m_poolSize;
	char *m_pPool;
};

} // namespace memory
} // namespace lib

#endif