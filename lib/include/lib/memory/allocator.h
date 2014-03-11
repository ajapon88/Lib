#ifndef __LIB_MEMORY_ALLOCATOR_H__
#define __LIB_MEMORY_ALLOCATOR_H__

#include <lib/define.h>

#define CREATE_HEAP_ALLOCATOR(AllocatorName, Tag, ...) \
template <class T> class AllocatorName { \
public: \
	typedef size_t size_type; \
	typedef ptrdiff_t difference_type; \
	typedef T* pointer; \
	typedef const T* const_pointer; \
	typedef T& reference; \
	typedef const T& const_reference; \
	typedef T value_type; \
\
	template <class U> struct rebind { typedef AllocatorName<U> other; }; \
\
	AllocatorName() throw() {} \
	AllocatorName(const AllocatorName&) throw() {} \
	template <class U> AllocatorName(const AllocatorName<U>&) throw() {} \
	~AllocatorName() throw() {} \
\
	pointer allocate(size_type num) { return (pointer)NEW(Tag, ##__VA_ARGS__) T[num]; }   \
	void construct(pointer p, const_reference value) { new(p) T(value); } \
	void deallocate(pointer p, size_type num) { ::operator delete(p); } \
	void destroy(pointer p) { p->~T(); } \
	pointer address(reference value) const { return &value; } \
	const_pointer address(const_reference value) const { return &value; } \
	size_type max_size() const throw() { return std::numeric_limits<size_t>::max()/sizeof(T); } \
};

#endif	// __LIB_MEMORY_ALLOCATOR_H__