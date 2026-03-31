#include "types.h"
#include "riscv.h"
#include "proc.h"
#include "scheduler.h"

// swtch.S is alreadly compiled in kernel
void swtch(struct context *old, struct context *new);

void
scheduler(void)
{
    struct proc *p;
    struct cpu *c = &cpu; // hawx uses a global cpu struct instead

    c->proc = 0;

    for(;;) {
        // this enables interruptsso the timer can wake the cpu
        intr_on();

        int found = 0;

        for (p = proc; p < &proc[NPROC]; p++) {
            if (p->state == RUNNABLE) {
                // preparing the process
                p->state = RUNNING;
                c->proc = p;

                // context switch like in xv6
                // so saving the scheduler regs, loading p->context
                swtch(&c->context, &p->context);

                // process returned 
                c->proc = 0;
                found = 1;
            }
        }

        if (found == 0) {
            // waiting for interrupt (nothing to run)
            asm volatile("wfi");
        }
    }
}