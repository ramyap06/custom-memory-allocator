#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define PAGE_SIZE (4096)  // 4KB
#define SUCCESS (0)
#define FAILURE (-1)

typedef struct mem_block {
    size_t size;
    struct mem_block *next;
    bool is_free; // if free true, if allocated false
} Block;

struct list {
    Block *head_node;
    size_t available;
} static List;

void *mymalloc(size_t size) {
    Block *reserved = NULL;
    if (List.head_node == NULL) {
        Block *mem_ptr = mmap(NULL, PAGE_SIZE);
        if (mem_ptr == NULL) {
            return ferror("mmap failed");
        }
        List.head_node = mem_ptr;
        reserved = mem_ptr;
    } else {
        Block *prev = NULL;
        Block *curr = List.head_node;
        int size_diff = -1;
        int best_size = 0;
        while (curr != NULL && size_diff != 0) {
            size_diff = curr->size - size;
            if (size_diff < best_size && size_diff >= 0) {
                best_size = size_diff;
            }
            prev = curr;
            curr = curr->next;
        }
        curr = List.head_node;
        while (curr != NULL && size_diff != best_size) {
            size_diff = curr->size - size;
            prev = curr;
            curr = curr->next;
        }
        if (best_size != 0) {
            /*
             * split the block
             * curr = pointer to the current block
             * split current into the reserved block and reserved->next
             * 
            */
            
            
            reserved = curr;
            
            // finish splitting ptrs
        }
    }
    reserved->size = size;
    reserved->is_free = false;
    List.available -= reserved->size + sizeof(reserved);
    return reserved;
}

void *mycalloc(size_t num, size_t size);

void *myrealloc(void *ptr, size_t new_size);

int myfree(void *ptr) {
    Block *to_free = (Block *) ptr;

    if (to_free == NULL) {
        return FAILURE;
    }
    if (to_free->is_free) {
        return FAILURE;
    }
    to_free->is_free = true;
    List.available += to_free->size + sizeof(to_free);
    
    if (to_free->next != NULL) {
        Block *prev = List.head_node;
        Block *after = List.head_node;

        if ((prev == NULL) || (after == NULL)) {
            return FAILURE;
        }

        while (prev->next != to_free) {
            prev = prev->next;
        }
        while (to_free->next != after) {
            after = after->next;
        }
        if (prev->is_free) {
            prev->size += to_free->size;
            prev->next = after;
        }
        if (after->is_free) {
            after->size += to_free->size;
            prev->next = after;
        }
    }
    return SUCCESS;
}