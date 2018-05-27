#include "ListeningSocket.h"


#include <iostream>
using std::cout;
using std::endl;
using std::cerr;


#include <sys/socket.h>	//socket()
#include <sys/types.h>  //socket()
#include <arpa/inet.h>  //htons()

#include <stdexcept>

#include <unistd.h> 	//close()

#include <strings.h>    //bcopy()



ListeningSocket::ListeningSocket( const string& address, const int port )
	:address(address), port(port)
{

	this->listening_fd = socket(AF_INET, SOCK_STREAM, 0);
	if( this->listening_fd < 0 ){
		throw std::runtime_error("Error opening socket.");
	}

	bzero( (char *) &this->server_address, sizeof(this->server_address) );
	bzero( (char *) &this->client_address, sizeof(this->client_address) );


	this->server_address.sin_family = AF_INET;
	this->server_address.sin_port = htons(port);
	this->server_address.sin_addr.s_addr = INADDR_ANY;


	if( bind(this->listening_fd, (struct sockaddr *) &this->server_address, sizeof(this->server_address)) < 0 ){
		close( this->listening_fd );
		throw std::runtime_error( "Error on binding." );
	}

	constexpr int backlog_queue = 5;

	int listen_result = ::listen( this->listening_fd, backlog_queue );
	if( listen_result < 0 ){
		close( this->listening_fd );
		throw std::runtime_error( "Error listening to socket." );
	}
    
}

ListeningSocket::~ListeningSocket(){

	int result = close( this->listening_fd );
	if( result < 0 ){
		cerr << "Failed to close listening socket: " << result << endl;
	}

}



void ListeningSocket::listen(){

	constexpr int buffer_size = 4096;
	char buffer[buffer_size];

	socklen_t client_length = sizeof( this->client_address );

	while( 1 ){

		this->client_fd = accept( this->listening_fd, (struct sockaddr *) &this->client_address, &client_length );
		if( this->client_fd < 0 ){
			throw std::runtime_error("Error accepting connection.");
		}

		//bzero( buffer, buffer_size );
		int n = read( this->client_fd, buffer, buffer_size );
		if( n < 0 ){
			throw std::runtime_error("Error reading from socket.");
		}

		if( n > 0 ){
			string message( buffer, n );
			cout << "Message: " << message << endl;
		}

		string response = "OK";

		n = write( this->client_fd, response.c_str(), response.size() );
		if( n < 0 ){
			close( this->client_fd );
			throw std::runtime_error("Error writing response.");
		}

		int result = close( this->client_fd );
		if( result < 0 ){
			cerr << "Failed to close client socket: " << result << endl;
		}

	}

}