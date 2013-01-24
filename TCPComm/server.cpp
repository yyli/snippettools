#include <iostream>
#include "TCPComm.h"

int main(int, char**) {
	TCPComm tester(12345, 1);
	while(1) {
		sleep(10);
	}
} 