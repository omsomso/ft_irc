#include "../inc/Irc.hpp"

int Irc::setupServer() {
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	// Set up server socket for clients to connect to on localhost:6667

	struct sockaddr_in addressSetup;
	addressSetup.sin_family = AF_INET;
	addressSetup.sin_port = htons(6667);
	addressSetup.sin_addr.s_addr = inet_addr("127.0.0.1");

	// cast struct sockaddr_in into struct sockaddr*
	_serverAddress = reinterpret_cast<struct sockaddr*>(&addressSetup);

	// bind serverSocket to localhost:6667
	if (bind(_serverSocket, _serverAddress, sizeof(_serverAddress)) < 0) {
		return 1;
	}

	// add serverSocket to the vector of sockets monitored by poll()
	struct pollfd serverPollfd;
	serverPollfd.fd = _serverSocket;
	serverPollfd.events = POLLIN;
	_fds.push_back(serverPollfd);

	// manage errors...
	return 0;
}

int Irc::monitor() {
	while (true) {
		// monitor the sockets vector
		int pollResult = poll(_fds.data(), _fds.size(), -1);
		// in case of error shut the whole thing down
		if (pollResult == -1) {
			close(_serverSocket);
			return 1;
		}
		for (size_t i = 0; i != _fds.size(); i++) {
			// if there are POLLIN events from poll
			if (_fds[i].revents & POLLIN) {
				// if a client tries to connect to localhost:6667 (establish first onnection)
				if (_fds[i].fd == _serverSocket) {
					addClient();
				}
				// else (if an existing client sends data)
				else {
					handleClient();
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
	clientPollfd.fd = accept(_serverSocket, nullptr, nullptr);
	clientPollfd.events = POLLIN;

	// Add clientSocket to monitored file descriptors
	_fds.push_back(clientPollfd);
	return 0;
}

int Irc::handleClient() {
	// Do all the client things
	return 0;
}