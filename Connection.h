#pragma once

#include <string>
using std::string;

#include <netinet/in.h>



class Connection{

	public:
		Connection( const string& address, const int port );

		virtual ~Connection();

		string send( const string& message ) const;  //returns the response


	private:
		string address;
		int port = 0;

		int connection_fd = 0;

		struct sockaddr_in server_address;
		struct hostent *server;

};