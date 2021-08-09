#ifndef KMALLOC_H
#define KMALLOC_H

#include <maestro.h>

// page size
#define PGSZ    4096

void *brk(int);
void  kfree(void *);
void *kmalloc(size_t);
void *kmalloca(size_t);
void *kmallocp(size_t, u32 *);
void *kmallocap(size_t, int, u32 *);

#endif // KMALLOC