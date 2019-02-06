// vim: set et ts=4 sw=4:

#include "machine/cgascr.h"
// Verwendung der Klasse IO_Port für Zugriff auf die Register
#include "machine/io_port.h"
#include "debug/output.h"

static IO_Port index(0x3d4);
static IO_Port data(0x3d5);

CGA_Screen::CGA_Screen(int from_col, int to_col, int from_row, int to_row, bool use_cursor) :
    cur_x(from_col), cur_y(from_row), from_col(from_col), to_col(to_col), from_row(from_row),
    to_row(to_row), use_cursor(use_cursor) {}

void CGA_Screen::setpos(int x, int y) {
    // to account for negative x and y
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

        // sanity check in case of unfortunate scheduling
        if (cursor < from_col * from_row || cursor > to_col * to_row) {
            DBG << "CGA_S: invalid getpos" << endl;
            x = 0;
            y = 0;
            return;
        }

		x = cursor % COLUMNS;
		y = int(cursor / COLUMNS);
	} else {
		x = cur_x;
		y = cur_y;
	}
}

void CGA_Screen::move_up_one_line(void) {
    uint16_t *base = (uint16_t *) BASE_ADDRESS;

    for (int x = from_col; x <= to_col; x++) {
		for (int y = from_row; y <= to_row - 1; y++) {
		    base[y * COLUMNS + x] = base[(y + 1) * COLUMNS + x];
        }
        // set last row to nothing with no color
        base[to_row * COLUMNS + x] = 0;
	}
}

void CGA_Screen::LF(int& x, int& y) {
    x = from_col;
    if (++y > to_row) {
        move_up_one_line();
        y--;
    }
}

void CGA_Screen::print(char* string, int length, Attribute attrib) {
	int x, y;
	getpos(x, y);
    if (x < from_col || x > to_col || y < from_row || y > to_row) {
        DBG << "CGA_S: invalid print (x: " << x << ", y: " << y << ")" << endl;
        return;
    }
	
	for (int i = 0; i < length; i++) {
		if (string[i] == '\n') {
			LF(x, y);
            continue;
		}

		show(x, y, string[i], attrib);
		if (++x > to_col) {
            LF(x, y);
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
	// to account for negative x or y
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

	char *base = BASE_ADDRESS;
	char *pos  = &(base[(y * COLUMNS + x) * 2]);

	pos[0] = character;
	pos[1] = char(attrib);
}
