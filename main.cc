#include <iostream>
using std::cout;
using std::endl;
using std::cerr;


#include <string>
using std::string;

#include "ListeningSocket.h"
#include "Connection.h"

#include <unistd.h>
#include <fcntl.h>

#include <csignal>

#include <args.hxx>  //https://github.com/taywee/args


 
namespace{
	volatile std::sig_atomic_t gSignalStatus;
}

void signal_handler( int signal ){
	gSignalStatus = signal;
	std::cout << "SignalValue: " << gSignalStatus << endl;
}


int main( int argc, char** argv ){


    args::ArgumentParser parser( "StreamDB - A simple, streaming database.", "MIT License. Source and issues: https://github.com/homer6/streamdb" );
    args::HelpFlag help( parser, "help", "Display this help menu", {"help"} );
    
    args::Flag read_from_stdin( parser, "stdin", "Read from stdin (client).", {'i', "stdin"} );
    args::Flag run_as_server( parser, "server", "Run as server.", {'s', "server"} );

    args::ValueFlag<string> hostname(parser, "hostname", "The server to connect to (client; defaults to 127.0.0.1) or the address to listen on (server; defaults to 0.0.0.0).", { 'h', "hostname" } );
	args::ValueFlag<int> port(parser, "port", "Port to use. Defaults to 7250.", { 'p', "port" }, 7250 );

	args::Positional<string> client_message(parser, "message", "Message to send to server (if run as client).");

	args::CompletionFlag completion( parser, {"complete"} );


    try{
        parser.ParseCLI(argc, argv);
    }catch( args::Completion e ){
        std::cout << e.what();
        return 0;
    }catch( args::Help ){
        std::cout << parser;
        return 0;
    }catch( args::ParseError e ){
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }


	string hostname_str = args::get(hostname);
	int port_int = args::get(port);


    if( run_as_server ){

    	//run as server

		// Install a signal handler
		std::signal( SIGPIPE, signal_handler );

		if( hostname_str == "" ){
			hostname_str = "0.0.0.0";
		}

		ListeningSocket socket( hostname_str, port_int );
		socket.listen();

    }else{

		//run as client

		if( hostname_str == "" ){
			hostname_str = "127.0.0.1";
		}

		Connection connection( hostname_str, port_int );


		if( read_from_stdin ){

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

			int sleep_count = 0;

			while( continue_reading ){

				int n = read( STDIN_FILENO, buffer, buffer_size );
				if( n < 0 ){
					cout << "sleeping" << endl;
					//
					sleep_count++;
					if( sleep_count >= 200 ){
						cout << "actual sleep" << endl;
						sleep(1);
						sleep_count = 0;
					}

					continue;
				}

				sleep_count = 0;

				cout << "read: " << n << " bytes." << endl;
				
				if( n > 0 ){
					string message( buffer, n );
					connection.send( message );
				}

				if( n == 0 ){
					continue_reading = false;
					continue;
				}

			}

		}else{

			connection.send( args::get(client_message) );

		}

    }

    return 0;

}