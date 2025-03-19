#include <stdio.h>
#include <stdlib.h>

#define PAGE_SIZE 4096  // 4KB

typedef struct mem_block {
    size_t size;
    struct mem_block *next;
    int status; // if free 0, if allocated 1
};

static struct mem_block *freeList = NULL;

// functions

void mymalloc(size_t size) {
    // allocate a page of memory
    if (freeList == NULL) {
        void *mem = mmap(NULL, PAGE_SIZE);
        if (mem == (void *) -1) {
            perror("sbrk failed");
            return NULL;
        }
        freeList = (struct mem_block *) mem; //now free list points to the start of the memory block
        freeList->next = NULL;
        freeList->size = freeList->size - sizeof(struct mem_block);
        freeList->status = 0;
    }

    // search for free blocks
    struct mem_block *curr = freeList;
    struct mem_block *prev = NULL;
    struct mem_block *best_block = NULL;
    struct mem_block *best_prev = NULL;

    while (curr != NULL) {
        
    }

}
void mycalloc(size_t num, size_t size);
void myrealloc(void *ptr, size_t new_size);
myfree(void *ptr);