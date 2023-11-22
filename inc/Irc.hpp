#ifndef IRC_HPP
#define IRC_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <map>

#define NICK_REQUEST "Provide a nickname with the NICK command (NICK <nickname>)"
#define USER_REQUEST "Provide user info with the USER command (USER <username> <hostname> <servername> :<realname>"

#include "../inc/Client.hpp"

class Irc {
	private:
	int _serverSocket;
	std::string _serverName;
	struct sockaddr* _serverAddress;
	std::vector<pollfd> _fds;
	std::map<int, Client> _clients;

	public:
	int setupServer();
	int monitor();
	int addClient();
	int handleClient(int fd);
	int handleClientCmd(size_t idx, std::string input, Client& client);
	int printWelcome(Client& newClient);
	int initNick(Client& newClient);
	int initUser(Client& newClient);
};

#endif