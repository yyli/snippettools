#ifndef TCPCOMM_H
#define TCPCOMM_H

#include <iostream>
#include <deque>

#define HEADER_SIZE 12

struct TCPCommBootStrap {
    void *context;
    void *sock;
    void (*function)(char*&, int&, void *);
    void *args;
    void *id;
};

enum TCPCommReadType {
    READ,
    NOREAD,
    THREADED
};

class TCPComm {
    public:
        TCPComm(int port, int maxConn, TCPCommReadType sType, void (*process_loop) (char *&, int&, void *), void * args);
        TCPComm(const char* hostname, int port);
        ~TCPComm();

        int write(const char* buf, unsigned int size);
        int write(int sock, const char* buf, unsigned int size);
        int read(char** buf);
        int read(int sock, char** buf);

    private:
        void *server_loop(void *args);
        void *accept_loop(void *args);

        /* this is needed to boot strap the pthread creations */
        static void *accept_loop_helper(void *args);
        static void *server_loop_helper(void *args);

        int checkSum();

        bool setup;


        enum connectionType {
            SERVER,
            CLIENT, 
            NONE
        } type;

        TCPCommReadType readType;

        /* server stuff */
        pthread_t server_loop_id;
        int maxConn;
        
        int numConnections;

        pthread_t *pthread_connections;
        std::deque<int> available_connections;

        pthread_mutex_t *connMutex;

        /* shared thread_attr */
        pthread_attr_t thread_attr;

        /* client stuff */
        int internal_client_sock;
};

#endif