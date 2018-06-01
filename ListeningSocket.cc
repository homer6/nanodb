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

#include <netdb.h>

#include <fstream>

#include <memory>


ListeningSocket::ListeningSocket( const string& address, const int port )
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



			bzero( (char *) &this->server_address, sizeof(this->server_address) );
			bzero( (char *) &this->client_address, sizeof(this->client_address) );

			this->address_length = sizeof( this->client_address );

			//create socket

				this->listening_fd = socket(this->address_type, SOCK_STREAM, 0);
				if( this->listening_fd < 0 ){
					throw std::runtime_error("Error opening socket.");
				}

			//set address and bind

				this->server_address.sin_family = this->address_type;
				this->server_address.sin_port = htons(port);
				bcopy( 
					(char *) &address->sin_addr.s_addr,
					(char *) &this->server_address.sin_addr.s_addr,
					address_info_result_ptr->ai_addrlen
				);

				this->server_address.sin_addr.s_addr = INADDR_ANY;

				/*
				int inet_pton_result = inet_pton( this->address_type, address.c_str(), (struct in_addr*) &this->server_address.sin_addr );
				if( inet_pton_result == 0 ){
					throw std::runtime_error("inet_pton: Invalid server address.");
				}*/

				int bind_result = bind( this->listening_fd, (struct sockaddr *) &this->server_address, sizeof(this->server_address) );
				if( bind_result < 0 ){
					close( this->listening_fd );
					perror("bind");
					throw std::runtime_error( "Error on binding." );
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


			bzero( (char *) &this->server_address_v6, sizeof(this->server_address_v6) );
			bzero( (char *) &this->client_address_v6, sizeof(this->client_address_v6) );

			this->address_length = sizeof( this->client_address_v6 );

			//create socket

				this->listening_fd = socket(this->address_type, SOCK_STREAM, 0);
				if( this->listening_fd < 0 ){
					throw std::runtime_error("Error opening socket.");
				}

			//set address and bind

				this->server_address_v6.sin6_family = this->address_type;
				this->server_address_v6.sin6_port = htons(port);

				bcopy( 
					(char *) &address->sin6_addr.s6_addr,
					(char *) &this->server_address_v6.sin6_addr.s6_addr,
					address_info_result_ptr->ai_addrlen
				);

				struct in6_addr ia6 = IN6ADDR_ANY_INIT;

				this->server_address_v6.sin6_addr = ia6; //in6addr_any

				

				/*
				int inet_pton_result = inet_pton( this->address_type, address.c_str(), (struct in6_addr*) &this->server_address_v6.sin6_addr );
				if( inet_pton_result == 0 ){
					throw std::runtime_error("inet_pton(v6): Invalid server address.");
				}*/

				int bind_result = bind( this->listening_fd, (struct sockaddr *) &this->server_address_v6, sizeof(this->server_address_v6) );
				if( bind_result < 0 ){
					close( this->listening_fd );
					perror("bind");
					throw std::runtime_error( "Error on binding." );
				}


		}else{

			throw std::runtime_error("Only IPV4 or IPV6 is supported.");

		}



	//open listening fd

		constexpr int backlog_queue = 1000;

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

	constexpr int buffer_size = 128 * 1024; // 128 kiB
	char buffer[buffer_size];

	std::ofstream database_file;
	database_file.open( "stream.db" );
	
	while( 1 ){

		if( this->address_type == AF_INET ){
			this->client_fd = accept( this->listening_fd, (struct sockaddr *) &this->client_address, &this->address_length );
		}else if( this->address_type == AF_INET6 ){
			this->client_fd = accept( this->listening_fd, (struct sockaddr *) &this->client_address_v6, &this->address_length );
		}else{
			throw std::runtime_error("accept: Unknown address family.");
		}

		
		if( this->client_fd < 0 ){
			throw std::runtime_error("Error accepting connection.");
		}

		bool continue_reading = true;

		while( continue_reading ){

			int n = read( this->client_fd, buffer, buffer_size );
			if( n < 0 ){
				throw std::runtime_error("Error reading from socket.");
			}

			//cout << "read: " << n << " bytes." << endl;
			
			if( n > 0 ){
				string message( buffer, n );
				database_file << message;
				database_file.flush();
			}

			if( n == 0 ){
				continue_reading = false;
			}

			/*
			
			string response = "HTTP/1.1 200 OK\nContent-Length: 0\n\n";

			n = write( this->client_fd, response.c_str(), response.size() );
			if( n < 0 ){
				//close( this->client_fd );
				//continue_reading = false;
				continue;
				//throw std::runtime_error("Error writing response.");
			}*/
			

		}

		int result = close( this->client_fd );
		if( result < 0 ){
			cerr << "Failed to close client socket: " << result << endl;
		}

	}

	database_file.close();

}