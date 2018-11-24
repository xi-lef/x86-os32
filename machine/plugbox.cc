// vim: set et ts=4 sw=4:

/* INCLUDES */

#include "machine/plugbox.h"
#include "device/panic.h"

Plugbox plugbox;
static Gate *vector_table[256];

Plugbox::Plugbox() {
    for (int i = 0; i < 256; i++) {
        vector_table[i] = &::panic;
    }
}

void Plugbox::assign(unsigned int vector, Gate *gate) {
    vector_table[vector] = gate;
}

Gate* Plugbox::report(unsigned int vector) {
    return vector_table[vector];
}
