// vim: set et ts=4 sw=4:

#include "console.h"
#include "debug/output.h"

Console::Console(Serial::comPort port, Serial::baudRate baudrate, Serial::dataBits databits, Serial::stopBits stopbits, Serial::parity parity) :
    Console::Serial(port, baudrate, databits, stopbits, parity) {
    DBG << "start" << endl;
    setpos(0, 0);
    setForeground(WHITE);
    setBackground(BLACK);
    setAttribute(RESET);
    DBG << "done" << endl;
}

void Console::write_number(int x) {
    char r = x % 10;
    if (x / 10 != 0) {
        write_number(x / 10);
    }

    write(r + 48, true);
}

void Console::flush() {
    print(buffer, pos);
    pos = 0;
}

void Console::setForeground(Console::color c) {
    print("\e[3", 3);
    write_number(c);
    print("m", 1);
}

void Console::setBackground(Console::color c) {
    print("\e[4", 3);
    write_number(c);
    print("m", 1);
}

void Console::setAttribute(Console::attrib a) {
    print("\e[", 2);
    write_number(a);
    print("m", 1);
}

void Console::reset() {
    print("\ec", 2);
}

void Console::setpos(int x, int y) {
    print("\e[", 2);
    write_number(y);
    print(";", 1);
    write_number(x);
    print("H", 1);
}

bool Console::getpos(int& x, int& y) {
    int tmp_x = 0;
    int tmp_y = 0;

    print("\e[6n", 4);
    if (read(true) != '\e' || read(true) != '[') {
        return false;
    }

    while (read(true) != ';') {
        tmp_y *= 10 + int(read(true) - 48);
    }
    while (read(true) != 'R') {
        tmp_x *= 10 + int(read(true) - 48);
    }
    x = tmp_x;
    y = tmp_y;

    return true;
}

void Console::print(char* string, int length) {
    for (int i = 0; i < length; i++) {
        if (string[i] == '\n') {
            write('\r', true);
        }
        write(string[i], true);
    }
}
