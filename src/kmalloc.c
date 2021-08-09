#include <kmalloc.h>

// start of heap begins at end of maestro image
extern u32 end;
u32 heap = (u32) &end;

void *brk(int amt)
{
    u32 ptr = heap;
    heap += amt;
    return (void *) ptr;
}

void kfree(void *ptr)
{

}

// general kmalloc
void *kmalloc(size_t nbytes)
{
    u32 ptr = (u32) heap;
    heap += nbytes;
    return (void *) ptr;
}

// kmalloc align - makes sure address returned is aligned on a page boundary
void *kmalloca(size_t nbytes)
{
    if (heap & 0xFFFFF000)
    {
        //ptr &= ~PGSZ;
        heap &= 0xFFFFF000;

        // if the previous operation rounds down, we don't want to return any memory
        // that may have already been kmalloc'd. so, we'll add a page size to ensure
        // this memory is new.
        heap += PGSZ;
    }

    return kmalloc(nbytes);
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
        heap += PGSZ;
    }

    u32 ptr = kmalloc(nbytes);
    if (phys)
        *phys = ptr;

    return (void *) ptr;
}