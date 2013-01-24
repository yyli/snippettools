#include <iostream>
#include "TCPComm.h"

void print(char *buf, int size, void * args) {
    for (int i = 0; i < size; i++)
        std::cout << buf[i];
}

int main(int, char**) {
    TCPComm tester(12345, 1, &print, NULL);

    while (1) {}
} 