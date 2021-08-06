#include <kmalloc.h>

// start of heap begins at end of maestro image
extern u32 end;
u32 heap = (u32) &end;

void kfree(void *ptr)
{

}

// general kmalloc
void *kmalloc(size_t nbytes)
{
    return kmallocap(nbytes, 0, NULL);
}

// kmalloc align - makes sure address returned is aligned on a page boundary
void *kmalloca(size_t nbytes)
{
    return kmallocap(nbytes, 1, NULL);
}

// kmalloc phys - fills the phys pointer with the physical address of the returned memory
void *kmallocp(size_t nbytes, u32 *phys)
{
    return kmallocap(nbytes, 0, phys);
}

// kmalloc align + kmalloc phys
void *kmallocap(size_t nbytes, int align, u32 *phys)
{
    u32 ptr = (u32) heap;

    if (align && (heap & 0xFFFFF000))
    {
        //ptr &= ~PGSZ;
        ptr &= 0xFFFFF000;

        // if the previous operation rounds down, we don't want to return any memory
        // that may have already been kmalloc'd. so, we'll add a page size to ensure
        // this memory is new.
        ptr += PGSZ;
    }

    // store physical address of memory in pointer if requested
    if (phys)
        *phys = ptr;

    heap += nbytes;
    return (void *) ptr;
}