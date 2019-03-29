#pragma once

#include "types.h"

#define STRING_MAX_LENGTH 256

class String {
private:
    char   data[STRING_MAX_LENGTH];
    size_t len;

    size_t save_index; // for strtok
    friend String strtok(String& str, const String& delim);

public:
    static const size_t MAX_LENGTH = STRING_MAX_LENGTH;

    String();
    String(char *s);

    operator char*();

    String& operator =(const String& str);

    char at(size_t i) const;
    char& operator [](size_t i);
    const char& operator [](size_t i) const;

    bool operator ==(const String& str) const;

    // pos is 0-indicated and inclusive
    String from(size_t pos) const;
    String to(size_t pos) const;
    void resize(size_t n, char c = ' ');

    size_t length() const;

    String without_lf() const;
    void remove_lf();

    bool append(char c);
    bool append(const String& str);
    String& operator +=(char c);
    String& operator +=(const String& str);

    String& insert(size_t pos, char c);
    String& insert(size_t pos, const String& str);

    bool push_back(char c);
    bool pop_back();

    bool compare(size_t pos, const String& str) const;
    size_t find_first_of(char c, size_t pos = 0) const;
    size_t find_first_of(const String& str, size_t pos = 0) const;
    String tok(const String& delim);

    void empty();
    bool is_empty() const;
};

size_t strlen(const char *s);

char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t size);

bool streq(const String& str1, const String& str2);

// returns an empty string at the end
String strtok(String& str, const String& delim);

// base 0 means "figure it out yourself" (only 2, 8, 10, 16).
// supported bases are [2, 36].
// returns SSIZE_MAX on overflow
long strtol(const String& str, bool *error = nullptr, int base = 0);
