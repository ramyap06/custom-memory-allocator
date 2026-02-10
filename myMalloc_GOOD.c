

/*
 * @brief Helper allocate an object given a raw request size from the user
 *
 * @param raw_size number of bytes the user needs
 *
 * @return A block satisfying the user's request
 */
static inline header * allocate_object(size_t raw_size) {
  // TODO implement allocation

  // if the size requested is 0, return NULL
  if (raw_size == 0) {
    return NULL;
  }

  // align the raw allocable size to 8 bytes
  size_t rounded = (raw_size + 7) & ~7;

  if (rounded < ALLOC_HEADER_SIZE) {
    rounded = ALLOC_HEADER_SIZE;
  }
  size_t actual_size = ALLOC_HEADER_SIZE + rounded;

  // choose an appropriate sized free list
  int freelist_idx = (rounded / 8) - 1;
  if (freelist_idx >= N_LISTS) {
    freelist_idx = N_LISTS - 1;
  }

  // find the first fit block
  header *sentinel = &freelistSentinels[freelist_idx];
  header *block = sentinel->next;

  // if no free lists move down to next free list
  for (int i = freelist_idx; i < N_LISTS; i++) {
    sentinel = &freelistSentinels[i];
    block = sentinel->next;
    while ((block != sentinel) && (get_size(block) < actual_size)) {
      block = block->next;
    }
    if (block != sentinel) {
      break;
    }
  }

  // if no free lists at all that fit the size return NULL (replace with calling chunks)
  if (block == sentinel) {
    return NULL;
  }

  // remove block from linked free list by connect block->prev to block->next
  block->prev->next = block->next;
  block->next->prev = block->prev;

  // if the leftover block is larger than a header then it can made into a free block
  size_t leftover_size = get_size(block) - actual_size;

  if (leftover_size >= sizeof(header)) {
    header *free_block = block;
    set_size(free_block, leftover_size);
    free_block->left_size = block->left_size;

    // make a new black that is jumps the size of the free block so it is on the right of it
    // use a cast of char * to actually move 1 byte at a time, the size of the leftover memory
    block = (header *)((char *)free_block + leftover_size);
    set_size(block, actual_size);
    block->left_size = leftover_size;

    // update the left size of the block to the right
    header *right_block = (header *)((char *)block + get_size(block));
    right_block->left_size = get_size(block);

    // find a free list for the new free block made from leftover memory
    int free_idx = (get_size(free_block) - ALLOC_HEADER_SIZE) / 8 - 1;
    if (free_idx >= N_LISTS) {
      free_idx = N_LISTS - 1;
    }

    // add free block to the start of the list, connecting to sentinel->next & sentinel->next->prev
    header *free_sentinel = &freelistSentinels[free_idx];
    free_block->next = free_sentinel->next;
    free_block->prev = free_sentinel;
    free_sentinel->next->prev = free_block;
    free_sentinel->next = free_block;
  }

  // set allocated bit to allocated using OR bitwise operation
  set_state(block, ALLOCATED);

  // return a pointer to the data section of the header where stored memory will start
  return (header *)(block->data);

}

/**
 * @brief Helper to manage deallocation of a pointer returned by the user
 *
 * @param p The pointer returned to the user by a call to malloc
 */
static inline void deallocate_object(void * p) {
  // TODO implement deallocation

  // if the argument is a null pointer return NULL
  if (p == NULL) {
    return;
  }

  // move the block pointer to the start of the block
  header *block = (header *)((char *)p - ALLOC_HEADER_SIZE);

  // if already free alert as Double Free detected
  if (get_state(block) == UNALLOCATED) {
    printf("Double Free Detected");
    printf(":test_double_free: ../myMalloc.c:577: deallocate_object: Assertion `false' failed.");
    abort();
  }

  // change block to free
  set_state(block, UNALLOCATED);

  // if left is unallocated take it out of free list
  header *left_block = (header *)((char *)block - block->left_size);

  if (get_state(left_block) == UNALLOCATED) {
    left_block->prev->next = left_block->next;
    left_block->next->prev = left_block->prev;

    // add to left_size's size and make block point to the start of the left_block
    size_t total_size = get_size(block) + get_size(left_block);
    set_size_and_state(left_block, total_size, UNALLOCATED);
    block = left_block;
  }

  // if right is unallocated take it out of free list
  header *right_block = (header *)((char *)block + get_size(block));

  if (get_state(right_block) == UNALLOCATED) {
    right_block->prev->next = right_block->next;
    right_block->next->prev = right_block->prev;

    // add to block's size and make block point to the start of the block
    size_t total_size = get_size(block) + get_size(right_block);
    set_size(block, total_size);
  }

  // update the new right block to have the free block as its left_size
  header *new_right = (header *)((char *)block + get_size(block));
  new_right->left_size = get_size(block);

  // find new appropriate free list
  int alloc_size = get_size(block) - ALLOC_HEADER_SIZE;
  if (alloc_size < ALLOC_HEADER_SIZE) {
    alloc_size = ALLOC_HEADER_SIZE;
  }
  int free_idx = alloc_size / 8 - 1;
  if (free_idx >= N_LISTS) {
    free_idx = N_LISTS - 1;
  }

  // add free block to appropriate free list
  header *sentinel = &freelistSentinels[free_idx];
  block->next = sentinel->next;
  block->prev = sentinel;
  sentinel->next->prev = block;
  sentinel->next = block;

}