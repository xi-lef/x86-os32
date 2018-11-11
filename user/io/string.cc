#include "user/io/string.h"
#include "debug/output.h"

String::String() : length(0) {}

String::String(char *s) {
    int i;
    for (i = 0; s[i] != '\0'; i++) {
        data[i] = s[i];
    }   
    length = i;
}

String::operator char*() {
    for (int i = length; i < STRING_MAX_LENGTH; i++) {
        data[i] = '\0';
    }
    return data;
}

char String::at(int i) {
    return data[i];
}

uint8_t String::get_length() const {
    return length;
}

String String::without_nl() {
    String new_str(*this);
    if (new_str.data[length - 1] == '\n') {
        new_str.data[length - 1] =  '\0';
    }
    new_str.length--;
    return new_str;
}

void String::remove_nl() {
    if (data[length - 1] == '\n') {
        data[length - 1] =  '\0';
    }
    length--;
}

bool String::append(char c) {
    if (length == STRING_MAX_LENGTH) {
        return false;
    }
    data[length++] = c;
    return true;
}

void String::empty() {
    length = 0;
}

int strlen(String s) {
    return s.get_length();
}

bool streq(String s1, String s2) {
    if (s1.get_length() != s2.get_length()) {
        return false;
    }
    for (int i = 0; i < s1.get_length(); i++) {
        if (s1.at(i) != s2.at(i)) {
            return false;
        }
    }
    return true;
}

/*#include <stdio.h>

int main() {
    String s1("abcdef");
    String s2("abcdef");

    printf("%d\n", streq(s1, s2));

    return 0;
}//*/
