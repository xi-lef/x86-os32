#include "user/io/string.h"
#include "debug/output.h"

String::String() : len(0) {}

String::String(char *s) {
    int i;
    for (i = 0; s[i] != '\0'; i++) {
        data[i] = s[i];
    }   
    len = i;
}

String::operator char*() {
    for (int i = len; i < STRING_MAX_LENGTH; i++) {
        data[i] = '\0';
    }
    return data;
}

char String::at(int i) {
    return data[i];
}

size_t String::length() const {
    return len;
}

String String::without_lf() {
    String new_str(*this);
    if (new_str.data[len - 1] == '\n') {
        new_str.data[len - 1] =  '\0';
        new_str.len--;
    }
    return new_str;
}

void String::remove_lf() {
    if (data[len - 1] == '\n') {
        data[len - 1] =  '\0';
        len--;
    }
}

bool String::append(char c) {
    if (len == STRING_MAX_LENGTH) {
        return false;
    }
    data[len++] = c;
    return true;
}

void String::empty() {
    len = 0;
}

bool String::is_empty() {
    return len == 0;
}

bool streq(String s1, String s2) {
    if (s1.length() != s2.length()) {
        return false;
    }
    for (size_t i = 0; i < s1.length(); i++) {
        if (s1.at(i) != s2.at(i)) {
            return false;
        }
    }
    return true;
}
