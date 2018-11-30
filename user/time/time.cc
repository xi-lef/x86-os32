#include "user/time/time.h"
#include "debug/output.h"

static const IO_Port cmos_ctrl_port(0x70);
static const IO_Port cmos_data_port(0x71);

static bool initialized = false;
static Time current_time(get_time());
static volatile bool is_clock_active = false; // is clock() running right now?

void init_cmos() {
    if (initialized) {
        DBG << "cmos is init'd already" << flush;
        return;
    }

    // set frequency of periodic interrupt to 128Hz
    // frequency is the result of this --> 65536 / (2 ^ |)
    uint8_t statusA_old = read_port(offset_statusA); // v
    write_port(offset_statusA, (statusA_old & 0xf0) | 0b0110);

    // enable periodic interrupt & set data mode to binary
    uint8_t statusB_old = read_port(offset_statusB);
    write_port(offset_statusB, statusB_old | (1 << 6) | (1 << 2) | (1 << 4));
                                         // p interrupt  binary   u interrupt

    DBG_VERBOSE << "a: " << int(read_port(offset_statusA)) << " b: "
        << int(read_port(offset_statusB)) << " cmos initialized" << endl;
    initialized = true;
}

uint8_t read_port(cmos_offset offset) {
    // bit 7 (MSB) must NOT be changed (NMI)
    uint8_t ctrl_orig = cmos_ctrl_port.inb();
    uint8_t ctrl_new = (ctrl_orig & 0x80) | offset;

    cmos_ctrl_port.outb(ctrl_new);
    return cmos_data_port.inb();
}

uint8_t write_port(cmos_offset offset, uint8_t value) {
    uint8_t ctrl_orig = cmos_ctrl_port.inb();
    uint8_t ctrl_new = (ctrl_orig & 0x80) | offset;

    cmos_ctrl_port.outb(ctrl_new);
    uint8_t data_old = cmos_data_port.inb();
    cmos_data_port.outb(value);

    return data_old;
}

void clear_statusC() {
    read_port(offset_statusC);
}

uint16_t bcd_to_int(uint8_t bcd) {
    return (bcd >> 4) * 10 + (bcd & 0xf);
}

void sleep(unsigned int time, bool from_clock) {
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

static uint8_t get_value(cmos_offset offset, bool from_clock) {
    if (!from_clock && is_clock_active) {
        switch (offset) {
            case offset_second:  return current_time.second;
            case offset_minute:  return current_time.minute;
            case offset_hour:    return current_time.hour;
            case offset_day:     return current_time.day;
            case offset_month:   return current_time.month;
            case offset_year:    return current_time.year;
            case offset_weekday: return current_time.weekday;
            case offset_century: return current_time.century;
            default:             return 0;
        }
    }
    while ((read_port(offset_statusA) & (1 << 7)) == 1) ;
    return initialized ? read_port(offset) : bcd_to_int(read_port(offset));
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
    is_clock_active = true;
    stream.reset();
    for (;;) {
        stream.setpos(stream.from_col, stream.from_row);
        current_time = get_time(true);
        stream << current_time << flush;
        //sleep(1, true);
    }
}

void update_clock(CGA_Stream& stream) {
    current_time = get_time(is_clock_active);
    stream << current_time << flush;
}
