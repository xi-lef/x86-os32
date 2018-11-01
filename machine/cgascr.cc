// vim: set et ts=4 sw=4:

#include "machine/cgascr.h"
// Verwendung der Klasse IO_Port für Zugriff auf die Register
#include "machine/io_port.h"

CGA_Screen::CGA_Screen(int from_col, int to_col, int from_row, int to_row, bool use_cursor) :
    from_col(from_col), to_col(to_col), from_row(from_row), to_row(to_row), use_cursor(use_cursor),
    cur_x(from_col), cur_y(from_row) {}

void CGA_Screen::setpos(int x, int y) {
	// TODO? check if coordinates are outside of window // done?
	if (x > to_col - from_col + 1 || x < from_col - to_col - 1) { // to account for negative x
		x %= to_col - from_col + 1;
	}
	if (y > to_row - from_row + 1 || y < from_row - to_row - 1) { // to account for negative y
		y %= to_row - from_row + 1;
	}
	
	int new_x;
	int new_y;
	
	if (x < 0) {
		new_x = to_col + 1 + x;
	} else {
		new_x = from_col + x;
	}
		
	if (y < 0) {
		new_y = to_row + 1 + y;
	} else {
		new_y = from_row + y;
	}

	if (use_cursor) {
		int new_cursor = new_y * COLUMNS + new_x;
		IO_Port index(0x3d4);
		IO_Port data(0x3d5);
		index.outb(15);
		data.outb(new_cursor & 0xff);
		index.outb(14);
		data.outb((new_cursor >> 8) & 0xff);
	} else {
		cur_x = new_x;
		cur_y = new_y;
	}
}
	
void CGA_Screen::getpos(int& x, int& y) {
	if (use_cursor) {
		IO_Port index(0x3d4);
		IO_Port data(0x3d5);
		index.outb(15);
		uint16_t cursor = data.inb();
		index.outb(14);
		cursor += data.inb() << 8;
		
		x = cursor % COLUMNS;
		y = int(cursor / COLUMNS);
	} else {
		x = cur_x;
		y = cur_y;
	}
}

void CGA_Screen::move_up_one_line(void) {
    char *base = (char *) 0xb8000;
    /*int size_window = (to_row - from_row + 1) * (to_col - from_col + 1) * 2; // * 2 because two bytes per char
    int size_line   = (to_col - from_col + 1) * 2;                           // size of a single line in bytes
    for (int i = 0; i <= size_window - size_line; i++) {
        
    }*/

    for (int x = from_col; x <= to_col; x++) {
		for (int y = from_row; y <= to_row - 1; y++) {
            base[x * y * 2] = base[(x + 1) * (y + 1) * 2];
            base[x * y * 2 + 1] = base[(x + 1) * (y + 1) * 2 + 1];
		}
        base[x * to_row * 2] = 0;
        base[x * to_row * 2 + 1] = 0;
	}
}

void CGA_Screen::print(char* string, int length, Attribute attrib) {
	int x;
	int y;
	getpos(x, y);
	
	// check if string fits into current line (if its less than window width)
	if (length <= to_col - from_col + 1 && x + length - 1 > to_col) {
		x = from_col;
		if (y == to_row) {
            move_up_one_line();
        } else {
            y++;
        }
		setpos(x, y);
	}
	
	for (int i = 0; i < length; i++) {
		if (string[i] == '\n' || x == to_col) {
			x = from_col;
			if (++y > to_row) {
				move_up_one_line(); // TODO
				y--;
			}
			//setpos(x, y);
			
			if (string[i] == '\n') {
				continue;
			}
		}
		
		show(x, y, string[i], attrib);
		x++;
	}
}

void CGA_Screen::reset(char character, Attribute attrib) {
	cur_x = from_col;
	cur_y = from_row;
	for (int x = from_col; x <= to_col; x++) {
		for (int y = from_row; y <= to_row; y++) {
			show(x, y, character, attrib);
		}
	}
}

void CGA_Screen::show(int x, int y, char character, Attribute attrib) {
	// to account for negative x or y
	if (x < 0) {
		x += ROWS;
	}
	if (y < 0) {
		y += COLUMNS;
	}
	
	char *base = (char *) 0xb8000;
	int offset     = (y * COLUMNS + x) * 2;
	char *pos      = &(base[offset]);
	
	pos[0] = character;
	pos[1] = (attrib.foreground) | (attrib.background << 4) | (attrib.blink << 7);
}
