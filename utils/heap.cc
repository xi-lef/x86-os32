#include "utils/heap.h"
#include "utils/memutil.h"
#include "debug/assert.h"
#include "debug/output.h"
#include "guard/secure.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

static const size_t ALIGNMENT = sizeof(size_t); // must be power of 2 and at least 4
#define align(val) ((val + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))
#define is_aligned(val) (((size_t)val & (ALIGNMENT - 1)) == 0)

// use bottom 2 bits because min alignment is 4
static const size_t USED_BIT = 0x1;
static const size_t RESERVED_BIT = 0x2;
static const size_t FLAG_BITS = USED_BIT | RESERVED_BIT;

struct Memblock
{
	size_t size; // bottom bits are used as flags (see above)
	uint8_t mem[0];

	/* the actual structure of a memblock is:
	 *
	 * size_t size_with_flags;
	 * u8 mem[size_without_flags];
	 * size_t size_without_flags;
	 */
};

struct FreelistNode
{
	FreelistNode *prev;
	FreelistNode *next;
};

// don't change unless heap structure changes
static const size_t MEMBLOCK_OVERHEAD = sizeof(Memblock) + sizeof(size_t);
static const size_t MIN_BLOCK_SIZE = MAX(ALIGNMENT, sizeof(FreelistNode));
//static const size_t MAX_BLOCK_SIZE = HEAP_SIZE - MEMBLOCK_OVERHEAD;

static inline void write_size_fields(Memblock *block, size_t size, size_t flags);
static inline void free_list_insert(Memblock *block);

struct Heap
{
	FreelistNode free_list_sentinel; // TODO size classes
	union
	{
		Memblock head;
		uint8_t _mem[HEAP_SIZE];
	};

	Heap()
		{
			free_list_sentinel.prev = &free_list_sentinel;
			free_list_sentinel.next = &free_list_sentinel;
			write_size_fields(&head, HEAP_SIZE - MEMBLOCK_OVERHEAD, 0);
			free_list_insert(&head);
		}
};

static Heap heap;

// don't change unless heap structure changes
static const void *MIN_HEAP_PTR = heap._mem + sizeof(Memblock);
static const void *MAX_HEAP_PTR = heap._mem + HEAP_SIZE - sizeof(size_t);
static const Memblock *MIN_MEMBLOCK_POINTER = (Memblock *)heap._mem;
static const Memblock *MAX_MEMBLOCK_POINTER = (Memblock *)(heap._mem + HEAP_SIZE - MEMBLOCK_OVERHEAD - MIN_BLOCK_SIZE);

static inline bool is_heap_pointer(void *ptr)
{
	return (ptr <= MAX_HEAP_PTR
		&& ptr >= MIN_HEAP_PTR);
}

static inline size_t get_size(Memblock *block)
{
	return (block->size & (~FLAG_BITS));
}

static inline size_t get_flags(Memblock *block)
{
	return (block->size & FLAG_BITS);
}

static inline bool memblock_valid(Memblock *block)
{
	return (block >= MIN_MEMBLOCK_POINTER
		&& block <= MAX_MEMBLOCK_POINTER);
}

static inline bool is_free(Memblock *block)
{
	return !(block->size & USED_BIT);
}

static inline void mark_used(Memblock *block)
{
	block->size |= USED_BIT;
}

static inline void mark_free(Memblock *block)
{
	block->size &= ~USED_BIT;
}

static inline void free_list_insert(Memblock *block)
{
	assert(is_free(block));
	FreelistNode *free_block = (FreelistNode *)block->mem;

	free_block->next = heap.free_list_sentinel.next;
	free_block->prev = &heap.free_list_sentinel;

	heap.free_list_sentinel.next = free_block;
	free_block->next->prev = free_block;
}

static inline void free_list_remove(Memblock *block)
{
	FreelistNode *free_block = (FreelistNode *)block->mem;

	FreelistNode *prev = free_block->prev;
	FreelistNode *next = free_block->next;
	prev->next = next;
	next->prev = prev;
}

static inline Memblock *get_block(FreelistNode *free_block)
{
	return ((Memblock *)free_block - 1);
}

static inline void write_size_fields(Memblock *block, size_t size, size_t flags)
{
	assert((size & FLAG_BITS) == 0);
	assert((flags & ~FLAG_BITS) == 0);

	block->size = size | flags;
	*(size_t *)(block->mem + size) = size;
}

static inline Memblock *get_next(Memblock *block)
{
	size_t *block_size = (size_t *)(block->mem + get_size(block));
	if (get_size(block) != *block_size) {
		// Panic::panic("Heap: Corruption detected!!!");
		return NULL;
	}

	Memblock *next = (Memblock *)(block_size + 1);
	if (next > MAX_MEMBLOCK_POINTER) {
		return 0;
	}
	return next;
}

static inline Memblock *get_previous(Memblock *block)
{
	size_t *prev_size = (size_t *)block - 1;
	Memblock *prev = (Memblock *)((uint8_t *)prev_size - *prev_size - sizeof(Memblock));

	if (get_size(prev) != *prev_size) {
		// Panic::panic("Heap: Corruption detected!!!");
		return NULL;
	}

	if (prev < MIN_MEMBLOCK_POINTER) {
		return 0;
	}
	return prev;
}

// returns a pointer to the merged block
// flags of the returned block match flags of the original block
static Memblock *try_merge(Memblock *block)
{
	Memblock *result = block;
	size_t orig_size = get_size(block);
	size_t new_size = orig_size;

	Memblock *prev = get_previous(block);
	if (prev && is_free(prev)) {
		free_list_remove(prev);
		result = prev; // if we merge with prev the address changes
		new_size += get_size(prev) + MEMBLOCK_OVERHEAD;
	}

	Memblock *next = get_next(block);
	if (next && is_free(next)) {
		free_list_remove(next);
		new_size += get_size(next) + MEMBLOCK_OVERHEAD;
	}

	if (new_size != orig_size) {
		assert(is_aligned(new_size));
		write_size_fields(result, new_size, get_flags(block));
	}
	return result;
}

// returns a pointer to a block with at least *size* free bytes
// flag bits of the returned block match the original block
static Memblock *maybe_split_block(Memblock *block, size_t size)
{
	size = MAX(align(size), MIN_BLOCK_SIZE);
	if (get_size(block) >= size + MEMBLOCK_OVERHEAD + MIN_BLOCK_SIZE) {
		// splitting will produce a new block with at least minimal size

		size_t flags = get_flags(block);
		size_t new_size = get_size(block) - size - MEMBLOCK_OVERHEAD;
		assert(new_size >= MIN_BLOCK_SIZE);
		write_size_fields(block, new_size, 0);
		free_list_insert(block);

		block = get_next(block);
		assert(block);
		write_size_fields(block, size, flags);

	}
	return block;
}

HeapStats get_heap_stats()
{
	size_t used = 0;
	size_t free = 0;
	size_t used_blocks = 0;
	size_t free_blocks = 0;
	for (Memblock *cur = &heap.head; cur; cur = get_next(cur)) {
		if (is_free(cur)) {
			free += get_size(cur);
			free_blocks++;
		} else {
			used += get_size(cur);
			used_blocks++;
		}
	}
	HeapStats result;
	result.free = free;
	result.free_blocks = free_blocks;
	result.used = used;
	result.used_blocks = used_blocks;
	return result;
}

static inline Memblock *find_free_block(size_t size)
{
	FreelistNode *free_block = heap.free_list_sentinel.next;
	while (free_block != &heap.free_list_sentinel) {
		Memblock *block = get_block(free_block);
		if (get_size(block) >= size) {
			return block;
		}
		free_block = free_block->next;
	}
	return NULL;
}

static Memblock *__malloc(size_t size)
{
	Memblock *block = find_free_block(size);
	if (!block) {
		// Panic::panic("Heap: Out of memory!!!");
		return NULL;
	}
	assert(is_free(block));
	mark_used(block);
	// we remove then probably insert again in maybe_split_block...
	free_list_remove(block);

	// found a block that's big enough and not in use
	// now try to split it before returning it
	block = maybe_split_block(block, size);

	return block;
}

void *malloc(size_t size)
{
	if (size == 0) {
		assert(!"size == 0");
		return 0;
	}
	Memblock *block = __malloc(size);
	assert(is_heap_pointer(block->mem));
	return block->mem;
}

void *calloc(size_t nmemb, size_t size)
{
	if (nmemb > 0 && size <= -1/nmemb) {
		size_t total = nmemb * size;
		void *p = malloc(total);
		if (p != 0)
			return memset(p, 0, total);
	}
	return 0;
}

static void __free(Memblock *block)
{
	// if this changes __krealloc has to change as well
	block = try_merge(block);
	mark_free(block);
	free_list_insert(block);
}

void free(void *ptr)
{
	Memblock *block = (Memblock *)ptr - 1;
	if(!memblock_valid(block) || is_free(block)) {
		assert(!"Invalid pointer");
		return;
	}
	__free(block);
}

static Memblock *__realloc(Memblock *block, size_t size)
{
	// first try to merge as much as possible to combat fragmentation
	// after that if the block is big enough try splitting again
	// if not relocate (alloc + memcpy + free)
	// TODO is this really the best order?

	block = try_merge(block);

	if (get_size(block) < size) {
		// block size is too small even after merging
		Memblock *new_block = __malloc(size);
		memcpy(new_block->mem, block->mem, size);

		// this is__kfree without the merge since we already did that
		mark_free(block);
		free_list_insert(block);

		return new_block;
	}

	// block might be to big after merging or user just wanted to shrink the block
	block = maybe_split_block(block, size);

	return block;
}

void *realloc(void *ptr, size_t size)
{
	if (size == 0) {
		assert(!"size == 0");
		free(ptr);
		return 0;
	}

	if (!ptr) {
		assert(!"Nullpointer");
		return malloc(size);
	}

	Memblock *block = (Memblock *)ptr - 1;
	if(!memblock_valid(block) || is_free(block)) {
		assert(!"Invalid pointer");
		return 0;
		// return ptr; ?
	}

	block = __realloc(block, size);
	assert(is_heap_pointer(block->mem));
	return block->mem;
}

void *operator new(size_t size) {
    return malloc(size);
}
