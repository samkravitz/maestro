#ifndef FRAME_H
#define FRAME_H

#include <maestro.h>

// maximum address on x86
#define MAX_ADDR    0xffffffff

#define NFRAMES     (MAX_ADDR / 4096)

void frame_init();
void clear_frame(int);
void set_frame(int);
int test_frame(int);

#endif // FRAME_H