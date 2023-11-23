#include "../inc/Irc.hpp"

Irc::Irc(int port, std::string pass, std::string serverName) : _port(port), _pass(pass), _serverName(serverName) {}

int Irc::setupServer() {
	// Set up server socket for clients to connect to on localhost:6667
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
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

	Channel general("general", "whatever", 0);
	_channels.push_back(general);
	return 0;
}

int Irc::monitor() {
	std::cout << "Waiting for incoming connections" << std::endl;
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
			Client& client = _clients[_fds[i].fd];
			// if there are POLLIN events from poll
			if (_fds[i].revents & POLLIN) {
				if (client.getSetupStatus() == 0)
					handleClient(client);
				setupNewClients(client, _fds[i].fd);
			}
		}
	}
	close(_serverSocket);
	return 0;
}

void Irc::setupNewClients(Client& client, int fd) {
	// if a client tries to connect to localhost:6667 (establish first connection)
	if (fd == _serverSocket)
		addClient();

	// test client status : 3 = no pass, 2 = no nick, 1 = no userdata
	// 0 = fully setup
	else if (client.getSetupStatus() == 3)
		testPass(client);
	else if (client.getSetupStatus() == 2)
		initNick(client);
	if (client.getSetupStatus() == 1 && SKIP_ID == true) {
		client.setSetupStatus(0);
		printReady(client, client.getFd());
	}
	else if (client.getSetupStatus() == 1)
		initUser(client);
}

int Irc::addClient() {
	struct pollfd clientPollfd;

	// accept new client connection on socket returned by accept
	clientPollfd.fd = accept(_serverSocket, nullptr, nullptr);

	// monitor (poll) for in events
	clientPollfd.events = POLLIN;

	// set non-blocking mode
	fcntl(clientPollfd.fd, F_SETFL, O_NONBLOCK);

	// add clientSocket to monitored file descriptors
	_fds.push_back(clientPollfd);

	// create client entry in _clients map
	Client newClient;
	newClient.setFd(clientPollfd.fd);
	_clients.insert(std::pair<int, Client>(clientPollfd.fd, newClient));
	std::cout << "Client added with fd " << clientPollfd.fd << std::endl;

	// ask client for pwd
	sendToClient(newClient.getFd(), PASS_PROMPT);
	return 0;
}

std::string Irc::fixNlCr(char buffer[], size_t len) {
	buffer[len] = 0x00;
	if (buffer[len - 1] == '\n' || 0x0D)
		buffer[len - 1] = 0x00;
	if (buffer[len - 2] == 0x0D)
		buffer[len - 2] = 0x00;
	return (buffer);
}

void Irc::testPass(Client& client) {
	int fd = client.getFd();
	char buffer[BUFFER_SIZE];
	std::string input;
	
	size_t len = recv(fd, buffer, sizeof(buffer), 0);
	if (len > 0) {

		input = fixNlCr(buffer, len);

		std::string cmdPass = input.substr(0, 5);
		if (cmdPass != "PASS ") {
			// std::string mess = "Type the password as follows : PASS <password>\n";
			// sendToClient(fd, mess);
			return ;
		}
		std::string pass = input.substr(5, input.length());
		if (pass == _pass) {
			client.setSetupStatus(2);
			std::cout << "Client wtih fd " << fd << " entered correct pwd" << std::endl;
			printWelcome(client);
		}
		else
			sendToClient(fd, WRONG_PASS);
	}
	else if (len == 0) {
		std::cout << UIDCLIENT_CLOSE << std::endl;
		disconnectClient(fd);
	}
	else {
		std::cerr << UIDCLIENT_RECV_ERR << std::endl;
		sendToClient(fd, RECV_ERR);
	}
}

void Irc::parseNick(Client& client, int fd, std::string input) {
	std::string cmdNick = input.substr(0, 5);
	std::string nick = input.substr(5, input.length() - 5);

	if (cmdNick != "NICK ") {
		sendToClient(fd, NICK_ERROR);
	}
	
	else {
		client.setNickName(nick);
		// std::cout << "Added nick " << nick << " for client with fd " << fd << std::endl;
		if (SKIP_ID == false) {
			std::string mess = "Welcome " + nick + "\n" + USER_REQUEST;
			sendToClient(fd, mess);
		}
		client.setSetupStatus(1);
	}
}

int Irc::initNick(Client& client) {
	int fd = client.getFd();
	char buffer[BUFFER_SIZE];
	std::string input;
	std::string cmdNick;
	std::string nick;
	
	size_t len = recv(fd, buffer, sizeof(buffer), 0);
	if (len > 0) {
		input = fixNlCr(buffer, len);
		parseNick(client, fd, input);
	}
	
	else if (len == 0) {
		std::cout << UIDCLIENT_CLOSE << std::endl;
		disconnectClient(fd);
	}

	else {
		std::cerr << UIDCLIENT_RECV_ERR << std::endl;
		sendToClient(fd, RECV_ERR);
	}
	return 0;
}

void Irc::printReady(Client &client, int fd) {

	std::cout << "Client joined : " << std::endl;
	client.printClientInfo();

	std::string mess = ":" + _serverName + " 001 " + client.getNickName() + " :You're all set, you can now chat :)\n";
	sendToClient(fd, mess);
	
	mess = client.getNickName() + " joined the chat!\n";
	sendToAll(mess);
}

int Irc::parseUser(Client& client, int fd, std::string input) {
	std::string cmdUser = input.substr(0, 5);

	if (cmdUser != "USER ") {
		sendToClient(fd, USER_ERROR);
		return 1;
	}

	input = input.substr(0, input.length() - 1);
	std::stringstream ss(input);
	std::string tmp;
	std::vector<std::string> tmpVec;
	int i = 0;

	while (std::getline(ss, tmp, ' ')) {
		i++;
		tmpVec.push_back(tmp);
	}

	for (size_t i = 0; i < tmpVec.size(); i++)
		std::cout << tmpVec[i] << "." << std::endl;
	std::cout << ";" << std::endl;
	std::cout << "i = " << i << std::endl;

	if (i < 5) {
		sendToClient(fd, USER_ERROR);
		return 1;
	}

	std::string realName = input.substr(input.find(":") + 1, input.length());
	if (realName.empty()) {
		sendToClient(fd, USER_ERROR);
		return 1;
	}

	// check for more stuff

	client.setUserName(tmpVec[1]);
	client.setHostName(tmpVec[2]);
	// what is servername doing here
	client.setRealName(realName);

	printReady(client, fd);
	return 0;
}

int Irc::initUser(Client& client) {
	int fd = client.getFd();
	std::string nick = client.getNickName();
	char buffer[BUFFER_SIZE];
	std::string input;

	size_t len = recv(fd, buffer, sizeof(buffer), 0);

	if (len > 0) {
		input = fixNlCr(buffer, len);
		if (!parseUser(client, fd, input))
			client.setSetupStatus(0);
	}
	
	else if (len == 0) {
		std::cout << nick << " has closed the connection" << std::endl;
		disconnectClient(fd);
	}

	else {
		std::cerr << "Error : recv() from client " << nick << std::endl;
		sendToClient(fd, RECV_ERR);
	}

	return 0;
}

void Irc::sendToClient(int fd, std::string mess) {
	send(fd, &mess[0], strlen(&mess[0]), 0);
}

void Irc::sendToAll(std::string mess) {
	for (size_t i = 1; i < _fds.size(); i++)
		send(_fds[i].fd, &mess[0], strlen(&mess[0]), 0);
}

int Irc::printWelcome(Client& client) {
	std::string mess = "Welcome to " + _serverName + "!\n" + NICK_REQUEST;
	sendToClient(client.getFd(), mess);
	return 0;
}

void Irc::disconnectClient(int fd) {
	std::cout << "Disconnected client with fd " << fd << std::endl;
	// close the client socket
	close(fd);

	// remove clientPollfd from monitored sockets
	for (size_t i = 0; i < _fds.size(); i++) {
		if (_fds[i].fd == fd)
			_fds.erase(_fds.begin() + i);
	}

	// remove client from _clients
	_clients.erase(fd);
}

void Irc::list(Client& client) {
	std::string tmp;
	for (size_t i = 0; i < _channels.size(); i++) {
		tmp = _channels[i].getChannelName() + "\n";
		sendToClient(client.getFd(), tmp);
	}
}

int Irc::handleClientCmd(Client& client, std::string input) {
	// compare input to avaliable commands
	// manage these commands according to op level
	int fd = client.getFd();

	// /NAMES returns all users of a channel
	if (input == "/NAMES") {
		std::string tmp;
		std::map<int, Client>::iterator it = _clients.begin();
		it++;
		while (it != _clients.end()) {
			tmp = it->second.getNickName() + "\n";
			sendToClient(fd, tmp);
			it++;
			// std::cout << it->second.getNickName() << std::endl;
		}
		// return 1;
	}

	// /LIST returns a list of all channels
	else if (input == "/LIST") {
		std::string tmp;
		for (size_t i = 0; i < _channels.size(); i++) {
			tmp = _channels[i].getChannelName() + "\n";
			sendToClient(fd, tmp);
		}
		// return 1;
	}

	// /JOIN joins a channel
	else if (input == "/JOIN ") {
		std::string channelName = input.substr(7, input.length());
		for (size_t i = 0; i < _channels.size(); i++) {
			if (channelName == _channels[i].getChannelName())
				_channels[i].addUser(client);
		}
	}
	return 0;
}

int Irc::handleClient(Client& client) {
	int fd = client.getFd();
	std::string nick = client.getNickName();
	char buffer[BUFFER_SIZE];

	size_t len = recv(fd, buffer, sizeof(buffer), 0);

	if (len == 0) {
		std::string mess = nick + " left the chat\n";
		sendToAll(mess);
		disconnectClient(fd);
	}

	else if (len < 0) {
		std::cerr << "Error : recv() from client " << nick << std::endl;
		sendToClient(fd, RECV_ERR);
	}

	if (len > 0) {
		std::string input = fixNlCr(buffer, len);
		std::string mess = nick + ": " + input + "\n";

		std::cout << "client input :" << input << std::endl;
		if (input[0] == '/')
			handleClientCmd(client, input);

		// delete this if no default server-wide channel
		else if (client.getChannelJoined() == -1)
			sendToAll(mess);
		else
			_channels[client.getChannelJoined()].sendToChannel(mess);
	}

	return 0;
}