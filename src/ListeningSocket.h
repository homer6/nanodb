#pragma once

#include <string>
using std::string;

#include <netinet/in.h>


class ListeningSocket{

	public:
		ListeningSocket( const string& address, const int port );

		void listen();

		virtual ~ListeningSocket();


	private:
		string address;
		int port = 0;

		int listening_fd = 0;
		int client_fd = 0;

		in_addr_t address_type;
		socklen_t address_length;

		struct sockaddr_in server_address, client_address;
		struct sockaddr_in6 server_address_v6, client_address_v6;

};