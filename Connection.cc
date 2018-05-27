#include "Connection.h"


#include <iostream>
using std::cout;
using std::endl;
using std::cerr;


#include <sys/socket.h>	//socket()
#include <sys/types.h>  //socket()
#include <arpa/inet.h>  //htons()

#include <stdexcept>

#include <unistd.h> 	//close()

#include <netdb.h>

#include <strings.h>    //bcopy()



Connection::Connection( const string& address, const int port )
	:address(address), port(port)
{

	this->connection_fd = socket(AF_INET, SOCK_STREAM, 0);
	if( this->connection_fd < 0 ){
		throw std::runtime_error("Error opening socket.");
	}

	bzero( (char *) &this->server_address, sizeof(this->server_address) );
	this->server_address.sin_port = htons(port);

    this->server = gethostbyname( address.c_str() );
    if( this->server == nullptr ){
        throw std::runtime_error("Error: no host entry found.");
    }

    this->server_address.sin_family = this->server->h_addrtype;
    bcopy( 
		(char *) this->server->h_addr, 
		(char *) &this->server_address.sin_addr.s_addr,
		this->server->h_length
    );

    int connect_result = connect( this->connection_fd, (struct sockaddr *) &this->server_address, sizeof(this->server_address) );

    if( connect_result < 0 ){
    	close( this->connection_fd );
    	throw std::runtime_error( "Error connecting." );
    }
    
}

Connection::~Connection(){

	int result = close( this->connection_fd );
	if( result < 0 ){
		cerr << "Failed to close listening socket: " << result << endl;
	}

}



string Connection::send( const string& message ) const{

    int n = write( this->connection_fd, message.c_str(), message.size() );
    if( n < 0 ){
    	throw std::runtime_error( "Error sending message." );
    } 

    string response;
   
    constexpr int buffer_size = 4096;
	char response_buffer[buffer_size];

    n = read( this->connection_fd, response_buffer, buffer_size );
    if( n < 0 ){
    	throw std::runtime_error( "Error reading response." );
    }

    if( n > 0 ){
    	response += string( response_buffer, n );
    }    

    return response;

}
