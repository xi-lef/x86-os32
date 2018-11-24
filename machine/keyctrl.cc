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
	keydecoder(this), leds(0), ctrl_port (0x64), data_port (0x60) {
	// alle LEDs ausschalten (bei vielen PCs ist NumLock nach dem Booten an)
    set_led(led_caps_lock, false);
	set_led(led_scroll_lock, false);
	set_led(led_num_lock, false);

	// maximale Geschwindigkeit, minimale Verzoegerung
	set_repeat_rate(0, 0);
    drainKeyboardBuffer();
}

Key Keyboard_Controller::key_hit() {
    if (ctrl_port.inb() & auxb) {
        return Key();
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
    uint8_t config = speed | (delay << 5);

    DBG_VERBOSE << "set repeat rate (" << "speed: " << speed << ", delay: " << delay << ")" << endl;

    send_command(kbd_cmd::set_speed, config);
}

void Keyboard_Controller::set_led(led_t led, bool on) {
    leds = on ? (leds | led) : (leds & ~led);
    DBG_VERBOSE << "set LEDs (" << ((led == 1) ? "scroll lock" : ((led == 2) ? "num lock" : "caps lock"))
        << " to " << (on ? "on" : "off") << ")" << endl;

    send_command(kbd_cmd::set_led, leds);
}

void Keyboard_Controller::drainKeyboardBuffer() {
    while ((ctrl_port.inb() & outb) == 1) {
        data_port.inb();
    }
}

void Keyboard_Controller::send_command(unsigned char cmd, unsigned char data) {
    send_byte(cmd);
    send_byte(data);
}

void Keyboard_Controller::send_byte(unsigned char byte) {
    while ((ctrl_port.inb() & inpb) != 0) ;
    data_port.outb(byte);
}
