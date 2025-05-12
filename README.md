# Custome Memory Allocator
Sure! Creating your own memory allocator is a great project for understanding how dynamic memory management works at a low level. Below is a basic guide on how you might approach building your own allocator, modeled after `malloc`, `calloc`, `realloc`, and `free`. This guide is intended to provide a high-level roadmap of the project, and you will need to implement the functions yourself, as per your project requirements.

### **Overview of What You're Building**
1. **`malloc(size_t size)`**: Allocates a block of memory of the specified size. Returns a pointer to the beginning of the block.
2. **`calloc(size_t num, size_t size)`**: Allocates memory for an array of `num` elements, each of size `size`. It initializes the allocated memory to zero.
3. **`realloc(void *ptr, size_t new_size)`**: Resizes the memory block pointed to by `ptr` to `new_size`. If `new_size` is smaller, it truncates the memory block. If larger, it allocates new memory and copies the old data over.
4. **`free(void *ptr)`**: Frees the previously allocated memory block.

---

### **Key Concepts to Understand**
Before you start implementing, you need a strong understanding of:
1. **Memory management**: You will have to manage the heap and know how to divide and combine memory blocks manually.
2. **Pointers**: You will manipulate pointers to allocate, resize, and free blocks of memory.
3. **Metadata**: You’ll need to store additional information about the allocated blocks, such as size and allocation status, which allows your allocator to know where each block begins and ends.
4. **System calls**: At the lower level, functions like `sbrk()` (for older systems) or `mmap()` (modern systems) are used to extend the heap by requesting more memory from the operating system.

---

### **Step-by-Step Guide**

#### 1. **Managing Memory (Heap)**
- **Memory Pool**: 
  You will start with a memory pool (like the heap) from which all allocations are made. You can use system calls like `sbrk()` or `mmap()` to expand the heap when you run out of space. Think of this pool as a large chunk of memory that you will carve up into smaller pieces as needed.
  
- **Metadata for Blocks**:
  Each block of memory you allocate needs to have metadata to track its size and whether it is in use or free. This metadata will typically reside just before the actual data block. For instance, you might need to store the size of the block and possibly a flag that indicates if the block is currently free.

#### 2. **Implementing `malloc`**
- **Basic Idea**: The `malloc` function should search the free blocks of memory and return a block of memory that fits the requested size.
- **Search Strategy**: You will likely need a search strategy for finding free memory. The simplest is a linear search through free blocks, but you can implement more efficient strategies like the best-fit, first-fit, or worst-fit approaches.
- **Splitting Blocks**: If you find a free block that is larger than what’s requested, you may want to split it into two parts: one that is the requested size and the other that remains as free memory.
  
#### 3. **Implementing `calloc`**
- **Basic Idea**: `calloc` behaves like `malloc`, but it also initializes the memory it allocates to zero.
- **Implementation**: After calling `malloc` to get a memory block, you will need to zero out the block manually. A simple loop to set each byte to `0` can accomplish this.

#### 4. **Implementing `realloc`**
- **Basic Idea**: `realloc` is a more complex function. It needs to handle three cases:
  - If the new size is smaller, you may simply shrink the block and possibly merge the adjacent free memory.
  - If the new size is larger, you may need to allocate a new block of memory, copy the data from the old block to the new one, and then free the old block.
  - If the pointer is `NULL`, you can treat it as a call to `malloc`.
  
  Handling the resizing is key here. You will need to check if there’s enough adjacent free space to grow the block in place.

#### 5. **Implementing `free`**
- **Basic Idea**: `free` should mark the memory block as available (free), so it can be reused for future allocations. You must also make sure to merge adjacent free blocks to avoid fragmentation.
- **Merging Blocks**: After freeing a block, you should try to coalesce (merge) it with neighboring free blocks. This helps avoid fragmentation in the heap.

#### 6. **Handling Fragmentation**
Memory fragmentation is one of the biggest challenges with custom memory allocators. You need to:
- **Avoid Internal Fragmentation**: Minimize unused space within allocated blocks.
- **Avoid External Fragmentation**: Keep the heap organized by merging adjacent free blocks.

#### 7. **Error Handling and Edge Cases**
- **Null pointers**: Ensure you correctly handle `NULL` pointers in `realloc` and `free`.
- **Invalid free**: Freeing a pointer that hasn’t been allocated or has already been freed should be avoided (you might want to implement some error checking).
- **Alignment**: Ensure that the allocated blocks respect the correct alignment for the platform (typically 8 or 16 bytes).

---

### **Tools & Techniques**
- **System Calls**: As mentioned earlier, `sbrk()` (deprecated on modern systems) or `mmap()` are useful for managing the underlying memory. These system calls allow you to increase the size of the heap.
  
- **Block Structure**: Your memory blocks will likely look something like this:
  ```
  +------------+---------+--------+------+
  | Block Size | Status  | Data   | ...  |
  +------------+---------+--------+------+
  ```
  Where:
  - Block size: The size of the entire block, including metadata.
  - Status: A flag indicating whether the block is in use or free.
  - Data: The actual memory that the user requested.
  
- **Debugging**: Debugging a custom memory allocator can be tricky. Use tools like `valgrind` or a custom memory checker to detect memory leaks and corruption.

---

### **Testing and Evaluation**
1. **Memory Leaks**: Ensure that all memory allocated is freed properly.
2. **Fragmentation**: Test how your allocator handles memory fragmentation.
3. **Edge Cases**: Test small allocations, large allocations, reallocations, and freeing memory multiple times.
4. **Performance**: Measure the performance (e.g., speed and memory overhead) of your allocator with various workloads.

---

### **Grading Considerations**
- **Correctness**: Your allocator should function like the standard `malloc`, `calloc`, `realloc`, and `free` in terms of behavior.
- **Efficiency**: Consider how efficiently your allocator handles searching for free blocks, splitting, coalescing, and resizing.
- **Edge Case Handling**: Ensure that edge cases like `NULL` pointers, invalid frees, and memory overflows are handled properly.
- **Memory Management**: Your allocator should minimize fragmentation and avoid memory leaks.
  
Good luck with your project!
