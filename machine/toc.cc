// vim: set et ts=4 sw=4:

#include "machine/toc.h"

void toc_settle(struct toc *regs, void *tos, void (*kickoff)(Thread*), Thread *object) {
    void **esp = (void **) tos; // TODO -1 here or in caller?

    regs->ebp = esp;
    *esp      = (void *) object;
    *(--esp)  = (void *) 0xbaadf00d;
    *(--esp)  = (void *) kickoff;
    regs->esp = esp;
}
