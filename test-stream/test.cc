#include "console_out.h"
#include "file_out.h"

ConsoleOut cout;
FileOut foo("foo.txt");

int main(){
	cout << "Console Test <stream result> -> <expected>" << endl;
	cout << "   zero: " << 0 << " -> 0" << endl;
	cout << "   ten: " << (10) << " -> 10" << endl;
	cout << "   uint max: " << ~((unsigned int)0) << " -> 4294967295" << endl;
	cout << "   int max: " << ~(1<<31) << " -> 2147483647" << endl;
	cout << "   int min: " << (1<<31) << " -> -2147483648" << endl;
	cout << "   some int: " << (123456789) << " -> 123456789" << endl;
	cout << "   some negative int: " << (-123456789) << " -> -123456789" << endl;
	cout << "   binary: " << bin << 42 << dec << " -> 0b101010" << endl;
	cout << "   octal: " << oct << 42 << dec << " -> 052" << endl;
	cout << "   hex: " << hex << 42 << dec << " -> 0x2a" << endl;
    cout << "   octal: " << oct << -1 << dec << " -> 037777777777" << endl;
    cout << "   hex: " << hex << -1 << dec << " -> 0xffffffff" << endl;
	cout << "   pointer: " << ((void*)(3735928559u)) << " -> 0xdeadbeef" << endl;
	cout << endl;

	cout << "File Test" << endl
	     << "   currently open: " << FileOut::count() << endl
	     << "   writing into '" << foo.getPath() << "'..." << endl;
	foo << "C makes it easy to shoot yourself in the foot;" << endl;
	foo << "C++ makes it harder, but when you do it blows your whole leg off." << endl;
	{
		FileOut bar("bar.txt");
		cout << "   opened the " << FileOut::count() << ". file, " << endl;
		cout << "   writing into '" << bar.getPath() << "'..." << endl;
		bar << "Anyone who claims to have the perfect programming language is either a fool or a salesman or both." << endl;
	}
	cout << "   having only " << FileOut::count() << " file opened since the other is out of scope" << endl;

	return 0;
}

