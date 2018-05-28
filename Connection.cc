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

#include <memory>



Connection::Connection( const string& address, const int port )
	:address(address), port(port)
{

	//detect address family
		struct addrinfo address_info_hint, *address_info_result = NULL;
		int address_info_return_value;

		bzero( &address_info_hint, sizeof(address_info_hint) );

		address_info_hint.ai_family = PF_UNSPEC;
		address_info_hint.ai_flags = AI_NUMERICHOST;

		address_info_return_value = getaddrinfo( address.c_str(), NULL, &address_info_hint, &address_info_result );
		if( address_info_return_value ){			
			throw std::runtime_error("Error: invalid server address.");
		}

		//ensure this result is freed
		std::unique_ptr<struct addrinfo, void(*)(struct addrinfo*)> address_info_result_ptr( address_info_result, freeaddrinfo );


	this->address_type = address_info_result_ptr->ai_family;

	cout << "address_info_result_ptr->ai_flags: " << address_info_result_ptr->ai_flags << endl;
	cout << "address_info_result_ptr->ai_family: " << address_info_result_ptr->ai_family << endl;
	cout << "address_info_result_ptr->ai_socktype: " << address_info_result_ptr->ai_socktype << endl;
	cout << "address_info_result_ptr->ai_protocol: " << address_info_result_ptr->ai_protocol << endl;
	cout << "address_info_result_ptr->ai_addrlen: " << address_info_result_ptr->ai_addrlen << endl;
	//cout << "address_info_result_ptr->ai_addr: " << address_info_result_ptr->ai_addr << endl;

	//cout << "address_info_result_ptr->ai_canonname: " << address_info_result_ptr->ai_canonname << endl;
	cout << "address_info_result_ptr->ai_next: " << address_info_result_ptr->ai_next << endl;


	if( this->address_type == AF_INET ){

		struct sockaddr_in *address = (struct sockaddr_in *) address_info_result_ptr->ai_addr;

		char address_string[256];
		if( inet_ntop( this->address_type, &address->sin_addr.s_addr, address_string, 256 ) == nullptr ){
			throw std::runtime_error("Could not convert network to presentation.");
		}

		cout << "address_info_result_ptr->ai_addr->sin_family: " << address->sin_family << endl;
		cout << "address_info_result_ptr->ai_addr->sin_port: " << address->sin_port << endl;
		cout << "address_info_result_ptr->ai_addr->sin_addr.s_addr: " << address_string << endl;

		/*           
			struct sockaddr_in {
               sa_family_t    sin_family; /* address family: AF_INET
               in_port_t      sin_port;   /* port in network byte order 
               struct in_addr sin_addr;   /* internet address
           };

           /* Internet address. 
           struct in_addr {
               uint32_t       s_addr;     /* address in network byte order
           };
        */


		this->address_length = sizeof( this->server_address );

		this->connection_fd = socket(this->address_type, SOCK_STREAM, 0);
		if( this->connection_fd < 0 ){
			throw std::runtime_error("Error opening socket.");
		}


		bzero( (char *) &this->server_address, this->address_length );
		this->server_address.sin_port = htons(port);

	    this->server_address.sin_family = this->address_type;
	    bcopy( 
			(char *) &address->sin_addr.s_addr,
			(char *) &this->server_address.sin_addr.s_addr,
			address_info_result_ptr->ai_addrlen
	    );

	    int connect_result = connect( this->connection_fd, (struct sockaddr *) &this->server_address, this->address_length );

	    if( connect_result < 0 ){
	    	close( this->connection_fd );
	    	throw std::runtime_error( "Error connecting." );
	    }




	}else if( this->address_type == AF_INET6 ){

		/*

           struct sockaddr_in6 {
               sa_family_t     sin6_family;   // AF_INET6
               in_port_t       sin6_port;     // port number 
               uint32_t        sin6_flowinfo; // IPv6 flow information 
               struct in6_addr sin6_addr;     /* IPv6 address 
               uint32_t        sin6_scope_id; /* Scope ID (new in 2.4) 
           };

           struct in6_addr {
               unsigned char   s6_addr[16];   /* IPv6 address
           };

		*/

        struct sockaddr_in6 *address = (struct sockaddr_in6 *) address_info_result_ptr->ai_addr;

		char address_string[256];
		if( inet_ntop( this->address_type, &address->sin6_addr.s6_addr, address_string, 256 ) == nullptr ){
			throw std::runtime_error("Could not convert network to presentation.");
		}


		cout << "address_info_result_ptr->ai_addr->sin6_family: " << address->sin6_family << endl;
		cout << "address_info_result_ptr->ai_addr->sin6_port: " << address->sin6_port << endl;
		cout << "address_info_result_ptr->ai_addr->sin6_flowinfo: " << address->sin6_flowinfo << endl;
		cout << "address_info_result_ptr->ai_addr->sin6_addr.s6_addr: " << address_string << endl;
		cout << "address_info_result_ptr->ai_addr->sin6_scope_id: " << address->sin6_scope_id << endl;



		this->address_length = sizeof( this->server_address_v6 );

		this->connection_fd = socket(this->address_type, SOCK_STREAM, 0);
		if( this->connection_fd < 0 ){
			throw std::runtime_error("Error opening socket.");
		}

		bzero( (char *) &this->server_address_v6, this->address_length );
		this->server_address_v6.sin6_port = htons(port);

	    this->server_address_v6.sin6_family = this->address_type;
	    bcopy( 
			(char *) &address->sin6_addr.s6_addr, 
			(char *) &this->server_address_v6.sin6_addr.s6_addr,
			address_info_result_ptr->ai_addrlen
	    );

	    int connect_result = connect( this->connection_fd, (struct sockaddr *) &this->server_address_v6, this->address_length );

	    if( connect_result < 0 ){
	    	close( this->connection_fd );
	    	throw std::runtime_error( "Error connecting." );
	    }


	}else{

		throw std::runtime_error("Only IPV4 or IPV6 is supported.");

	}









    
}

Connection::~Connection(){

	int result = close( this->connection_fd );
	if( result < 0 ){
		cerr << "Failed to close listening socket: " << result << endl;
	}

}



string Connection::send( const string& message ) const{

	//cout << "writing: " << message.size() << " bytes." << endl;

    int n = write( this->connection_fd, message.c_str(), message.size() );
    if( n < 0 ){
    	throw std::runtime_error( "Error sending message." );
    }

    //cout << "written: " << n << " bytes." << endl;

	
    string response;
   
   	/*
    constexpr int buffer_size = 4096;
	char response_buffer[buffer_size];

	
    n = read( this->connection_fd, response_buffer, buffer_size );
    if( n < 0 ){
    	throw std::runtime_error( "Error reading response." );
    }

    if( n > 0 ){
    	response += string( response_buffer, n );
    }
    */

    return response;

}
