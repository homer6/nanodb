all: main.cc
	g++ --std=c++17 -I . -o sdb Connection.cc ListeningSocket.cc main.cc

clean: 
	rm sdb