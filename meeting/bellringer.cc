// vim: set et ts=4 sw=4:

#include "meeting/bellringer.h"
#include "debug/output.h"

Bellringer bellringer;

void Bellringer::check() {
    //DBG << "BR: check " << flush;
    if (!bell_pending()) {
        //DBG << "BR: no pending " << flush;
        return;
    }
    //DBG << "BR: pending " << flush;

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
    //DBG << "BR: job, ms " << ms << " " << flush;
    Bell *prev = nullptr;
    for (auto const & b : bell_list) {
        if (b->ms <= ms) { // before bell
            ms -= b->ms;
            prev = b;
        } else {           // after bell
            b->ms -= ms;
        }
    }

    if (prev == nullptr) {
        bell_list.insert_first(bell);
    } else {
        bell_list.insert_after(prev, bell);
    }
}

void Bellringer::cancel(Bell *bell) {
    bell_list.remove(bell);
}

bool Bellringer::bell_pending() {
    return bell_list.first() != nullptr;
}
