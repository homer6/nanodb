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

		struct sockaddr_in server_address, client_address;

};