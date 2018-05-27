#include <iostream>
using std::cout;
using std::endl;
using std::cerr;


#include <string>
using std::string;

#include "ListeningSocket.h"
#include "Connection.h"

/*
#include <istream>
#include <fstream>
#include <ostream>
#include <iterator>
*/

#include <unistd.h>
#include <fcntl.h>

#include <csignal>

 
namespace{
	volatile std::sig_atomic_t gSignalStatus;
}

void signal_handler( int signal ){
	gSignalStatus = signal;
	std::cout << "SignalValue: " << gSignalStatus << endl;
}


int main( int argc, char** argv ){

	if( argc == 1 ){

		// Install a signal handler
		std::signal( SIGPIPE, signal_handler );

		ListeningSocket socket( "0.0.0.0", 7000 );
		socket.listen();

	}else{

		Connection connection( "127.0.0.1", 7000 );


		constexpr int buffer_size = 128 * 1024; // 128 kiB
		char buffer[buffer_size];

		{
			//make cin nonblocking
		    int flags;
		    if( -1 == (flags = fcntl(STDIN_FILENO, F_GETFL, 0)) ){
		    	flags = 0;
		    }        
		    int result = fcntl( STDIN_FILENO, F_SETFL, flags | O_NONBLOCK );
		    if( result < 0 ){
		    	throw std::runtime_error("Failed to make stdin nonblocking.");
		    }
		}


		bool continue_reading = true;
		bool first_read_worked = false;

		int bytes_read = 0;

		int read_error_count = 0;

		while( continue_reading ){

			int n = read( STDIN_FILENO, buffer, buffer_size );
			if( n < 0 ){
				
				//cerr << "error reading... " << endl;
				read_error_count++;

				if( read_error_count > 10000 && bytes_read == 0 ){
					continue_reading = false;
				}

				continue;
			}

			//cout << "read: " << n << " bytes." << endl;
			
			if( n > 0 ){
				bytes_read += n;
				first_read_worked = true;
				string message( buffer, n );
				connection.send( message );
			}

			if( n == 0 ){
				continue_reading = false;
				continue;
			}

		}

		if( !first_read_worked ){
			string message( argv[1] );
			connection.send( message );
		}


	}

	return 0;

}