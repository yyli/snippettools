#include <iostream>
#include "TCPComm.h"

int main(int, char**) {
    TCPComm tester("localhost", 12345);
    while (1) {
        if (tester.write("test\n", 6) < 0)
            break;
 /*       char *buf;
        int size = tester.read(&buf);
        if (size <= 0)
        	break;
        for (int i = 0; i < size; i++) {
        	std::cout << buf[i];
        }
        delete [] buf;
        */
   }
} 