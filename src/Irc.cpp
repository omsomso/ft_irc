#include "../inc/Irc.hpp"

Irc::Irc(int port, std::string const pass, std::string const serverName) : _port(port), _pass(pass), _serverName(serverName), _cmd(*this), _setup(*this) {}

int Irc::setupServer() {
	// Set up server socket for clients to connect to on localhost:6667
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	// int optval = 1;
	// if (setsockopt(_serverSocket, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval)) < 0) {
	// 	std::cerr << "Error : setsockopt" << std::endl;
	// 	return 1;
	// }
	struct sockaddr_in addressSetup;
	addressSetup.sin_family = AF_INET;
	addressSetup.sin_port = htons(_port);
	addressSetup.sin_addr.s_addr = inet_addr("127.0.0.1");

	// cast struct sockaddr_in into struct sockaddr*
	_serverAddress = (struct sockaddr*)&addressSetup;

	// bind serverSocket to localhost:6667
	if (bind(_serverSocket, _serverAddress, (socklen_t)sizeof(struct sockaddr)) < 0) {
		std::cerr << "Error : bind - try waiting a moment..." << std::endl;
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

	Channel general("general", "whatever");
	_channels.insert(std::pair<std::string, Channel>("general", general));
	return 0;
}

int Irc::monitor() {
	std::cout << "Waiting for incoming connections" << std::endl;

	// main monitoring loop
	while (true) {
		// monitor the sockets vector
		int pollResult = poll(_fds.data(), _fds.size(), -1);

		// in case of poll error shut the whole thing down
		if (pollResult == -1) {
			std::cerr << "Error : poll" << std::endl;
			close(_serverSocket);
			return 1;
		}

		// for each socket
		for (size_t i = 0; i < _fds.size(); i++) {

			// find client entry by socket in _clients map
			Client& client = _clients[_fds[i].fd];

			// if there are POLLIN events from poll
			if (_fds[i].revents & POLLIN) {
				if (client.getSetupStatus() == 0)
					handleClient(client);
				else
					_setup.setupNewClients(client, _fds[i].fd);
			}
		}
	}
	close(_serverSocket);
	return 0;
}

int Irc::addClient() {
	// create new client Pollfd
	struct pollfd clientPollfd;

	// accept new client connection on socket returned by accept()
	clientPollfd.fd = accept(_serverSocket, nullptr, nullptr);

	// monitor (poll) for in events
	clientPollfd.events = POLLIN;

	// set non-blocking mode
	fcntl(clientPollfd.fd, F_SETFL, O_NONBLOCK);

	// add clientPollfd to monitored file descriptors
	_fds.push_back(clientPollfd);

	// create client entry in _clients map
	Client newClient;
	newClient.setFd(clientPollfd.fd);
	_clients.insert(std::pair<int, Client>(clientPollfd.fd, newClient));

	if (DEBUG)
		std::cout << "Client added with fd " << clientPollfd.fd << std::endl;

	// prompt client for server password
	_setup.promptPass(newClient);
	return 0;
}

void Irc::disconnectClient(Client& client) {
	int fd = client.getFd();
	std::cout << "Disconnected client with fd " << fd << std::endl;
	
	// close the client socket
	close(fd);

	// remove clientPollfd from monitored sockets
	for (size_t i = 0; i < _fds.size(); i++) {
		if (_fds[i].fd == fd)
			_fds.erase(_fds.begin() + i);
	}

	// remove client from channel if joined one
	std::vector<std::string> chVec = client.getChannelsJoined();
	for (size_t i = 0; i < chVec.size(); i++) {
		std::string tmp = chVec[i];
		client.quitChannel(_channels[tmp]);
	}
	
	// remove nick from _clientsByNicks if user set nickName
	if (_clientsByNicks.find(client.getNickName()) != _clientsByNicks.end())
		_clientsByNicks.erase(client.getNickName());
	// remove client from _clients
	_clients.erase(fd);
}

int Irc::handleClient(Client& client) {
	std::string clientCmd;
	std::string nick = client.getNickName();

	int clientInputStatus = client.readClientInput();
	
	if (clientInputStatus == 1) {
		clientCmd = client.getCmdBuffer();
		clientCmd = processInput(&clientCmd[0], clientCmd.length());
		if (DEBUG)
			std::cout << "registered client input :" << clientCmd << std::endl;
		_cmd.handleClientCmd(client, clientCmd);
		client.clearCmdBuffer();

	}
	else if (clientInputStatus == 0) {
		std::string msg = nick + " left the server\n";
		sendToAll(msg);
		disconnectClient(client);
	}
	else if (clientInputStatus < 0) {
		std::cerr << UIDCLIENT_RECV_ERR << std::endl;
		// client.sendToClient(ERR_RECV);
	}
	// else if (client.getChName() == "")
	// 	sendToAll(msg);

	return 0;
}

void Irc::sendToAll(std::string msg) {
	for (size_t i = 1; i < _fds.size(); i++)
		send(_fds[i].fd, &msg[0], strlen(&msg[0]), 0);
}

std::vector<std::string> Irc::tokenizeInput(std::string input, char sep) {
	std::vector<std::string> tokens;
	std::stringstream ss(input);
	std::string token;
	while (std::getline(ss, token, sep))
		tokens.push_back(token);
	return tokens;
}

std::string	Irc::processInput(char buffer[], size_t len) {
	buffer[len] = 0x00;
	std::string out = buffer;
	size_t crnl = out.find_first_of("\r\n");
	out = out.substr(0, crnl);
	if (out.back() == ' ')
		out = out.substr(0, out.size() - 1);
	return out;
}

bool Irc::channelExists(std::string channel) {
	if (_channels.find(channel) == _channels.end())
		return false;
	else
		return true;
}

bool Irc::clientExists(std::string nick) {
	if (_clientsByNicks.find(nick) == _clientsByNicks.end())
		return false;
	else
		return true;
}