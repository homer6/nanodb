#include <iostream>
using std::cout;
using std::endl;
using std::cerr;


#include <string>
using std::string;

#include "ListeningSocket.h"
#include "Connection.h"


#include <istream>
#include <ostream>
#include <iterator>


int main( int argc, char** argv ){

	if( argc == 1 ){
		
		ListeningSocket socket( "0.0.0.0", 7000 );
		socket.listen();

	}else{

		Connection connection( "127.0.0.1", 7000 );

		// don't skip the whitespace while reading
		std::cin >> std::noskipws;

		// use stream iterators to copy the stream to a string
		std::istream_iterator<char> it(std::cin);
		std::istream_iterator<char> end;
		std::string results(it, end);

		cout << "Response: " << connection.send(results) << endl;

	}

	return 0;

}