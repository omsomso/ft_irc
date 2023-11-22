#include "../inc/Irc.hpp"

int Irc::setupServer() {
	_serverName = "super serveur";

	// Set up server socket for clients to connect to on localhost:6667
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in addressSetup;
	addressSetup.sin_family = AF_INET;
	addressSetup.sin_port = htons(6667);
	addressSetup.sin_addr.s_addr = inet_addr("127.0.0.1");

	// cast struct sockaddr_in into struct sockaddr*
	_serverAddress = (struct sockaddr*)&addressSetup;

	// bind serverSocket to localhost:6667
	if (bind(_serverSocket, _serverAddress, (socklen_t)sizeof(struct sockaddr)) < 0) {
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
		for (size_t i = 0; i < _fds.size(); i++) {
			// if there are POLLIN events from poll
			if (_fds[i].revents & POLLIN) {
				// if a client tries to connect to localhost:6667 (establish first connection)
				if (_fds[i].fd == _serverSocket) {
					addClient();
				}
				else if (POLLIN && _clients[_fds[i].fd].getSetupStatus() == 2)
					initNick(_clients[_fds[i].fd]);
				else if (POLLIN && _clients[_fds[i].fd].getSetupStatus() == 1)
					initUser(_clients[_fds[i].fd]);
				// else (if an existing client sends data)
				else {
					handleClient(_fds[i].fd);
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
	// server socket, struct sockaddr for client info, size of struc sockaddr as socklen_t pointer)
	struct sockaddr clientAddress;
	// clientPollfd.fd = accept(_serverSocket, (struct sockaddr*)&clientAddress, (socklen_t*)sizeof(clientAddress));
	clientPollfd.fd = accept(_serverSocket, nullptr, nullptr);
	// poll for in events
	clientPollfd.events = POLLIN;

	// set non-blocking mode for client socket
	fcntl(clientPollfd.fd, F_SETFL, O_NONBLOCK);

	// Add clientSocket to monitored file descriptors
	_fds.push_back(clientPollfd);

	Client newClient;
	newClient.setHostName(clientAddress.sa_data);
	newClient.setFd(clientPollfd.fd);
	_clients.insert(std::pair<int, Client>(clientPollfd.fd, newClient));
	printWelcome(newClient);

	// std::cout << "Added client!" << std::endl;
	return 0;
}

int Irc::initNick(Client& newClient) {
	if (newClient.getSetupStatus() == 2) {
		char buffer[1024];
		std::string input;
		// mandatory identification procedure
		// recv() shouldn't block the whole server MSG_DONTWAIT or fcntl
		size_t bytesReceived = recv(newClient.getFd(), buffer, sizeof(buffer), 0);
		buffer[bytesReceived] = 0x00;
		input = buffer;
		if (input.substr(0, 4) != "NICK") {
			std::cout << "Error : not a NICK command! Try again." << std::endl;
			return 1;
		}
		else {
			newClient.setNickName(input.substr(5, input.length() - 6));
			std::cout << "Welcome " << newClient.getNickName() << "!" << std::endl;
			std::cout << USER_REQUEST << std::endl;
			newClient.setSetupStatus(1);
		}
	}
	return 0;
}

int Irc::initUser(Client& newClient) {
	char buffer[1024];
	std::string input;

	size_t bytesReceived = recv(newClient.getFd(), buffer, sizeof(buffer), 0);
	buffer[bytesReceived] = 0x00;
	input = buffer;
	std::cout << "The user info you provided is " << input.substr(0, input.length() - 1) << std::endl;
	std::cout << "You can now chat!" << std::endl;
	// parse user info into newClient

	newClient.setSetupStatus(0);
	return 0;
}

int Irc::printWelcome(Client& newClient) {
	std::cout << "Welcome to " << _serverName << "." << std::endl;
	std::cout << NICK_REQUEST << std::endl;
	(void) newClient;
	return 0;
}

int Irc::handleClientCmd(size_t idx, std::string input, Client& client) {
	(void) idx;
	(void) input;
	(void) client;
	// compare input to avaliable commands
	// manage these commands
	return 0;
}

int Irc::handleClient(int fd) {
	char buffer[1024];
	size_t bytesReceived = recv(fd, buffer, sizeof(buffer), 0);

	if (bytesReceived > 0) {
		buffer[bytesReceived - 1] = 0x00;
		std::string clientInput = buffer;
		std::cout << _clients[fd].getNickName() << ": " << clientInput << std::endl;
	}

	else if (bytesReceived == 0) {
		std::cout << _clients[fd].getNickName() << " has closed the connection" << std::endl;

		// close the client socket
		close(fd);

		// remove clientPollfd from monitored sockets
		// probably should use map<int, pollfd>
		for (size_t i = 0; i < _fds.size(); i++) {
			if (_fds[i].fd == fd)
				_fds.erase(_fds.begin() + i);
		}

		// remove client from _clients
		_clients.erase(fd);
		return 1;
	}
	else {
		std::cerr << "Error : recv() from client " << _clients[fd].getNickName() << std::endl;
	}
	return 0;
}