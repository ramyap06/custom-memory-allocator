static inline header * allocate_object(size_t raw_size) {
  // TODO implement allocation

  // if the size requested is 0, return NULL
  if (raw_size == 0) {
    return NULL;
  }

  // align the raw allocable size to 8 bytes
  raw_size = (raw_size + 7) & ~7;
  size_t actual_size = sizeof(header);
  if (raw_size > 16) {
    actual_size += raw_size - 16;
  }
  
  // choose an appropriate sized free list
  int freelist_idx = actual_size / 8;
  if (freelist_idx >= N_LISTS) {
    freelist_idx = N_LISTS - 1;
  }

  // find the first fi
  header *sentinel = &freelistSentinels[freelist_idx];
  header *block = sentinel->next;
  
  while (block != sentinel) {
    if (block->size_state >= actual_size)
      break;
    }
    block = block->next;
  }
  if (block == sentinel) {
    for (int i = freelist_idx + 1; i < N_LISTS; i++) {
      sentinel = &freelistSentinels[i];
      block = sentinel->next;
      while (block != sentinel && block->size_state < actual_size) {
        block = block->next;
      }
      if (block != sentinel) break;
    }
  }
  if (block == sentinel) {
    return NULL;
  }
  block->prev->next = block->next;
  block->next->prev = block->prev;
  size_t leftover_size = block->size_state - actual_size;
  if (leftover_size >= sizeof(header)) {
    header *free_block = block;
    free_block->size_state = leftover_size;
    block = (header *)((char *)free_block + leftover_size);
    block->size_state = actual_size;
    block->left_size = leftover_size;
    header *right_neighbor = (header *)((char *)block + block->size_state);
    if (!(right_neighbor->size_state & ALLOCATED)) {
      right_neighbor->left_size = block->size_state;
    }
    int free_idx = free_block->size_state / 8;
    if (free_idx >= N_LISTS) {
      free_idx = N_LISTS - 1;
    }
    header *free_sentinel = &freelistSentinels[free_idx];
    free_block->next = free_sentinel->next;
    free_block->prev = free_sentinel;
    free_sentinel->next->prev = free_block;
    free_sentinel->next = free_block;
  }
  block->size_state |= ALLOCATED;
  return (header *)(block->data);
}