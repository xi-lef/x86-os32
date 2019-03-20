// vim: set et ts=4 sw=4:

#include "meeting/bellringer.h"
#include "debug/output.h"

Bellringer bellringer;

void Bellringer::check() {
    if (!bell_pending()) {
        return;
    }

    Bell *first = bell_list.first();
    if (--first->ms == 0) {
        do {
            first->ring();
            cancel(first);
        // also ring other bells that waited for the same time as the first one
        } while (bell_pending() && (first = bell_list.first())->ms == 0);
    }
}

void Bellringer::job(Bell *bell, unsigned int ms) {
    Bell *prev = nullptr;
    for (Bell *b : bell_list) {
        if (b->ms > ms) { // if insert position was found: decrease the next one
            b->ms -= ms;
            break;
        }
        // otherwise: just keep iterating and decreasing
        ms -= b->ms;
        prev = b;
    }
    bell->ms = ms;

    if (prev == nullptr) {
        bell_list.insert_first(bell);
    } else {
        bell_list.insert_after(prev, bell);
    }
}

void Bellringer::cancel(Bell *bell) {
    Bell *next = bell_list.next(bell);
    if (next) {
        next->ms += bell->ms;
    }
    bell_list.remove(bell);
}

bool Bellringer::bell_pending() {
    return bell_list.first() != nullptr;
}
