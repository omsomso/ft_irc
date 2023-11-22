#ifndef IRC_HPP
#define IRC_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "../inc/Client.hpp"

class Irc {
	private:
	int _serverSocket;
	std::string _serverName;
	struct sockaddr* _serverAddress;
	std::vector<pollfd> _fds;
	std::vector<Client> _clients;

	public:
	int setupServer();
	int monitor();
	int addClient();
	int handleClient(size_t idx);
	int handleClientCmd(size_t idx, std::string input, Client client);
	Client initClient(size_t idx);
};

#endif