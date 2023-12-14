#include "../inc/Server.hpp"

Server::ClientSetup::ClientSetup(Server& server) : _server(server) {}

void Server::ClientSetup::setupNewClients(Client& client, int fd) {
	// if a client tries to connect to port 6667 (establish first connection)
	if (fd == _server._serverSocket)
		_server.addClient();

	else if (client.getSetupStatus() == PASS)
		checkPass(client);
	else if (client.getSetupStatus() == NICK)
		setupNick(client);
	else if (client.getSetupStatus() == USER && SKIP_ID == true) {
		client.setSetupStatus(REGISTERED);
		printWelcome(client);
	}
	else if (client.getSetupStatus() == USER)
		setupUser(client);
}

void Server::ClientSetup::checkQuit(Client& client, std::string input) {
		if (input.substr(0, 4) == "QUIT")
		_server.disconnectClient(client);
}

void Server::ClientSetup::promptPass(Client& client) {
	if (DEBUG)
		std::cout << "prompting pwd for user on fd " << client.getFd() << std::endl;
	client.sendToClient(PROMPT_PASS);
}

void Server::ClientSetup::promptNick(Client& client) {
	client.sendToClient(PROMPT_NICK);
}

void Server::ClientSetup::promptUser(Client& client) {
	std::string msg = "Welcome " + client.getNickName() + "\n" + PROMPT_USER;
	client.sendToClient(msg);
}

void Server::ClientSetup::parsePass(Client& client, std::string input) {
	checkQuit(client, input);
	std::string cmdPass = input.substr(0, 5);
	if (cmdPass != "PASS ") {
		client.sendToClient(ERR_CMD_PASS);
		return ;
	}
	std::string pass = input.substr(5, input.length());
	if (pass == _server._pass) {
		client.setSetupStatus(NICK);
		if (DEBUG)
			std::cout << "Client with fd " << client.getFd() << " entered correct pwd" << std::endl;
		promptNick(client);
	}
	else
		client.sendToClient(ERR_PASSWDMISMATCH);
}

void Server::ClientSetup::checkPass(Client& client) {
	std::string clientCmd;
	int clientInputStatus = client.readClientInput();
	
	if (clientInputStatus == 1) {
		clientCmd = client.getCmdBuffer();
		clientCmd = _server.processInput(&clientCmd[0], clientCmd.length());
		if (DEBUG)
			std::cout << "PASS setup input :" << clientCmd << std::endl;
		checkQuit(client, clientCmd);
		parsePass(client, clientCmd);
		client.clearCmdBuffer();
	}
	else if (clientInputStatus == 0) {
		std::cout << UIDCLIENT_CLOSE << std::endl;
		_server.disconnectClient(client);
	}
	else if (clientInputStatus < 0) {
		std::cerr << UIDCLIENT_RECV_ERR << std::endl;
		// client.sendToClient(ERR_RECV);
	}
}

int Server::ClientSetup::parseNick(Client& client, std::string input) {
	std::string cmdNick = input.substr(0, 5);

	std::vector<std::string> tokens = _server.tokenizeInput(input, ' ');

	if (tokens.size() < 2) {
		client.sendToClient(ERR_NONICKNAMEGIVEN);
		return -1;
	}

	if (tokens[0] != "NICK") {
		client.sendToClient(ERR_CMD_NICK);
		return -1;
	}

	std::string nick = tokens[1];
	if (tokens.size() > 2) {
		client.sendToClient(ERR_ERRONEUSNICKNAME);
		return -1;
	}

	if (_server.clientExists(nick) == true) {
		client.sendToClient(ERR_NICKNAMEINUSE);
		return -1;
	}

	if (nick.find(",") != std::string::npos || nick.find('#') == 0 || nick.find(':') == 0) {
		client.sendToClient(ERR_ERRONEUSNICKNAME);
		return -1;
	}

	client.setNickName(nick);
	if (SKIP_ID == false) {
		std::string msg = "Welcome " + nick + "\n" + PROMPT_USER;
		client.sendToClient(msg);
	}
	client.setSetupStatus(USER);
	return 0;
}

void Server::ClientSetup::setupNick(Client& client) {
	std::string clientCmd;
	int clientInputStatus = client.readClientInput();
	
	if (clientInputStatus == 1) {
		clientCmd = client.getCmdBuffer();
		clientCmd = _server.processInput(&clientCmd[0], clientCmd.length());
		if (DEBUG)
			std::cout << "NICK setup input :" << clientCmd << std::endl;
		checkQuit(client, clientCmd);
		parseNick(client, clientCmd);
		client.clearCmdBuffer();
		_server._clientsByNicks.insert(std::pair<std::string, Client*>(client.getNickName(), &client));
	}
	else if (clientInputStatus == 0) {
		std::cout << UIDCLIENT_CLOSE << std::endl;
		_server.disconnectClient(client);
	}
	else if (clientInputStatus < 0) {
		std::cerr << UIDCLIENT_RECV_ERR << std::endl;
		// client.sendToClient(ERR_RECV);
	}
}

int Server::ClientSetup::parseUser(Client& client, std::string input) {
	std::string cmdUser = input.substr(0, 5);

	if (cmdUser != "USER ") {
		client.sendToClient(ERR_CMD_USER);
		return 1;
	}

	input = input.substr(0, input.length());
	if (DEBUG)
		std::cout << "user setup input :" << input << std::endl;

	std::vector<std::string> tokens = _server.tokenizeInput(input, ' ');

	if (tokens.size() < 5) {
		client.sendToClient(ERR_CMD_USER);
		return 1;
	}

	std::string realname;
	for (size_t i = 4; i < tokens.size(); i++)
		realname += (tokens[i] + " ");
	realname.pop_back();

	client.setUserName(tokens[1]);
	client.setHostName(tokens[2]);
	client.setServerName(tokens[3]);
	client.setRealName(realname);
	
	client.setSetupStatus(REGISTERED);
	printWelcome(client);
	return 0;
}

void Server::ClientSetup::setupUser(Client& client) {
	std::string clientCmd;
	int clientInputStatus = client.readClientInput();
	
	if (clientInputStatus == 1) {
		clientCmd = client.getCmdBuffer();
		clientCmd = _server.processInput(&clientCmd[0], clientCmd.length());
		if (DEBUG)
			std::cout << "USER setup input :" << clientCmd << std::endl;
		checkQuit(client, clientCmd);
		parseUser(client, clientCmd);
		client.clearCmdBuffer();
	}
	else if (clientInputStatus == 0) {
		std::cout << UIDCLIENT_CLOSE << std::endl;
		_server.disconnectClient(client);
	}
	else if (clientInputStatus < 0) {
		std::cerr << UIDCLIENT_RECV_ERR << std::endl;
		// client.sendToClient(ERR_RECV);
	}
}

void Server::ClientSetup::printWelcome(Client& client) {
	std::cout << "Client joined : " << std::endl;
	client.printClientInfo();

	std::string msg = ":" + _server._serverName + " 001 " + client.getNickName() + " :You're all set, you can now chat :)\n";
	client.sendToClient(RPL_WELCOME);
	client.sendToClient(RPL_YOURHOST);
	client.sendToClient(RPL_CREATED);
	
	msg = ":" + client.getNickName() + " joined the server!\r\n";
	_server.sendToAll(msg);
}
