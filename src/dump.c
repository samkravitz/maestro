#include <dump.h>

#include <kout.h>

void hexdump(void *data, int num) { }

void stackdump(u32 *stk, int num)
{
	koutf("dumping %d entries of stack\n", num);
	u32 *tmp = stk + num - 1;
	u32 d;
	while (num--)
	{
		d = *tmp;
		koutf("0x%x: 0x%x\n", tmp, d);
		tmp--;
	}
}