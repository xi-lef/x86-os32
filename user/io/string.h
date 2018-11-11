#pragma once

#include "types.h"

#define STRING_MAX_LENGTH 80

class String {
private:
    char    data[STRING_MAX_LENGTH];
    uint8_t length;

public:
    String();

    String(char *s);

    operator char*();

    char at(int i);

    uint8_t get_length() const;

    String without_nl();
    void remove_nl();

    bool append(char c);

    void empty();
};

int strlen(String s);

bool streq(String s1, String s2);
