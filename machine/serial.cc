#include "serial.h"
#include "debug/output.h"

Serial::Serial(Serial::comPort port, Serial::baudRate baudrate, Serial::dataBits databits, Serial::stopBits stopbits, Serial::parity parity) :
    port(port), io_port_buffer(port), io_port_lcr(port + lcr), io_port_ier(port + ier), io_port_lsr(port + lsr) {
    /// baudrate
    // set DLAB bit
    //io_port_lcr.outb(1 << 7);
    writeReg(lcr, 1 << 7);

    // write lowbyte to transmitting buffer (DLAB low)
    //io_port_buffer.outb(baudrate & 0xff);
    writeReg(tbr, baudrate & 0xff);
    // write highbyte to ier (DLAB high)
    //io_port_ier.outb((baudrate >> 8) & 0xff);
    writeReg(ier, (baudrate >> 8) & 0xff);

    // unset DLAB bit
    //io_port_lcr.outb(0 << 7);
    writeReg(lcr, 0 << 7);

    /// databits + stopbits + parity
    //io_port_lcr.outb(databits | stopbits | parity);
    writeReg(lcr, databits | stopbits | parity);

    // prepare ier for reading
    //io_port_ier.outb(0x00);
    writeReg(ier, 0x00);
    //DBG << "serial: finished init" << endl;
}

void Serial::writeReg(Serial::regIndex reg, char out) {
    IO_Port p(port + reg);
    p.outb(out);
}

char Serial::readReg(Serial::regIndex reg) {
    IO_Port p(port + reg);
	return p.inb();
}

int Serial::write(char out, bool blocking) {
    if (!blocking && !(readReg(lsr) & (1 << 5))) {
        DBG << "serial: bad write" << endl;
        return -1;
    }
    while (!(readReg(lsr) & (1 << 5)));

    //io_port_buffer.outb(out);
    writeReg(tbr, out);

	return out;
}

int Serial::read(bool blocking) {
    if (!blocking && !(readReg(lsr) & 1)) {
        DBG << "serial: bad read" << endl;
        return -1;
    }
    while (!(readReg(lsr) & 1));

    return readReg(rbr);
}
