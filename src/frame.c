#include <frame.h>

#include <kmalloc.h>
#include <klog.h>
#include <maestro/string.h>

static u8 frames[NFRAMES] = {0};

void clear_frame(int idx)
{
    frames[idx] = 0;
}

int fff()
{
    for (int i = 0; i < NFRAMES; ++i)
    {
        if (frames[i] == 0)
            return i;
    }

    return -1;
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
    kprintf("There are %d frames\n", NFRAMES);
}