#include <iostream>
#include <cstring>
#include <unistd.h>

#include "TCPComm.h"

void print(char * &buf, int &size, void * args) {
    for (int i =  0; i < size; i++)
        std::cout << buf[i];

    delete[] buf;
    buf = NULL;
    /*buf = new char[7];

    memcpy(buf, "RETURN\n", 7);
    size = 7;*/
}

int main(int, char**) {
    TCPComm tester(12345, 1, READ, &print, NULL);

    while (1) {
    	sleep(10);
    }
} 