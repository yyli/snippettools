#include "TCPComm.h"

#include <deque>
#include <iostream>
#include <cstdio>
#include <cstring>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <signal.h>
#include <netdb.h>

TCPComm::TCPComm(const char* hostname, int port) : setup(false), type(CLIENT), internal_client_sock(-1) {
    signal(SIGPIPE, SIG_IGN);

    struct addrinfo hints, *servinfo, *p;
    memset(&hints, 0, sizeof(hints));

    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (port > 99999 || port < 0)
        return;

    char port_char[5];
    sprintf(port_char, "%d", port);

    if (getaddrinfo(hostname, port_char, &hints, &servinfo) != 0) {
        perror("can't getaddrinfo");
        return;
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((internal_client_sock = socket(p->ai_family, p->ai_socktype, 0)) == -1) {
            perror("client");
            continue;
        }

        if (connect(internal_client_sock, p->ai_addr, p->ai_addrlen) == -1) {
            close(internal_client_sock);
            perror("clent");
            continue;
        }

        break;
    }

    if (p == NULL) {
        std::cerr << "Failed to connect to " << hostname << ":" << port <<  std::endl;
        return;
    }

    freeaddrinfo(servinfo);

    setup = true;
}

TCPComm::TCPComm(int port, int maxConn, TCPCommReadType rType, void (*process_loop) (char *&, int&, void *), void * args) : setup(false), type(SERVER), readType(rType), server_loop_id(0), maxConn(maxConn) {
    signal(SIGPIPE, SIG_IGN);

    int sock;
    struct sockaddr_in in_addr;
    int one = 1;

    memset(&in_addr, 0, sizeof(in_addr));
    in_addr.sin_family      = AF_INET;
    in_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    in_addr.sin_port        = htons(port);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return;
    }

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) == -1) {
        perror("set SO_REUSEADDR");
        return;
    }

    if (bind(sock, (struct sockaddr *)&in_addr, sizeof(in_addr)) == -1) {
        perror("bind");
        return;
    }

    if (listen(sock, 20) < 0) {
        perror("listen");
        return;
    }

    socklen_t addrlen = sizeof(in_addr);
    if (getsockname(sock, (struct sockaddr *)&in_addr, &addrlen) == -1) {
        perror("getsockname");
        return;
    }

    TCPCommBootStrap *bootStrapArgs = new TCPCommBootStrap;
    bootStrapArgs->context  = this;
    bootStrapArgs->sock     = new int;
    bootStrapArgs->function = process_loop;
    bootStrapArgs->args     = args;
    memcpy(bootStrapArgs->sock, &sock, sizeof(int));

    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);

    if (pthread_create(&server_loop_id, &thread_attr, accept_loop_helper, (void*)bootStrapArgs) != 0) {
        perror("pthread_create accept_loop");
    }

    connMutex = new pthread_mutex_t;
    pthread_mutex_init(connMutex, NULL);
    pthread_mutex_lock(connMutex);
    pthread_connections = new pthread_t[maxConn]();
    for (int i = 0; i < maxConn; i++) {
        available_connections.push_back(i);
    }
    pthread_mutex_unlock(connMutex);

    setup = true;
}

void * TCPComm::accept_loop_helper(void *args) {
    TCPCommBootStrap *bootStrapArgs = (TCPCommBootStrap *)args;
    return ((TCPComm *)bootStrapArgs->context)->accept_loop(bootStrapArgs);
}

void * TCPComm::accept_loop(void *args) {
    TCPCommBootStrap *bootStrapArgs          = (TCPCommBootStrap *)args;
    int sock                                 = *(int *)bootStrapArgs->sock;
    void (*process_loop)(char*&, int&, void *) = bootStrapArgs->function;
    void *fargs                              = bootStrapArgs->args;
    delete (int *)bootStrapArgs->sock;
    delete bootStrapArgs;

    int client_sock;
    struct sockaddr_storage client_addr;
    socklen_t addrlen = sizeof(client_addr);

    pthread_mutex_lock(connMutex);
    numConnections = 0;
    pthread_mutex_unlock(connMutex);

    while (1) {
        client_sock = accept(sock, (struct sockaddr *)&client_addr, &addrlen);

        if (client_sock == -1) {
            std::cout << sock << std::endl;
            //perror("accept");
            continue;
        }

        pthread_mutex_lock(connMutex);
        if (numConnections >= maxConn) {
            std::cout << "Max number of connections achieved" << std::endl;
            pthread_mutex_unlock(connMutex);
            close(client_sock);
            continue;
        }
        if (available_connections.empty()) {
            std::cerr << "No available_connections even though there should be" << std::endl;
            pthread_mutex_unlock(connMutex);
            continue;
        }
        int avail_id = available_connections.front();
        available_connections.pop_front();

        bootStrapArgs           = new TCPCommBootStrap;
        bootStrapArgs->context  = this;
        bootStrapArgs->sock     = new int;
        memcpy(bootStrapArgs->sock, &client_sock, sizeof(int));
        bootStrapArgs->id       = new int;
        memcpy(bootStrapArgs->id, &avail_id, sizeof(int));
        bootStrapArgs->function = process_loop;
        bootStrapArgs->args     = fargs;

        if (pthread_create(&pthread_connections[avail_id], &thread_attr, server_loop_helper, (void *)bootStrapArgs) != 0) {
            perror("pthread_create server_loop");
            pthread_mutex_unlock(connMutex);
            continue;
        }

        numConnections++;
        pthread_mutex_unlock(connMutex);
    }

    pthread_exit(NULL);
}

void * TCPComm::server_loop(void *args) {
    TCPCommBootStrap *bootStrapArgs          = (TCPCommBootStrap *)args;
    int sock                                 = *(int *)bootStrapArgs->sock;
    int id                                   = *(int *)bootStrapArgs->id;
    void (*process_loop)(char*&, int&, void *) = bootStrapArgs->function;
    void *fargs                              = bootStrapArgs->args;

    delete (int *)bootStrapArgs->sock;
    delete (int *)bootStrapArgs->id;
    delete bootStrapArgs;

    char *buf;
    int buf_size = 0;

    while (1) {
        if (readType == READ) {
            buf_size = read(sock, &buf);
            if (buf_size <= 0)
                break;
        }

        if (process_loop != NULL)
            process_loop(buf, buf_size, fargs);

        if (buf != NULL && buf_size > 0) {
            if (write(sock, buf, buf_size) < 0) {
                std::cerr << "write failed" << std::endl;
                break;
            }
        }

        delete[] buf;
    }

    pthread_mutex_lock(connMutex);
    available_connections.push_back(id);
    numConnections--;
    pthread_mutex_unlock(connMutex);

    pthread_exit(NULL);
}

void * TCPComm::server_loop_helper(void *args) {
    TCPCommBootStrap *bootStrapArgs = (TCPCommBootStrap *)args;
    return ((TCPComm *)bootStrapArgs->context)->server_loop(args);
}

TCPComm::~TCPComm() {
    if (type == SERVER) {
        if (setup)
            pthread_cancel(server_loop_id);
        setup = false;
        pthread_mutex_destroy(connMutex);
        delete connMutex;
        pthread_attr_destroy(&thread_attr);
        for (int i = 0; i < maxConn; i++) {
            pthread_cancel(pthread_connections[i]);
        }
        delete[] pthread_connections;
        pthread_exit(NULL);
    }
}

int TCPComm::read(char** buf) {
    if (type != CLIENT) {
        std::cerr << "Not set up in CLIENT mode" << std::endl;
        return -1;
    }

    if (!setup) {
        std::cerr << "setting up during constructor failed" << std::endl;
        return -1;
    }

    if (internal_client_sock < 0)
        return -1;

    return read(internal_client_sock, buf);
}

int TCPComm::read(int sock, char** buf) {
    if (!setup) {
        std::cerr << "setting up during constructor failed" << std::endl;
        return -1;
    }

    char header[HEADER_SIZE];
    unsigned int total_read = 0;
    int n_read;

    char start[4];
    char end[4];
    uint32_t size;

    while (total_read < HEADER_SIZE) {
        n_read = recv(sock, header + total_read, HEADER_SIZE - total_read, 0);

        if (n_read < 0) {
            perror("couldn't recieve header data");
            return -1;
        } else if (n_read == 0) {
            return 0;
        }

        total_read += n_read;
    }

    memcpy(start, header, 4);
    memcpy(&size, header + 4, 4);
    memcpy(end, header + 8, 4);

    total_read = 0;
    *buf = new char[size];
    while (total_read < size) {
        n_read = recv(sock, *buf + total_read, size - total_read, 0);

        if (n_read < 0) {
            perror("couldn't recieve data");
            delete[] *buf;
            return -1;
        } else if (n_read == 0) {
            return 0;
        }

        total_read += n_read;
    }

    return size;
}

int TCPComm::write(const char* buf, unsigned int size) {
    if (type != CLIENT) {
        std::cerr << "Not set up in CLIENT mode" << std::endl;
        return -1;
    }

    if (!setup) {
        std::cerr << "setting up during constructor failed" << std::endl;
        return -1;
    }

    if (buf == NULL)
        return -1;

    if (internal_client_sock < 0)
        return -1;

    return write(internal_client_sock, buf, size);
}

int TCPComm::write(int sock, const char* buf, unsigned int size) {
    if (!setup) {
        std::cerr << "setting up during constructor failed" << std::endl;
        return -1;
    }

    if (buf == NULL)
        return -1;

    uint32_t portable_size = static_cast<uint32_t>(size);
    char header[HEADER_SIZE];
    memcpy(header, "$14$", 4);
    memcpy(header + 4, reinterpret_cast<char*>(&portable_size), 4);
    memcpy(header + 8, "$05$", 4);


    unsigned int total_sent = 0;
    int n_sent = 0;
    while (total_sent < HEADER_SIZE) {
        n_sent = send(sock, header + total_sent, HEADER_SIZE - total_sent, 0);

        if (n_sent <= 0) {
            perror("couldn't send header data");

            return -1;
        }

        total_sent += n_sent;
    }

    total_sent = 0;
    while (total_sent < size) {
        n_sent = send(sock, buf + total_sent, size - total_sent, 0);

        if (n_sent <= 0) {
            perror("couldn't send data");
        }

        total_sent += n_sent;
    }

    return 0;
}