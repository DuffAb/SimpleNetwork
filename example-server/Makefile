all: 
g++ ../src/OSocket.cpp -c -o ./bin/OSocket.o
g++ ../src/OUnixSocket.cpp -c -o ./bin/OUnixSocket.o
g++ ../src/TCPServer.cpp -c -o ./bin/TCPServer.o
g++ ./bin/*.o server.cpp -Wall -o ./server
	#g++ -Wall -o server server.cpp -I../src/ ../src/TCPServer.cpp ../src/TCPClient.cpp -std=c++11 -lpthread
