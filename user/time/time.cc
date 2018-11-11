#include "user/time/time.h"
#include "debug/output.h"

static Time current_time(get_time());
static volatile bool clock_active = false; // is clock() running right now?

uint16_t bcd_to_int(uint8_t bcd) {
    return (bcd >> 4) * 10 + (bcd & 0xf);
}

void sleep(unsigned int time, bool from_clock) {
    /*for (unsigned int i = 0; i < time; i++) {
		unsigned int curr_count, prev_count = ~0;
		int delta;

		IO_Port port1(0x43);
		IO_Port port2(0x40);
		port1.outb(0x00);
		curr_count = port2.inb();
		curr_count |= port2.inb() << 8;

		do {
			prev_count = curr_count;
			port1.outb(0x00);
			curr_count = port2.inb();
			curr_count |= port2.inb() << 8;
			delta = curr_count - prev_count;

			// Comment from the Linux source code:

			// This limit for delta seems arbitrary, but it isn't, it's
			// slightly above the level of error a buggy Mercury/Neptune
			// chipset timer can cause.

		} while (delta < 300);
	}*/
    //DBG << "sleep for " << time - 1 << "s to " << time << "s" << endl;
    uint8_t start = get_second(from_clock);
    while (get_second(from_clock) < (start + time) % 60);
}

Time get_time(bool from_clock) {
    Time t = {
        .second  = get_second (from_clock),
        .minute  = get_minute (from_clock),
        .hour    = get_hour   (from_clock),
        .day     = get_day    (from_clock),
        .month   = get_month  (from_clock),
        .year    = get_year   (from_clock),
        .weekday = get_weekday(from_clock),
        .century = get_century(from_clock)
    };

    return t;
}

static uint8_t get_value(time_offset offset, bool from_clock) {
    if (!from_clock && clock_active) {
        switch (offset) {
            case offset_second:  return current_time.second;
            case offset_minute:  return current_time.minute;
            case offset_hour:    return current_time.hour;
            case offset_day:     return current_time.day;
            case offset_month:   return current_time.month;
            case offset_year:    return current_time.year;
            case offset_weekday: return current_time.weekday;
            case offset_century: return current_time.century;
        }
    }
    cmos_ctrl_port.outb(offset);
    return bcd_to_int(cmos_data_port.inb());
}

uint8_t get_second (bool from_clock) {
    return get_value(offset_second,  from_clock);
}

uint8_t get_minute (bool from_clock) {
    return get_value(offset_minute,  from_clock);
}

uint8_t get_hour   (bool from_clock) {
    return get_value(offset_hour,    from_clock);
}

uint8_t get_day    (bool from_clock) {
    return get_value(offset_day,     from_clock);
}

uint8_t get_month  (bool from_clock) {
    return get_value(offset_month,   from_clock);
}

uint8_t get_year   (bool from_clock) {
    return get_value(offset_year,    from_clock);
}

uint8_t get_weekday(bool from_clock) {
    return get_value(offset_weekday, from_clock);
}

uint8_t get_century(bool from_clock) {
    return get_value(offset_century, from_clock);
}

void clock(CGA_Stream& stream) {
    clock_active = true;
    stream.reset();
    for (;;) {
        stream.setpos(stream.from_col, stream.from_row);
        current_time = get_time(true);
        stream << current_time << flush;
        //sleep(1, true);
    }
}
