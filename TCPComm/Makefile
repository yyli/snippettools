all: server client

server: TCPComm.cpp server.cpp
	g++ -g -Wall TCPComm.cpp server.cpp -o server -pthread

client: TCPComm.cpp client.cpp
	g++ -g -Wall TCPComm.cpp client.cpp -o client -pthread

clean:
	rm -rf server client
