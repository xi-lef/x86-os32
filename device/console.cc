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

    write(r + '0', true);
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
    flush();
    print("\ec", 2);
}

void Console::setpos(int x, int y) {
    flush();
    print("\e[", 2);
    write_number(y);
    print(";", 1);
    write_number(x);
    print("H", 1);
}

bool Console::getpos(int& x, int& y) {
    flush();
    x = 0;
    y = 0;

    print("\e[6n", 4);
    if (read() != '\e' || read() != '[') {
        return false;
    }

    int r;
    while ((r = read()) != ';') {
        y = y * 10 + r - '0';
    }
    y--;

    while (read() != 'R') {
        x = x * 10 + r - '0';
    }
    x--;

    return true;
}

void Console::print(char* string, int length) {
    for (int i = 0; i < length; i++) {
		if (string[i] == '\n' && i > 0 && string[i - 1] != '\r' ) {
			write('\r');
        }
        write(string[i], true);
    }
}

bool Console::prologue() {
    int c = read(false);
    if (c == -1) {
        return false;
    }
    if (!buf.produce(c)) {
        DBG << "Console: bbuffer full :(" << endl;
    }
    return true;
}

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

    plugbox.assign(console_vector, this);
    ioapic.config(console_slot, console_vector, TRIGGER_MODE_LEVEL);
    receiveInterrupt(true);
}
