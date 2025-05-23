#pragma once

#include "machine/apicsystem.h"

class Status {
    bool cpu_idle[CPU_MAX];
    unsigned int thread_counter;

    friend class StatusApplication;

public:
    Status() : cpu_idle(), thread_counter(0) {}

    void set_idle(bool state) {
        cpu_idle[system.getCPUID()] = state;
    }

    void thread_inc() {
        thread_counter++;
    }

    void thread_dec() {
        thread_counter--;
    }
};

extern Status status;
