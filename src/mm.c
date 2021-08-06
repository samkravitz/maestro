#include <mm.h>

#include <frame.h>
#include <kmalloc.h>
#include <maestro/string.h>

struct pagedir *kpd;    // kernel page directory
struct pagedir *cpd = 0;    // current page directory 

void mminit()
{
    frame_init();

    kpd = (struct pagedir *) kmalloca(sizeof(struct pagedir));
    memset(kpd, 0, sizeof(struct pagedir));
}