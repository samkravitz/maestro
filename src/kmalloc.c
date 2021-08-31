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

void kfree(void *ptr)
{
  if(!ptr) {
    // ignore null pointers :o
    return;
  } else {
    struct mem_block *block_ptr = get_block_ptr(ptr);
    
    // assign block as free
    // debug will be 0xB19B00B5 if successfully freed
    block_ptr->free = 1;
    block_ptr->debug = 0xB19B00B5;
  }
}

/**
 * @param size malloc size in n bytes
 */ 
void *kmalloc(size_t size)
{
  struct mem_block *i_am;
  // todo: handle alignment with page
  // actually i thhink it works with kmalloca

  // if someone is playing games... return null
  if(size <= 0) {
    return NULL;
  }


  if(!block_head) {
    // make request, if denied return null
    i_am = request_memory(NULL, size);
    if(!i_am) {
      return NULL;
    }
  } else {
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
      i_am->debug = 0xBADB100D;
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

// kmalloc align + kmalloc phys
void *kmallocap(size_t nbytes, int align, u32 *phys)
{
  if (heap & 0xFFFFF000)
  {
    //ptr &= ~PGSZ;
    heap &= 0xFFFFF000;

    // if the previous operation rounds down, we don't want to return any memory
    // that may have already been kmalloc'd. so, we'll add a page size to ensure
    // this memory is new.
    heap += _PAGE_SIZE;
  }

  u32 ptr = kmalloc(nbytes);
  if (phys)
    *phys = ptr;

  return (void *) ptr;
}

struct mem_block *find_next_free(struct mem_block **was, size_t size) {
  struct mem_block *i_am = block_head;
  for(;;) {
    if(i_am && i_am->free && i_am->size >= size) {
      break;
    } else {
      *was = i_am;
      i_am = i_am->next;
    }
  }
  return i_am;
}

struct mem_block *request_memory(struct mem_block *was, size_t size) {
  struct mem_block *block;
  block = brk(0);
  void *requested_mem = brk(size + _PAGE_SIZE);
  if(requested_mem == (void *) - 1) {
    // failed to fetch more memory
    return NULL;
  }

  if(was) {
    // was will always be null on the first request
    // because we're starting with the head of the list
    was->next = block;
  }
  
  // prepend this block to the head of the heap
  block->next = NULL;
  block->size = size;
  block->free = 0;
  block->debug = 0xBAADF00D;
  return block;
}

struct mem_block *get_block_ptr(void *ptr) {
  return (struct mem_block*) --ptr;
}
