// vim: set et ts=4 sw=4:

#include "machine/cgascr.h"
// Verwendung der Klasse IO_Port für Zugriff auf die Register
#include "machine/io_port.h"
#include "debug/output.h"

static IO_Port index(0x3d4);
static IO_Port data(0x3d5);

CGA_Screen::Pixel * const CGA_Screen::CGA_BASE = (Pixel * const) 0xb8000;

CGA_Screen::CGA_Screen(int from_col, int to_col, int from_row, int to_row, bool use_cursor) :
    cur_x(from_col), cur_y(from_row), from_col(from_col), to_col(to_col), from_row(from_row),
    to_row(to_row), height(to_row - from_row), width(to_col - from_col), use_cursor(use_cursor) {}

void CGA_Screen::setpos(int x, int y) {
    // account for negative x and y
	if (x < 0) {
		x += COLUMNS;
	}
	if (y < 0) {
		y += ROWS;
	}

    // sanity checks
    if (x < from_col || x > to_col || y < from_row || y > to_row) {
        DBG << "CGA_S: invalid setpos (x: " << x << ", y: " << y << ")" << endl;
        return;
    }

	if (use_cursor) {
		int new_cursor = y * COLUMNS + x;
		index.outb(15);
		data.outb(new_cursor & 0xff);
		index.outb(14);
		data.outb((new_cursor >> 8) & 0xff);
	} else {
		cur_x = x;
		cur_y = y;
	}
}

void CGA_Screen::getpos(int& x, int& y) {
	if (use_cursor) {
		index.outb(15);
		uint16_t cursor = data.inb();
		index.outb(14);
		cursor |= data.inb() << 8;

		x = cursor % COLUMNS;
		y = cursor / COLUMNS;
        // sanity checks in case of unfortunate scheduling
        if (x < from_col || x > to_col || y < from_row || y > to_row) {
            DBG << "CGA_S: invalid getpos" << endl;
            x = 0;
            y = 0;
        }
	} else {
		x = cur_x;
		y = cur_y;
	}
}

void CGA_Screen::move_up_one_line(Attribute attrib) {
    for (int x = from_col; x <= to_col; x++) {
		for (int y = from_row; y <= to_row - 1; y++) {
		    CGA_BASE[y * COLUMNS + x] = CGA_BASE[(y + 1) * COLUMNS + x];
        }
        // set last (new) row to nothing with correct Attribute
        CGA_BASE[to_row * COLUMNS + x] = {{' ', attrib}};
	}
}

void CGA_Screen::LF(int& x, int& y, Attribute attrib) {
    x = from_col;
    if (++y > to_row) {
        move_up_one_line(attrib);
        y--;
    }
}

void CGA_Screen::print(char *string, int length, Attribute attrib) {
	int x, y;
	getpos(x, y);
    if (x < from_col || x > to_col || y < from_row || y > to_row) {
        DBG << "CGA_S: invalid print (x: " << x << ", y: " << y << ")" << endl;
        return;
    }

	for (int i = 0; i < length; i++) {
		if (string[i] == '\n') {
			LF(x, y, attrib);
            continue;
		}

		show(x, y, string[i], attrib);
		if (++x > to_col) {
            LF(x, y, attrib);
        }
	}

    setpos(x, y);
}

void CGA_Screen::reset(char character, Attribute attrib) {
    setpos(from_col, from_row);
	for (int x = from_col; x <= to_col; x++) {
		for (int y = from_row; y <= to_row; y++) {
			show(x, y, character, attrib);
		}
	}
}

void CGA_Screen::show(int x, int y, char character, Attribute attrib) {
	// account for negative x or y
	if (x < 0) {
		x += COLUMNS;
	}
	if (y < 0) {
		y += ROWS;
	}

    // sanity checks
    if (x < 0 || x >= COLUMNS || y < 0 || y >= ROWS) {
        DBG << "CGA_S: invalid show (x: " << x << ", y: " << y << ")" << endl;
        return;
    }

    CGA_BASE[y * COLUMNS + x] = {{character, attrib}};
}
