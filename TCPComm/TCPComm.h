#ifndef TCPCOMM_H
#define TCPCOMM_H

#include <iostream>
#include <deque>

#define HEADER_SIZE 12

class TCPComm;

struct TCPCommBootStrap {
    void *context;
    void *sock;
    void (*function)(TCPComm&, int, void *);
    void *args;
    void *id;
};

class TCPComm {
    public:
        /* constructor for initializing a server 
         * Parameters:
         * port    - int   - port number to start server on
         * maxConn - int   - maximum number of connections
         * process_loop    - function pointer of the type: void(*)(TCPComm&, int, void *)
         * args    - void* - arguments for process_loop
         */
        TCPComm(int port, int maxConn, void (*process_loop) (TCPComm&, int, void *), void * args = NULL);

        /* constructor for initializing a client 
         * Parameters:
         * hostname - const char* - hostname to connect to
         * port     - int         - port number
         */
        TCPComm(const char* hostname, int port);
        ~TCPComm();

        /* write for the client, this function can only be called
         * if the object is constructed as a client
         * Parameters:
         * buf  - const char* - passed in buffer to write to socket
         * size - int         - amount of buffer to write
         * Return Value:
         * -1 - failure 
         *  0 - success
         */
        int write(const char* buf, unsigned int size);

        /* write for the server, this function can be called regardless of 
         * client or server
         * Parameters:
         * sock - int         - socket descriptor to write to
         * buf  - const char* - passed in buffer to write to socket
         * size - int         - amount of buffer to write
         * Return Value:
         * -1 - failure 
         *  0 - success
         */
        int write(int sock, const char* buf, unsigned int size);

        /* read for the client, this function can only be called
         * if the object is constructed as a client
         * Parameters:
         * buf  - char** - passed in buffer, allocated by the read function
         *                 Note: you must deallocate the buf that is returned 
         *                       by the function
         * Return Value:
         * -1    - failure 
         *  0    - peer has performed a orderly shutdown
         *  size - success
         */
        int read(char** buf);

        /* read for the server, this function can be called regardless of 
         * client or server
         * Parameters:
         * sock - int    - socket descriptor to write to
         * buf  - char** - passed in buffer, allocated by the read function
         *                 Note: you must deallocate the buf that is returned 
         *                       by the function
         * Return Value:
         * -1    - failure 
         *  0    - peer has performed a orderly shutdown
         *  size - success
         */
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