// vim: set et ts=4 sw=4:

#include "machine/toc.h"

// TOC_SETTLE: bereitet den Kontext der Koroutine fuer den ersten Aufruf vor.
void toc_settle (struct toc *regs, void *tos, void (*kickoff)(Thread*), Thread *object)
{
}

