#pragma once

#include "types.h"

#define STRING_MAX_LENGTH 80

class String {
    private:
        char str[STRING_MAX_LENGTH];
        uint8_t length;

    public:
        String();

        String(char *s);

        char at(int i);

        bool append(char c);
};
