#include <iostream>
#include <cstring>
#include <unistd.h>

#include "TCPComm.h"

void print(TCPComm &comm, int sock, void *args) {
    while (1) {
        if (comm.write(sock, "hello\n", 6) < 0)
            return;
    }
}

int main(int, char**) {
    TCPComm tester(12345, 2, &print, NULL);

    while (1) {
    	sleep(10);
    }
} 