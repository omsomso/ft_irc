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
#include <vector>
#include <sstream>

#include "../inc/Channel.hpp"
#include "../inc/Client.hpp"
#include "../inc/definitions.hpp"

#define SKIP_ID true
#define BUFFER_SIZE 1024

class Irc {
	private:
	int _port;
	std::string _pass;
	std::string _serverName;

	int _serverSocket;
	struct sockaddr* _serverAddress;

	std::vector<pollfd> _fds;
	std::map<int, Client> _clients;

	std::vector<Channel> _channels;

	void setupNewClients(Client& client, int fd);
	int addClient();
	std::string fixNlCr(char buffer[], size_t len);
	void testPass(Client& client);
	int printWelcome(Client& client);
	void parseNick(Client& client, int fd, std::string input);
	int initNick(Client& client);
	int parseUser(Client& client, int fd, std::string input);
	int initUser(Client& client);
	void printReady(Client &client, int fd);
	void disconnectClient(int fd);
	void sendToClient(int fd, std::string mess);
	void sendToAll(std::string mess);

	int handleClient(Client& client);
	int handleClientCmd(Client& client, std::string input);
	void list(Client& client);

	public:
	Irc(int port, std::string pass, std::string serverName);
	int setupServer();
	int monitor();
};

	// void sigAccept(void);
	// void sigHandle(int sig);

#endif