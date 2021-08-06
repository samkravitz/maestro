#ifndef KMALLOC_H
#define KMALLOC_H

#include <maestro.h>

void *kmalloc(size_t);
void  kfree(void *);

#endif // KMALLOC