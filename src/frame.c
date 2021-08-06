#include <frame.h>

#include <kmalloc.h>
#include <maestro/string.h>

static u8 frames[NFRAMES] = {0};

void clear_frame(int idx)
{
    frames[idx] = 0;
}

void set_frame(int idx)
{
    frames[idx] = 1;
}

int test_frame(int idx)
{
    return frames[idx];
}

void frame_init()
{
    memset(frames, 0, sizeof(frames));
}