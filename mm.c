/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "This struct caused a segfault",
    /* First member's full name */
    "Mohamed Elnakeeb",
    /* First member's email address */
    "nakeeb@kaist.ac.kr",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};


/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

/* Basic constants and macros */
#define MIN_PO2     5              // The minimum power of 2 in a block
#define MIN_SIZE    MIN_PO2 << 1    // The minimum size in bytes	
#define ASIZE       4       /* How many bytes a pointer takes*/
#define WSIZE       4       /* Word and header/footer size (bytes) */
#define DSIZE       8       /* Double word size (bytes) */
#define CHUNKSIZE   (1<<12) /* Extend heap by this amount (bytes) */

#define MAX(x, y)   ((x) > (y) ? (x) : (y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc)   ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p)      (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

/* Read and write a double word at adress p*/
#define GETL(p)      (*(unsigned int *)(p))
#define PUTL(p, val) (*(unsigned long long *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p)     (GET(p) & ~0x7)
#define GET_ALLOC(p)    (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp)    ((char *)(bp) - WSIZE)
#define FTRP(bp)    ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp)   ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp)   ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))


#define FL_SIZE 12 // size of freelist
char *mem_heap;// first byte of heap
/*Finds the the head of each freelist from the start of the heap*/
#define FREE_LIST(index) ((void **)mem_heap + index)




/* 
 * mm_init - initialize the malloc package.
 */
static int ind(size_t size){
	int index = 0;
	if(size <= MIN_SIZE)
		return index;
	size >>= MIN_PO2;
	while(size > 1 && index < FL_SIZE - 1)
		index++, size>>=1;
	return index;
}

static void insert_free(void *bp){
	//printf("insert_free: %p\n", bp);
	int index = ind(GET_SIZE(HDRP(bp)));
	void *ptr = *FREE_LIST(index);
	if(ptr == bp) return;
	*(void **)bp = NULL;
	*(void **)((char *)bp + ASIZE) = NULL;
	*FREE_LIST(index) = bp;
	if(ptr != NULL){
		*(void **)((char *)bp + ASIZE) = ptr;
		*(void **)(ptr) = bp;
		//printf("ptr: %p, prev: %p, next: %p\n", ptr, *(void **)(ptr), *(void **)((char *)ptr + ASIZE));
	}
	//printf("index: %d, freelist: %p, next: %p\n", index, *FREE_LIST(index), *(void **)((char *)bp + ASIZE));
}

static void remove_free(void *bp){
	//printf("remove_free: %p\n", bp);
	int index = ind(GET_SIZE(HDRP(bp)));
	void *prev = *(void **)(bp);
	void *next = *(void **)((char *)bp + ASIZE);
	//printf("index: %d, free_list: %p, prev: %p, next: %p\n", index,*FREE_LIST(index), prev, next);
	if(prev == NULL) *FREE_LIST(index) = next;
	else *(void **)((char *)prev + ASIZE) = next;

	if(next != NULL) *(void **)(next) = prev;
}

static void *coalesce(void *bp) {
	//printf("Coalesce: %p\n", bp);
	void *prev = PREV_BLKP(bp);
	void *next = NEXT_BLKP(bp);
	size_t p_alloc = GET_ALLOC(FTRP(prev));
	size_t n_alloc = GET_ALLOC(HDRP(next));
	size_t size = GET_SIZE(HDRP(bp));
	//printf("size: %d, next alloc: %d, prev alloc: %d\n", size, n_alloc, p_alloc);
	if (!p_alloc) {
		bp = prev;
		size += GET_SIZE(HDRP(prev));
		remove_free(prev);
	}

	if (!n_alloc) {
		size += GET_SIZE(HDRP(next));
		remove_free(next);
	}

	PUT(HDRP(bp), PACK(size, 0));
	PUT(FTRP(bp), PACK(size, 0));
	insert_free(bp);

	return bp;
}


static void* extend_heap(int words){
	char *bp;
	int size = ALIGN(words * WSIZE);
	bp = mem_sbrk(size);
	if(bp == (void *)-1)
		return NULL;
	PUT(HDRP(bp), PACK(size, 0)); //overwrites old epilogue bloc
	PUT(FTRP(bp), PACK(size, 0));
	PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));
	bp = coalesce(bp);
	return bp;
}

int mm_init(void)
{
	mem_heap = mem_sbrk(WSIZE * 20);
	//printf("top of the heap: %p\n", mem_heap);
	if(mem_heap == (void *)-1) return -1;
	PUT(mem_heap + 19 * WSIZE, PACK(0,1));
	PUT(mem_heap + 18 * WSIZE, PACK(DSIZE, 1));
	PUT(mem_heap + 17 * WSIZE, PACK(DSIZE, 1));
	for(int i = 0; i < FL_SIZE; i++){
		//printf("Freelist %d = %p\n", i, FREE_LIST(i));
		*FREE_LIST(i) = NULL;
	}
	if(extend_heap(CHUNKSIZE/WSIZE) == NULL)
		return -1;
    return 0;
}

static void* fit(size_t size){
	//printf("fit: %d\n", size);
	void *bp;
	int index = ind(size);
	for (int i = index; i < FL_SIZE; i++) {
		bp = *FREE_LIST(i);
		if(bp != NULL){
			//printf("bp: %p\n", bp);
			if (GET_SIZE(HDRP(bp)) >= size) return bp;
		}
	}
	bp = *FREE_LIST(index);
	while(bp != NULL){
		if (GET_SIZE(HDRP(bp)) >= size) return bp;
		bp = *(void **)(bp + ASIZE);
	}

	return NULL;
}

static void* split(void *bp, size_t asize){
	size_t bsize = GET_SIZE(HDRP(bp)); //block size
	remove_free(bp);
	//printf("split, Block size: %d, requested: %d\n", bsize, asize);
	if ((bsize - asize) >= MIN_SIZE) {// if difference is more than the minimum block size
		size_t rsize = bsize - asize; //remaining size
		void *free_block = (void *)((char *)bp + asize); //new freed block
		PUT(HDRP(free_block), PACK(rsize, 0));
		PUT(FTRP(free_block), PACK(rsize, 0));
		PUT(HDRP(bp), PACK(asize, 1));
		PUT(FTRP(bp), PACK(asize, 1));
		coalesce(free_block);
		return bp; //return the actual size as the size of the allocated block
	}
	PUT(HDRP(bp), PACK(bsize, 1));
	PUT(FTRP(bp), PACK(bsize, 1));
	return bp;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size){
	size_t asize;
	void *bp;
	if (size == 0) return NULL;

	asize = ALIGN(size + DSIZE);
	asize = MAX(asize, MIN_SIZE);
	bp = fit(asize);

	if (bp == NULL) {
		if ((bp = extend_heap(MAX(asize, CHUNKSIZE) / WSIZE)) == NULL)
			return NULL;
	}

	split(bp, asize);
	return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *bp){
	size_t size = GET_SIZE (HDRP(bp));
	PUT(HDRP(bp), PACK(size, 0));
	PUT(FTRP(bp), PACK(size, 0));
	coalesce(bp);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size){
	if (ptr == NULL) return mm_malloc(size); 

	if (size == 0){
		mm_free(ptr);
		return NULL;
	}

	size = ALIGN(size + DSIZE);
	size = MAX(size, MIN_SIZE);

	size_t old_size = GET_SIZE(HDRP(ptr));
	if(old_size >= size) return split(ptr, size);

	void *next_block = NEXT_BLKP(ptr);
	if(!GET_ALLOC(HDRP(next_block)) && (GET_SIZE(HDRP(next_block)) + old_size) >= size){
		remove_free(next_block);
		size_t new_size = old_size + GET_SIZE(HDRP(next_block));
		PUT(HDRP(ptr), PACK(new_size, 1));
		PUT(FTRP(ptr), PACK(new_size, 1));
		return ptr;
	}

	void *new_ptr = mm_malloc(size);
	if(new_ptr == NULL) return NULL;
	memcpy(new_ptr, ptr, old_size - DSIZE);
	mm_free(ptr);

	return new_ptr;
}
