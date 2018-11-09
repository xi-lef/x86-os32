#pragma once

#include "device/cgastr.h"

class Shell {
private:
    CGA_Stream stream;

public:
    Shell(CGA_Stream stream) : stream(stream) {}

    int main() {
        for (;;) {
            
        }
    }
}
