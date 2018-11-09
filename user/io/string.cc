#include "io/string.h"

String::String() : length(0) {}

String::String(char *s) {
    int i;
    for (i = 0; s[i] != '\0'; i++) {
        str[i] = s[i];
    }   
    length = i;
}

char String::at(int i) {
    return str[i];
}

bool String::append(char c) {
    if (length == STRING_MAX_LENGTH) {
        return false;
    }
    str[length++] = c;
    return true;
}
