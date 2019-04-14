#include "serial.h"
#include "debug/output.h"
#include "machine/ioapic.h"

Serial::Serial(Serial::comPort port, Serial::baudRate baudrate, Serial::dataBits databits, Serial::stopBits stopbits, Serial::parity parity) :
    port(port) {
    /// baudrate
    // set DLAB bit
    writeReg(lcr, 1 << 7);
    // write lowbyte to transmitting buffer (DLAB low)
    writeReg(tbr, baudrate & 0xff);
    // write highbyte to ier (DLAB high)
    writeReg(ier, (baudrate >> 8) & 0xff);
    // unset DLAB bit
    writeReg(lcr, 0 << 7);

    /// databits + stopbits + parity
    writeReg(lcr, databits | stopbits | parity);

    // disable interrupts for normal reading
    writeReg(ier, 0x00);
    //DBG << "serial: finished init" << endl;
}

void Serial::writeReg(Serial::regIndex reg, uint8_t out) {
    IO_Port p(port + reg);
    p.outb(out);
}

char Serial::readReg(Serial::regIndex reg) {
    IO_Port p(port + reg);
	return p.inb();
}

int Serial::write(char out, bool blocking) {
    if (!blocking) {
        if (readReg(lsr) & (1 << 5)) {
            writeReg(tbr, out);
            return out;
        } else {
            DBG << "Serial: bad write" << endl;
            return -1;
        }
    } // else
    while (!(readReg(lsr) & (1 << 5))) ;

    writeReg(tbr, out);
	return out;
}

int Serial::read(bool blocking) {
    if (!blocking) {
        if (readReg(lsr) & 1) {
            return readReg(rbr);
        } else {
            return -1;
        }
    } // else
    while (!(readReg(lsr) & 1)) ;

    return readReg(rbr);
}

bool Serial::receiveInterrupt(bool enable) {
    // enable interrupt when data is available
    bool old = readReg(ier) & 0x01;
    writeReg(ier, enable ? 0x01 : 0x00);
    // weird stuff, idk
    writeReg(mcr, 0x0b);

    // configure IOAPIC
    APICSystem::Device dev;
    switch (port) {
        default: // fall through to COM1
        case COM1: dev = APICSystem::Device::com1; break;
        case COM2: dev = APICSystem::Device::com2; break;
        case COM3: dev = APICSystem::Device::com3; break;
        case COM4: dev = APICSystem::Device::com4; break;
    }
    if (enable) {
        ioapic.allow(system.getIOAPICSlot(dev));
    } else {
        ioapic.forbid(system.getIOAPICSlot(dev));
    }

    // clear buffer
    while (read(false) != -1) ;

    return old;
}
