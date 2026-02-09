/*
 * @brief Helper allocate an object given a raw request size from the user
 *
 * @param raw_size number of bytes the user needs
 *
 * @return A block satisfying the user's request
 */
static inline header * allocate_object(size_t raw_size) {
  // TODO implement allocation

  // if raw size is 0 bytes no allocation is needed so return NULL pointer
  if (raw_size == 0) {
    return NULL;
  }

  // round raw_size to a multiple of 8 bytes for alignment
  raw_size = (raw_size + 7) & ~7;

  int min_size = sizeof(header);

  if (raw_size > 16) {
    min_size += raw_size - 16;
  }

  int freelist_idx = raw_size / 8;

  if (freelist_idx >= N_LISTS) {
    freelist_idx = N_LISTS - 1;
  }

  header * sentinel = &freelistSentinels[freelist_idx];
  header * alloc_block = sentinel->next;

  while (alloc_block != sentinel && min_size > alloc_block->size_state) {
    alloc_block = alloc_block->next;
  }
  if (alloc_block == sentinel) {
    return NULL;
  }

  int size_diff = alloc_block->size_state - min_size;
  if (min_size < alloc_block->size_state &&
     (size_diff >= sizeof(header))) {

    // split the free block
    header * block_one = alloc_block;
    header * temp = block_one->next;
    header * block_two = block_one->next;
    block_two->next = temp;
    block_two->prev = block_one;
    block_one->left_size = alloc_block->left_size;


    if (min_size >= size_diff) {
      block_one->size_state = size_diff;
      block_two->size_state = min_size;
      block_two->left_size = size_diff + block_one->size_state;
    } else {
      block_one->size_state = min_size;
      block_two->size_state = size_diff;
      block_two->left_size = min_size + block_one->size_state;
    }

    int min_cap = (8 * freelist_idx) + 1;

    // add the actual free block to a different list based on memory size
    if (block_one->size_state < min_cap) {
      for (int i = freelist_idx; i > 0; i--) {
        min_cap = (8 * freelist_idx) + 1;
        if (block_one->size_state >= min_cap) {
          freelist_idx = i;
          break;
        }
      }
      
      sentinel = &freelistSentinels[freelist_idx];
      header * curr = sentinel->next;

      while (curr != sentinel) {
        if (curr->size_state > block_one->size_state) {
          break;
        }
      }
      block_one->prev = curr;
      block_one->next = curr->next;
      block_one->next->prev = block_one;
      curr->next = block_one;
    }
    alloc_block = block_two;
  }

  // remove to be allocated block from free list
  alloc_block->prev->next = alloc_block->next;

  // change alloc_block attributes to reflect allocation
  alloc_block->size_state |= ALLOCATED;

  // return a pointer to the header of the newly allocated block
  return (header *)alloc_block->data;
}