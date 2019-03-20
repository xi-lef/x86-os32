// vim: set et ts=4 sw=4:

#include "user/status/sappl.h"
#include "user/status/status.h"
#include "device/cgastr.h"
#include "syscall/guarded_bell.h"
#include "utils/heap.h"

void StatusApplication::action() {
    for (;;) {
        dout_status.reset();
        dout_status << "idle CPUs: ";
        for (int i = 0; i < CPU_MAX; i++) {
            if (status.cpu_idle[i]) {
                dout_status << i;
            } else {
                dout_status << ' ';
            }
        }
        dout_status.flush();

        dout_status.setpos(dout_status.from_col + 20, dout_status.from_row);
        dout_status << "#threads: " << status.thread_counter << flush;

        HeapStats stats = get_heap_stats();
        dout_status.setpos(dout_status.to_col - 10, dout_status.from_row);
        int used = (100 * stats.used) / stats.total;
        dout_status << "RAM: " << ((used < 10) ? "  " : ((used < 100) ? " " : "")) // whitespace padding
                    << used << '%' << flush;

        Guarded_Bell::sleep(100); // 10 fps
    }
}

void StatusApplication::setID(int i) {
    id = i;
}
