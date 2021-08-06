#ifndef FRAME_H
#define FRAME_H

#include <maestro.h>

// 256 M (for now)
#define MAX_ADDR    0x10000000

#define NFRAMES     (MAX_ADDR / 4096)

void frame_init();
void clear_frame(int);
int fff();  // first free frame
void set_frame(int);
int test_frame(int);

#endif // FRAME_H