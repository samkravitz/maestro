#include <fs.h>

#include <ext2.h>

// mounts the filesystem
void mount()
{
    ext2_init();
}

FILE *open(const char *filenamd){ return NULL; }
size_t read(int fd, void *buff, size_t n){ return 0; }
size_t write(int fd, const void *buff, size_t n){ return 0; }