#include <iostream>
#include "TCPComm.h"

int main(int, char**) {
	TCPComm tester("localhost", 12345);
	while (1) {
		tester.write("hello\n", 6);
	}
} 