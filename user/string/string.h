#pragma once

#include "types.h"

#define STRING_MAX_LENGTH 256

class String {
private:
    char   data[STRING_MAX_LENGTH];
    size_t len;

    size_t save_index; // For strtok.
    friend String strtok(String& str, const String& delim);

public:
    static const size_t MAX_LENGTH = STRING_MAX_LENGTH;
    static const size_t npos = (size_t) -1;

    /// Constructors etc.
    String();
    String(char *s);

    String& operator =(const String& str);

    operator char*();

    /// Element access.
    // Use int instead of size_t because of g++.
    char at(int i) const;
    char& operator [](int i);
    const char& operator [](int i) const;

    bool operator ==(const String& str) const;

    /// Size manipulation.
    size_t length() const;
    void clear();
    bool empty() const;
    void resize(size_t n, char c = ' ');

    String substr(size_t pos = 0, size_t len = npos) const;

    /// String manipulation.
    String& append(char c);
    String& append(const String& str);
    String& operator +=(char c);
    String& operator +=(const String& str);

    String& insert(size_t pos, char c);
    String& insert(size_t pos, const String& str);

    String& erase(size_t pos = 0, size_t len = npos);

    bool push_back(char c);
    bool pop_back();

    String without_lf() const;
    void remove_lf();

    /// String operations.
    bool compare(size_t pos, const String& str) const;

    size_t find(const String& str, size_t pos = 0) const;
    size_t find_first_of(char c, size_t pos = 0) const;
    size_t find_first_of(const String& str, size_t pos = 0) const;
    String tok(const String& delim);
};

size_t strlen(const char *s);

char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t size);

bool streq(const String& str1, const String& str2);

// Returns an empty String at the end.
String strtok(String& str, const String& delim);

// Base 0 means "figure it out yourself" (only 2, 8, 10, 16).
// Supported bases are [2, 36].
// Returns SSIZE_MAX on overflow.
long strtol(const String& str, bool *error = nullptr, int base = 0);
