#pragma once

#include "types.h"

#define STRING_MAX_LENGTH 80

class String {
private:
    char    data[STRING_MAX_LENGTH];
    size_t len;

public:
    String();
    String(char *s);

    operator char*();

    char at(int i);

    size_t length() const;

    String without_lf();
    void remove_lf();
    bool append(char c);

    void empty();
    bool is_empty();
};

bool streq(String s1, String s2);
