#include "user/string/string.h"
#include "debug/output.h"
#include "utils/math.h"
#include "utils/memutil.h"

String::String() : len(0), save_index(0) {}

String::String(char *s) : save_index(0) {
    int i;
    for (i = 0; s[i] != '\0'; i++) {
        data[i] = s[i];
    }   
    len = i;
}

String& String::operator =(const String& str) {
    strncpy(data, str.data, str.length());
    len = str.len;
    save_index = 0;
    return *this;
}

String::operator char*() {
    for (int i = len; i < STRING_MAX_LENGTH; i++) {
        data[i] = '\0';
    }
    return data;
}

char String::at(int i) const {
    if (i < 0 || (size_t) i >= len) {
        DBG << "String: at: invalid index" << endl;
    }
    return data[i];
}

char& String::operator [](int i) {
    if (i < 0 || (size_t) i >= len) {
        DBG << "String: []: invalid index" << endl;
    }
    return data[i];
}

const char& String::operator [](int i) const {
    if (i < 0 || (size_t) i >= len) {
        DBG << "String: []: invalid index" << endl;
    }
    return data[i];
}

bool String::operator ==(const String& str) const {
    return streq(*this, str);
}

size_t String::length() const {
    return len;
}

void String::clear() {
    len = 0;
    save_index = 0;
}

bool String::empty() const {
    return len == 0;
}

void String::resize(size_t n, char c) {
    if (n > len) {
        for (size_t i = len; i < n; i++) {
            data[i] = c;
        }
    }
    len = n;
}

String String::substr(size_t pos, size_t len) const {
    if (pos == 0 && len >= this->len) {
        return *this;
    } else if (pos > this->len) {
        return String();
    }

    String new_str;
    for (size_t i = 0; i < Math::min(len, this->len - pos); i++) {
        new_str += at(pos + i);
    }
    return new_str;
}

String& String::append(char c) {
    if (len == STRING_MAX_LENGTH) {
        DBG << "String: append: full" << endl;
        return *this;
    }
    data[len++] = c;
    return *this;
}

String& String::append(const String& str) {
    for (size_t i = 0; i < str.length(); i++) {
        append(str[i]);
    }
    return *this;
}

String& String::operator +=(char c) {
    append(c);
    return *this;
}

String& String::operator +=(const String& str) {
    append(str);
    return *this;
}

String& String::insert(size_t pos, char c) {
    String end = substr(pos);
    resize(pos);
    append(c);
    append(end);
    return *this;
}

String& String::insert(size_t pos, const String& str) {
    String end = substr(pos);
    resize(pos);
    append(str);
    append(end);
    return *this;
}

String& String::erase(size_t pos, size_t len) {
    String new_str = (pos == 0) ? String() : substr(0, pos); // 1st part
    new_str.append(substr(pos + len));                       // 2nd part
    *this = new_str;
    return *this;
}

bool String::push_back(char c) {
    return append(c);
}

bool String::pop_back() {
    if (len > 0) {
        resize(len - 1);
        return true;
    }
    return false;
}

String String::without_lf() const {
    String new_str(*this);
    if (new_str[len - 1] == '\n') {
        new_str[len - 1] =  '\0';
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

bool String::compare(size_t pos, const String& str) const {
    if (pos + str.length() > len) {
        return false;
    }

    for (size_t i = pos; i < len; i++) {
        if (data[i] != str[i - pos]) {
            return false;
        }
    }
    return true;
}

size_t String::find(const String& str, size_t pos) const {
    for (size_t i = pos; i < len; i++) {
        if (compare(i, str)) {
            return i;
        }
    }
    return npos;
}

size_t String::find_first_of(char c, size_t pos) const {
    for (size_t i = pos; i < len; i++) {
        if (data[i] == c) {
            return i;
        }
    }
    return npos;
}

size_t String::find_first_of(const String& str, size_t pos) const {
    for (size_t i = pos; i < len; i++) {
        for (size_t j = 0; j < str.length(); j++) {
            if (data[i] == str[j]) {
                return i;
            }
        }
    }
    return npos;
}

String String::tok(const String& delim) {
    return strtok(*this, delim);
}

size_t strlen(const char *s) {
    const char *p = s;
    while (*p != '\0') {
        p++;
    }
    return p - s;
}

char *strcpy(char *dest, const char *src) {
    return strncpy(dest, src, strlen(src));
}

char *strncpy(char *dest, const char *src, size_t size) {
    return (char *) memcpy(dest, src, size);
}

bool streq(const String& str1, const String& str2) {
    if (str1.length() != str2.length()) {
        return false;
    }
    for (size_t i = 0; i < str1.length(); i++) {
        if (str1[i] != str2[i]) {
            return false;
        }
    }
    return true;
}

String strtok(String& str, const String& delim) {
    size_t pos;
    // skip leading delimiting chars
    while ((pos = str.find_first_of(delim, str.save_index)) == str.save_index) {
        str.save_index++;
    }
    if (pos == String::npos) {
        // no delimiting char could be found
        pos = str.length();
    }

    String ret = str.substr(0, pos).substr(str.save_index);
    str.save_index = pos + 1;
    return ret;
}

long strtol(const String& str, bool *error, int base) {
    // initialize error for proper error handling
    if (error) {
        *error = false;
    }

    // skip leading whitespace
    size_t i;
    for (i = 0; i < str.length() && str[i] == ' '; i++);

    // check if the number is negative
    bool negative = false;
    if (i < str.length() && str[i] == '-') {
        negative = true;
        i++;
    }

    // check for prefixes for binary, octal, or hexadecimal.
    // check hexadecimal before octal, or else we would mistake 0x as octal!
    if ((base == 0 || base == 2) && (i + 1) < str.length()
        && str[i] == '0' && (str[i + 1] == 'b' || str[i + 1] == 'B')) {
        i += 2;
        base = 2;
    } else if ((base == 0 || base == 16) && (i + 1) < str.length()
        && str[i] == '0' && (str[i + 1] == 'x' || str[i + 1] == 'X')) {
        i += 2;
        base = 16;
    } else if ((base == 0 || base == 8) && i < str.length()
        && str[i] == '0') {
        i += 1;
        base = 8;
    } else if (base == 0) {
        base = 10;
    }

    // do the actual conversion
    unsigned long res = 0;
    for (; i < str.length(); i++) {
        char cur = str[i];
        unsigned long tmp = res;

        // take min(base, 10) or else we would accept "digits" like ';'
        if (cur >= '0' && cur <= ('0' + Math::min(base, 10) - 1)) {
            res = base * res + (cur - '0');
        } else if (base > 10 && ((cur >= 'a' && cur <= 'a' + base)
                                 || (cur >= 'A' && cur <= 'A' + base))) {
            bool small = cur >= 'a' && cur <= 'a' + base;
            res = base * res + (10 + cur - (small ? 'a' : 'A'));
        } else {
            if (error) {
                *error = true;
            }
            break;
        }

        if (res < tmp) {
            goto overflow;
        }
    }

    if (res > SSIZE_MAX) {
        goto overflow;
    }
    return negative ? (long) res * -1 : res;

overflow:
    if (error) {
        *error = true;
    }
    return SSIZE_MAX; // meh
}
