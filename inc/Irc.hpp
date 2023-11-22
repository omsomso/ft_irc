#ifndef IRC_HPP
#define IRC_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <arpa/inet.h>

class Irc {
	private:
	int _serverSocket;
	struct sockaddr* _serverAddress;
	std::vector<pollfd> _fds;

	public:
	int setupServer();
	int monitor();
	int addClient();
	int handleClient();
};

#endif