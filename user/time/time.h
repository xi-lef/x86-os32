#pragma once

#include "types.h"
#include "machine/io_port.h"

#define TIMEZONE +1

struct Time {
    volatile uint16_t second;
    volatile uint16_t minute;
    volatile uint16_t hour;
    volatile uint16_t day;
    volatile uint16_t month;
    volatile uint16_t year;
    volatile uint16_t weekday;
    volatile uint16_t century;

    /*
    Time& operator=(const Time& t) {
        this->second = t.second;
        this->minute = t.minute;
        this->hour = t.hour;
        this->day = t.day;
        this->month= t.month;
        this->year= t.year;
        this->weekday= t.weekday;
        this->century= t.century;
        return *this;
    }*/
};

typedef enum {
    offset_second  = 0x00,
    offset_minute  = 0x02,
    offset_hour    = 0x04,
    offset_weekday = 0x06,
    offset_day     = 0x07,
    offset_month   = 0x08,
    offset_year    = 0x09,
    offset_statusA = 0x0a,
    offset_statusB = 0x0b,
    offset_statusC = 0x0c,
    offste_statusD = 0x0d,
    offset_century = 0x32
} cmos_offset;

/*!
 * Initializes the CMOS with fitting values. This includes:
 * * enabling the periodic interrupt
 * * enabling the update interrupt
 * * setting frequency of periodic interrupt to 128Hz
 * * setting data mode to binary instead of BCD
 */
void init_cmos();

/*!
 * Reads/writes the byte from/to the field specified in offset.
 * You must not use the ports directly. Instead, use this function.
 *
 * write_port returns the value from the field specified in offset prior
 * to writing.
 */
uint8_t read_port(cmos_offset offset);
uint8_t write_port(cmos_offset offset, uint8_t value);

/*!
 * Reads status-register C. This is necessary to keep interrupts coming.
 */
void clear_statusC();

/*!
 * Binary Coded Decimal to Integer conversion.
 * assumes an 8 bit BCD, so only values from 0 to 99 are supported.
 */
uint16_t bcd_to_int(uint8_t bcd);

/*! TODO
 * sleep waits for [time - 1, time] seconds. dont rely on precision!
 * sleep(1) can only really be used to wait until the next second starts
 * (good for clock displays ;)).
 */
void sleep(unsigned int time, bool from_clock = false);

Time get_time(bool from_clock = false);

uint8_t get_second (bool from_clock = false);
uint8_t get_minute (bool from_clock = false);
uint8_t get_hour   (bool from_clock = false);
uint8_t get_day    (bool from_clock = false);
uint8_t get_month  (bool from_clock = false);
uint8_t get_year   (bool from_clock = false);
uint8_t get_weekday(bool from_clock = false);
uint8_t get_century(bool from_clock = false);

class CGA_Stream; // fuck circular shit

/*!
 * clock is only meant to be used with a CGA_Stream thats exclusively for the clock!
 */
void clock(CGA_Stream& stream);

/*!
 * update_clock displays the current time on stream.
 */
void update_clock(CGA_Stream& stream);
