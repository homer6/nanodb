#include <iostream>
using std::cout;
using std::endl;
using std::cerr;


#include <string>
using std::string;

#include "ListeningSocket.h"
#include "Connection.h"


int main( int argc, char** argv ){

	if( argc == 1 ){
		ListeningSocket socket( "0.0.0.0", 7000 );
		socket.listen();
	}else{
		Connection connection( "127.0.0.1", 7000 );
		cout << "Response: " << connection.send( "meow" ) << endl;
	}

	return 0;

}