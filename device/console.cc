// vim: set et ts=4 sw=4:

#include "console.h"
#include "debug/output.h"
#include "machine/plugbox.h"
#include "machine/ioapic.h"
#include "object/bbuffer.h"

Console console;
static BBuffer<char, 64> buf;

Console::Console(Serial::comPort port, Serial::baudRate baudrate, Serial::dataBits databits, Serial::stopBits stopbits, Serial::parity parity) :
    Console::Serial(port, baudrate, databits, stopbits, parity) {
    setpos(0, 0);
    setForeground(WHITE);
    setBackground(BLACK);
    setAttribute(RESET);
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
        //DBG << int(string[i]) << ::flush;
        if (string[i] == '\r') { // harald lied, too :(
            write('\n', true);
        }
        write(string[i], true);
    }
}

bool Console::prologue() {
    int c = read(false);
    if (c == -1) {
        return false;
    }
    //DBG << char(c) << endl;
    if (!buf.produce(c)) {
        DBG << "console: bbuffer full :( " << ::flush;
    }
    return true;
}

extern CGA_Stream kout;

void Console::epilogue() {
    char c;

    while (buf.consume(c)) {
        // on real hardware, a '\r\n' is created, so the '\r' needs to be ignored.
        // in kvm, only a '\r' is created, so it needs to be replaced by a '\n'.
        if (c == '\r') {
            kout << '\n';
        } else {
            kout << c;
        }
    }
    kout << ::flush;
}

void Console::listen() {
    Plugbox::Vector console_vector = Plugbox::Vector::serial;
    APICSystem::Device dev;
    switch (port) {
        default: // fall through to COM1
        case COM1: dev = APICSystem::Device::com1; break;
        case COM2: dev = APICSystem::Device::com2; break;
        case COM3: dev = APICSystem::Device::com3; break;
        case COM4: dev = APICSystem::Device::com4; break;
    }
    unsigned char console_slot = system.getIOAPICSlot(dev);

    plugbox.assign(console_vector, &console);
    ioapic.config(console_slot, console_vector, TRIGGER_MODE_LEVEL); // TODO POLARITY_LOW?
    receiveInterrupt(true);
}
