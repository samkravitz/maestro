#include <kmalloc.h>

// start of heap begins at end of maestro image
extern u32 end;
u32 heap = (u32) &end;

void *kmalloc(size_t nbytes)
{
    void *ptr = (void *) heap;
    heap += nbytes;
    return ptr;
}

void kfree(void *ptr)
{

}