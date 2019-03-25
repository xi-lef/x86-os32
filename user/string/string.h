#pragma once

#include "types.h"

#define STRING_MAX_LENGTH 256

class String {
private:
    char   data[STRING_MAX_LENGTH];
    size_t len;
    size_t MAX_LENGTH = STRING_MAX_LENGTH;

public:
    String();
    String(char *s);

    operator char*();

    char at(size_t i) const;
    char& operator [](size_t i);
    const char& operator [](size_t i) const;

    // pos is 0-indicated and inclusive
    String from(size_t pos) const;
    String to(size_t pos) const;
    void resize(size_t n, char c = ' ');

    size_t length() const;

    String without_lf() const;
    void remove_lf();

    bool append(char c);
    bool append(const String& s);
    String& operator +=(char c);
    String& operator +=(const String& s);

    bool push_back(char c);
    bool pop_back();

    void empty();
    bool is_empty() const;
};

bool streq(String s1, String s2);

// base 0 means 'figure it out yourself'.
// returns SSIZE_MAX on overflow
long strtol(const String& s, bool *error = nullptr, int base = 0);
