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


void * mymalloc(size_t size) {
    Block *current = NULL;
    if (List.head_node == NULL) {
        void *head_ptr = mmap(NULL, PAGE_SIZE);
        if (head_ptr == NULL) {
            return ferror("mmap failed");
        }
        List.head_node = head_ptr;
        current = head_ptr;
    } else {
        Block *prev = NULL;
        current = List.head_node;
        while (!current->is_free) {
            prev = current;
            current = current->next;
        }
        prev->next = current;
    }
    current->size = size;
    current->is_free = false;
    List.available -= current->size + sizeof(current);
    return current;
}

int myfree(Block *to_free) {
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