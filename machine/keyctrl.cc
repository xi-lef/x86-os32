// vim: set et ts=4 sw=4:

/* INCLUDES */

#include "machine/keyctrl.h"
#include "debug/output.h"

/* GLOBALE VARIABLEN */

/* OEFFENTLICHE METHODEN */

// KEYBOARD_CONTROLLER: Initialisierung der Tastatur: alle LEDs werden
//                      ausgeschaltet und die Wiederholungsrate auf
//                      maximale Geschwindigkeit eingestellt.

Keyboard_Controller::Keyboard_Controller() :
	keydecoder(this), ctrl_port (0x64), data_port (0x60) {
	// alle LEDs ausschalten (bei vielen PCs ist NumLock nach dem Booten an)
	set_led (led_caps_lock, false);
	set_led (led_scroll_lock, false);
	set_led (led_num_lock, false);

	// maximale Geschwindigkeit, minimale Verzoegerung
	set_repeat_rate (0, 0);
}

Key Keyboard_Controller::key_hit() {
    while ((ctrl_port.inb() & outb) == 0) {
        //DBG << "waiting for keystroke..." << endl;
    }
	uint8_t code = data_port.inb();
    drainKeyboardBuffer();
    return keydecoder.decode(code);
}

void Keyboard_Controller::reboot() {
	const IO_Port CMOS_ctrl(0x70);
	const IO_Port CMOS_data(0x71);
	const IO_Port system_control_portA(0x92);
	DBG_VERBOSE << "rebooting smp" << endl;
	CMOS_ctrl.outb(0xe + 1);
	CMOS_data.outb(0);
	system_control_portA.outb(0x3);
}

void Keyboard_Controller::set_repeat_rate(int speed, int delay) {
    uint8_t config = (speed & 0b11111) | ((delay << 5) & 0b1100000); // TODO simplify

    DBG << "set repeat rate (" << "speed: " << speed << ", delay: " << delay << ")" << endl;

    send_command(kbd_cmd::set_speed, config);
}

void Keyboard_Controller::set_led(led_t led, bool on) {
    leds = on ? leds | led : leds & ~led;

    DBG << "set LEDs (" << ((led == 1) ? "scroll lock" : ((led == 2) ? "num lock" : "caps lock"))
        << " to " << (on ? "on" : "off") << ")" << endl;

    send_command(kbd_cmd::set_led, leds);
}

void Keyboard_Controller::drainKeyboardBuffer() {
    DBG_VERBOSE << "drain keyboard buffer. contents:" << endl;

    while ((ctrl_port.inb() & outb) == 1) {
        DBG_VERBOSE << keydecoder.decode(data_port.inb());
    }

    DBG_VERBOSE << endl;
    DBG_VERBOSE << "draining done" << endl;
}

void Keyboard_Controller::send_command(unsigned char cmd, unsigned char data) {
    // TODO avoid waiting for an ACK forever ?
    send_byte(cmd);
    while (data_port.inb() != kbd_reply::ack) {
        DBG_VERBOSE << "wait for ACK (command)" << endl;
    }

    send_byte(data);
    while (data_port.inb() != kbd_reply::ack) {
        DBG_VERBOSE << "wait for ACK (config)" << endl;
    }
}

void Keyboard_Controller::send_byte(unsigned char byte) {
    while ((ctrl_port.inb() & inpb) != 0) {
        DBG_VERBOSE << "wait for controller to read data" << endl;
    }
    data_port.outb(byte);
}
