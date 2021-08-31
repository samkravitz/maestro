#include <kmalloc.h>

// start of heap begins at end of maestro image
extern u32 end;
u32 heap = (u32) &end;
void *block_head = NULL;

void *brk(int amt)
{
  u32 ptr = heap;
  heap += amt;
  return (void *) ptr;
}

/**
 * set a block of memory as free as in libre
 * @param ptr pointer to the memory block to mark as free
 */
void kfree(void *ptr)
{
  if(!ptr) 
  {
    // ignore null pointers :o
    return;
  }

  struct mem_block *block_ptr = get_block_ptr(ptr);
  
  // assign block as free
  // debug will be 0xBAADF00D if successfully freed
  block_ptr->free = 1;
  block_ptr->debug = 0xBAADF00D;
}

/**
 * kernel malloc
 * @param size malloc size in n bytes
 */ 
void *kmalloc(size_t size)
{
  struct mem_block *i_am;
  // todo: handle alignment with page
  // actually i thhink it works with kmalloca

  // if someone is playing games... return null
  if(size <= 0) 
  {
    return NULL;
  }


  if(!block_head) 
  {
    // make request, if denied return null
    i_am = request_memory(NULL, size);
    if(!i_am) 
    {
      return NULL;
    }
  } 
  else 
  {
    struct mem_block *was = block_head;
    i_am = find_next_free(&was, size);

    if(!i_am) {
      // request more mem if no free blocks were found
      i_am = request_memory(was, size);
      if(!i_am) {
        // if request from was was denied return null
        return NULL;
      }
    } else {
      // successfully found a free block
      i_am->free = 0;
      i_am->debug = 0xBAD1DEED;
    }
  }

  return (++i_am);

  // u32 ptr = (u32) heap;
  // heap += size;
  // return (void *) ptr;
}

/**
 * @param size size of malloc in n bytes
 * @param align boolean indicating whether or not the
 * address returned should be aligned to page boundary
 */
void *kmalloca(size_t size)
{
  if(heap & 0xFFFFF000)
  {
    heap &= 0xFFFFF000;

    // if the previous operation rounds down, we don't want to return any memory
    // that may have already been kmalloc'd. so, we'll add a page size to ensure
    // this memory is new.
    heap += _PAGE_SIZE;
  }

  return kmalloc(size);
}

// kmalloc phys - fills the phys pointer with the physical address of the returned memory
void *kmallocp(size_t nbytes, u32 *phys)
{
  u32 ptr = kmalloc(nbytes);
  if (phys)
    *phys = ptr;

  return (void *) ptr;
}

/**
 * resize a block of memory
 * @param ptr pointer to the block of memory
 * @param size new size to allocate to
 */
void *krealloc(void *ptr, size_t size) 
{
  if(!ptr) 
  {
    // if null pointer is passed, realloc functions as malloc
    return kmalloc(size);
  }

  struct mem_block *current = get_block_ptr(ptr);
  if(current->size > size) 
  {
    // decide for caller that there's already enough memory...
    // blocks could split here in the future, thus freeing some
    // of the memory that is unused in this block, but for now
    // that isn't necessary
    return ptr;
  }

  // initialize new block
  void *new_block_ptr = kmalloc(size);
  if(!new_block_ptr) 
  {
    // failled to malloc
    return NULL;
  }

  // copy memory from old block to new block, then 
  // free the old block
  memcpy(new_block_ptr, ptr, current->size);
  free(ptr);
  return new_block_ptr;
}

/**
 * clear an allocation n elements
 * @param n number of elements
 * @param size_el size of each element
 */
void *kcalloc(size_t len, size_t size_el) 
{
  size_t size = len * size_el;
  void *ptr;
  if(size > heap) 
  {
    // check for overflow
    return NULL;
  }

  // initialize allocation to zero
  ptr = kmalloc(size);
  memset(ptr, 0, size);
  return ptr;
}

/**
 * find the next available memory block, if one exists
 * @param was double pointer to previous memory block
 * @param size size that is currently trying to be malloc'd 
 */ 
struct mem_block *find_next_free(struct mem_block **was, size_t size) 
{
  struct mem_block *i_am = block_head;
  for(;;) {
    if(i_am && i_am->free && i_am->size >= size) 
    {
      break;
    } 

    *was = i_am;
    i_am = i_am->next;
  }
  return i_am;
}

/**
 * ask `brk` for more memory
 * @param was pointer to the previous memory block
 * @param size n bytes to request
 */
struct mem_block *request_memory(struct mem_block *was, size_t size) 
{
  struct mem_block *block;
  block = brk(0);
  void *requested_mem = brk(sizeof(block) + size);
  if(requested_mem == (void *) - 1) 
  {
    // failed to fetch more memory
    return NULL;
  }

  if(was) 
  {
    // was will always be null on the first request
    // because we're starting with the head of the list
    was->next = block;
  }
  
  // prepend this block to the head of the heap
  block->next = NULL;
  block->size = size;
  block->free = 0;
  block->debug = 0xBAD1F00D;
  return block;
}

/**
 * get pointer to the block of memory
 * just does pointer - 1 but is more semantic / easier
 * to read than pointer arithmetic and casting to struct
 * each time
 * @param ptr pointer to get block of memory for
 */
struct mem_block *get_block_ptr(void *ptr) 
{
  return ((struct mem_block*) ptr) - 1;
}
