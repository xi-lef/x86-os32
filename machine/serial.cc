#include "serial.h"

Serial::Serial(Serial::comPort port, Serial::baudRate baudrate, Serial::dataBits databits, Serial::stopBits stopbits, Serial::parity parity) : port(port)
{
	//TODO: Implementieren (falls du Lust dazu hast --> freiwillig)
	(void) baudrate;
	(void) databits;
	(void) stopbits;
	(void) parity;
}

void Serial::writeReg(Serial::regIndex reg, char out){
	//TODO: Implementieren (falls du Lust dazu hast --> freiwillig)
	(void) reg;
	(void) out;
}

char Serial::readReg(Serial::regIndex reg){
	//TODO: Implementieren (falls du Lust dazu hast --> freiwillig)
	(void) reg;
	return '\0';
}

int Serial::write(char out, bool blocking){
	//TODO: Implementieren (falls du Lust dazu hast --> freiwillig)
	(void) out;
	(void) blocking;
	return 0;
}

int Serial::read(bool blocking){
	//TODO: Implementieren (falls du Lust dazu hast --> freiwillig)
	(void) blocking;
	return 0;
}

