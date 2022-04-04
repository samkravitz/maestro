/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: clk.c
 * DATE: August 2nd, 2021
 * DESCRIPTION: Controllers for the PIT
 */
#include <clk.h>

#include <intr.h>
#include <io.h>
#include <kprintf.h>
#include <proc.h>
#include <pq.h>
#include <queue.h>

// base frequency of the PIT, in Hz
#define PIT_BASE_RATE 1193180

static u64 sec = 0;    // seconds since maestro was bootstrapped
static int ms  = 0;    // ms since sec was last updated

extern struct queue *readyq;
extern struct pq *sleepq;
extern struct proc *curr;

/**
 * @brief total number of ms since maestro was bootstrapped
 */
static inline u32 timestamp()
{
	return sec * 1000 + ms;
}

static int sleep_cmp(void *p1, void *p2)
{
	struct proc *pptr1 = (struct proc *) p1;
	struct proc *pptr2 = (struct proc *) p2;
	return pptr1->wakeup < pptr2->wakeup;
}

static void clkhandler()
{
	// wake up sleeping process if necessary
	struct proc *pptr = (struct proc *) peek(&sleepq);
	if (pptr && pptr->wakeup <= timestamp())
	{
		pop(&sleepq);
		pptr->state = PR_READY;
		pptr->wakeup = 0;
		insert(readyq, pptr);
	}

	if (++ms == 1000)
	{
		++sec;
		ms = 0;
		sched();
	}
}

// init clk
void clk_init()
{
	set_vect(IRQ0, clkhandler);

	// we want our clock interrupt to trigger at a rate of 1000Hz,
	// or once every 1ms. Hence we divide by 1000.
	u32 divisor = PIT_BASE_RATE / 1000;

	// Send the command byte.
	outb(0x43, 0x36);

	// Send the frequency divisor.
	outb(0x40, divisor >> 0 & 0xFF);
	outb(0x40, divisor >> 8 & 0xFF);
}

/**
 * @brief puts the current process to sleep
 * @param msec number of milliseconds to sleep for
 */
void sleepms(uint msec)
{
	int mask = disable();
	u32 wakeup = timestamp() + msec;
	curr->wakeup = wakeup;
	push(&sleepq, curr, sleep_cmp);
	curr->state = PR_SLEEPING;
	sched();
	restore(mask);
}
