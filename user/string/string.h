#pragma once

#include "types.h"

class String {
private:
    // Pseudo-struct for use in sizeof.
    struct long_data {
        char *data;
        size_t capacity;
    };
    static const size_t long_data_size = sizeof(long_data);

    union {
        struct {
            char   *data;
            size_t capacity;
        };
        mutable char short_data[long_data_size];
    };
    bool use_heap;
    size_t len;

    size_t save_index; // For strtok.
    friend String strtok(String& str, const String& delim);

    // (At least) doubles the capacity of the "data" array.
    // "cap" is the minimum for the newly required capacity.
    void __maybe_resize(size_t cap);

public:
    static const size_t npos = (size_t) -1;

    /// Constructors etc.
    String();
    String(char *s);
    String(const String& str);

    ~String();

    String& operator =(const String& str);

    operator const char*();

    /// Element access.
    // Use int instead of size_t because of g++.
    char& at(int i);
    const char& at(int i) const;
    char& operator [](int i);
    const char& operator [](int i) const;

    bool operator ==(const String& str) const;

    /// Size manipulation.
    size_t length() const;
    void clear();
    bool empty() const;
    void resize(size_t n, char c = ' ');

    void shrink_to_fit();

    String substr(size_t pos = 0, size_t len = npos) const;

    /// String manipulation.
    String& append(char c);
    String& append(const String& str);
    String& operator +=(char c);
    String& operator +=(const String& str);

    String& insert(size_t pos, char c);
    String& insert(size_t pos, const String& str);

    String& erase(size_t pos = 0, size_t len = npos);

    void push_back(char c);
    void pop_back();

    String without_lf() const;
    void remove_lf();

    /// String operations.
    // Compare this string, starting at "pos", with "str".
    // "pos" is checked for validity.
    bool compare(size_t pos, const String& str) const;

    size_t find(const String& str, size_t pos = 0) const;
    size_t find_first_of(char c, size_t pos = 0) const;
    size_t find_first_of(const String& str, size_t pos = 0) const;
    String tok(const String& delim);

    // Copy the string into "s".
    size_t copy(char *s, size_t len = npos, size_t pos = 0) const;

    class Iterator {
    private:
        String *str;
        size_t pos;

    public:
        Iterator() : str(nullptr), pos(0) {}
        Iterator(String *str) : str(str), pos(0) {}

        char& operator *() const {
            return str->at(pos);
        }

        // Prefix
        Iterator& operator ++() {
            if (!str) {
                return *this;
            }

            if (++pos == str->length()) {
                str = nullptr;
                pos = 0;
            }
            return *this;
        }

        // Postfix
        Iterator operator ++(int) {
            Iterator ans(*this);
            ++(*this);
            return ans;
        }

        bool operator ==(const Iterator& it) const {
            return str == it.str && pos == it.pos;
        }

        bool operator !=(const Iterator& it) const {
            return !(*this == it);
        }
    };

    Iterator begin() {
        return Iterator(this);
    }

    Iterator end() {
        return Iterator();
    }
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
