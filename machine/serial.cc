#include "serial.h"
#include "debug/output.h"

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

    // prepare ier for reading
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
        if (!(readReg(lsr) & (1 << 5))) {
            DBG << "serial: bad write" << endl;
            return -1;
        } else {
            writeReg(tbr, out);
            return out;
        }
    } // else
    while (!(readReg(lsr) & (1 << 5))) ;

    writeReg(tbr, out);
	return out;
}

int Serial::read(bool blocking) {
    if (!blocking) {
        if (!(readReg(lsr) & 1)) {
            DBG << "serial: bad read" << endl;
            return -1;
        } else {
            return readReg(rbr);
        }
    } // else
    while (!(readReg(lsr) & 1)) ;

    return readReg(rbr);
}
