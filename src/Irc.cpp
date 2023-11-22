#include "../inc/Irc.hpp"

int Irc::setupServer() {
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	// Set up server socket for clients to connect to on localhost:6667

	_serverName = "super seveur";
	struct sockaddr_in addressSetup;
	addressSetup.sin_family = AF_INET;
	addressSetup.sin_port = htons(6667);
	addressSetup.sin_addr.s_addr = inet_addr("127.0.0.1");

	// cast struct sockaddr_in into struct sockaddr*
	// _serverAddress = reinterpret_cast<struct sockaddr*>(&addressSetup);
	(void) _serverAddress;

	// bind serverSocket to localhost:6667
	if (bind(_serverSocket, (struct sockaddr*)&addressSetup, sizeof(addressSetup)) < 0) {
		std::cerr << "Error : bind" << std::endl;
		return 1;
	}

	// mark the _serverSocket as passive
	if (listen(_serverSocket, SOMAXCONN) < 0) {
		std::cerr << "Error : listen" << std::endl;
		return 1;
	}
	// add serverSocket to the vector of sockets monitored by poll()
	struct pollfd serverPollfd;
	serverPollfd.fd = _serverSocket;
	serverPollfd.events = POLLIN;
	_fds.push_back(serverPollfd);

	std::cout << "Server initialised" << std::endl;

	return 0;
}

int Irc::monitor() {
	std::cout << "Starting monitoring for clients..." << std::endl;
	while (true) {
		// monitor the sockets vector
		int pollResult = poll(_fds.data(), _fds.size(), -1);

		// in case of error shut the whole thing down
		if (pollResult == -1) {
			std::cerr << "Error : poll" << std::endl;
			close(_serverSocket);
			return 1;
		}

		// for each socket
		for (size_t i = 0; i != _fds.size(); i++) {
			// if there are POLLIN events from poll
			if (_fds[i].revents & POLLIN) {
				// if a client tries to connect to localhost:6667 (establish first connection)
				if (_fds[i].fd == _serverSocket) {
					addClient();
				}
				// else (if an existing client sends data)
				else {
					handleClient(i);
				}
			}
		}
	}
	close(_serverSocket);
	return 0;
}
int Irc::addClient() {
	// Accept new client connection, monitor for incoming data
	struct pollfd clientPollfd;
	// server socket, struct sockaddr for client info, sizeof(struct sockaddr)
	clientPollfd.fd = accept(_serverSocket, nullptr, nullptr);
	// poll for in events
	clientPollfd.events = POLLIN;

	// set non-blocking mode for client socket
	fcntl(clientPollfd.fd, F_SETFL, O_NONBLOCK);

	// Add clientSocket to monitored file descriptors
	_fds.push_back(clientPollfd);



	std::cout << "Added client!" << std::endl;
	return 0;
}

int Irc::handleClientCmd(size_t idx, std::string input, Client client) {
	(void) idx;
	(void) input;
	(void) client;
	// compare input to avaliable commands except for NICK and USER
	// manage these commands
	return 0;
}

Client Irc::initClient(size_t idx) {
	(void) idx;
	Client currentClient;
	std::cout << "Welcome to " << _serverName << ". Please provide a nickname using the NICK command" << std::endl;
	// manage identification procedure...
	// recv() shouldn't block the whole server?
	return currentClient;
}

int Irc::handleClient(size_t idx) {
	char buffer[1010];
	size_t bytesReceived = recv(_fds[idx].fd, buffer, sizeof(buffer), 0);

	if (bytesReceived > 0) {
		// handleClientCmd(idx, input, ...);
		buffer[bytesReceived - 1] = 0x00;
		std::string clientInput = buffer;
		std::cout << "Client " << idx << " says : " << clientInput << std::endl;
	}

	else if (bytesReceived == 0) {
		std::cout << "Client " << idx << " has closed the connection" << std::endl;

		// close the client socket
		close(_fds[idx].fd);

		// removing the client causes a segfault ?
		// _fds.erase(_fds.begin() + idx);
		return 1;
	}
	else {
		std::cerr << "Error : recv() from client " << idx << std::endl;
	}

	// Do all the client things
	return 0;
}