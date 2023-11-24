#include "../inc/Irc.hpp"

Irc::IrcClientSetup::IrcClientSetup(Irc &irc) : _irc(irc) {}

void Irc::IrcClientSetup::setupNewClients(Client& client, int fd) {
	// if a client tries to connect to localhost:6667 (establish first connection)
	if (fd == _irc._serverSocket)
		_irc.addClient();

	// test client status : 3 = no pass, 2 = no nick, 1 = no userdata
	// 0 = fully setup
	else if (client.getSetupStatus() == 3)
		checkPass(client);
	else if (client.getSetupStatus() == 2)
		setupNick(client);
	else if (client.getSetupStatus() == 1 && SKIP_ID == true) {
		client.setSetupStatus(0);
		printWelcome(client);
	}
	else if (client.getSetupStatus() == 1)
		setupUser(client);
}

void Irc::IrcClientSetup::checkQuit(Client& client, std::string input) {
		if (input.substr(0, 4) == "QUIT")
		_irc.disconnectClient(client);
}

void Irc::IrcClientSetup::promptPass(Client& client) {
	std::cout << "prompting pwd for used on fd " << client.getFd() << std::endl;
	_irc.sendToClient(client.getFd(), PROMPT_PASS);
}

void Irc::IrcClientSetup::promptNick(Client& client) {
	_irc.sendToClient(client.getFd(), PROMPT_NICK);
}

void Irc::IrcClientSetup::promptUser(Client& client) {
	std::string msg = "Welcome " + client.getNickName() + "\n" + PROMPT_USER;
	_irc.sendToClient(client.getFd(), msg);
}

void Irc::IrcClientSetup::checkPass(Client& client) {
	int fd = client.getFd();
	char buffer[BUFFER_SIZE];
	std::string input;
	
	size_t len = recv(fd, buffer, sizeof(buffer), 0);
	if (len > 0) {

		input = _irc.processInput(buffer, len);
		if (DEBUG)
			std::cout << "pass check input :" << input << std::endl;
		checkQuit(client, input);
		std::string cmdPass = input.substr(0, 5);
		if (cmdPass != "PASS ") {
			_irc.sendToClient(fd, ERR_CMD_PASS);
			return ;
		}
		std::string pass = input.substr(5, input.length());
		if (pass == _irc._pass) {
			client.setSetupStatus(2);
			std::cout << "Client wtih fd " << fd << " entered correct pwd" << std::endl;
			promptNick(client);
		}
		else
			_irc.sendToClient(fd, ERR_PASSWDMISMATCH);
	}
	else if (len == 0) {
		std::cout << UIDCLIENT_CLOSE << std::endl;
		_irc.disconnectClient(client);
	}
	else {
		std::cerr << UIDCLIENT_RECV_ERR << std::endl;
		_irc.sendToClient(fd, ERR_RECV);
	}
}

int Irc::IrcClientSetup::parseNick(Client& client, int fd, std::string input) {
	std::string cmdNick = input.substr(0, 5);
	std::string nick = input.substr(5, input.length() - 5);

	if (cmdNick != "NICK ") {
		_irc.sendToClient(fd, ERR_CMD_NICK);
		return -1;
	}

	for (size_t i = 1; i < _irc._fds.size(); i++) {
		if (_irc._clients[_irc._fds[i].fd].getNickName() == nick) {
			_irc.sendToClient(fd, ERR_NICKNAMEINUSE);
			return -1;
		}
	}

	client.setNickName(nick);
	_irc._clNames.push_back(nick);
	if (SKIP_ID == false) {
		std::string msg = "Welcome " + nick + "\n" + PROMPT_USER;
		_irc.sendToClient(fd, msg);
	}
	client.setSetupStatus(1);
	return 0;
}

void Irc::IrcClientSetup::setupNick(Client& client) {
	int fd = client.getFd();
	char buffer[BUFFER_SIZE];
	std::string input;
	
	size_t len = recv(fd, buffer, sizeof(buffer), 0);

	if (len > 0) {
		input = _irc.processInput(buffer, len);
		checkQuit(client, input);
		if (DEBUG)
			std::cout << "nick setup input :" << input << std::endl;
		parseNick(client, fd, input);
	}
	
	else if (len == 0) {
		std::cout << UIDCLIENT_CLOSE << std::endl;
		_irc.disconnectClient(client);
	}

	else {
		std::cerr << UIDCLIENT_RECV_ERR << std::endl;
		_irc.sendToClient(fd, ERR_RECV);
	}
	// return 0;
}

int Irc::IrcClientSetup::parseUser(Client& client, int fd, std::string input) {
	std::string cmdUser = input.substr(0, 5);

	if (cmdUser != "USER ") {
		_irc.sendToClient(fd, ERR_CMD_USER);
		return 1;
	}

	input = input.substr(0, input.length());
	if (DEBUG)
		std::cout << "user setup input :" << input << std::endl;
	std::stringstream ss(input);
	std::string tmp;
	std::vector<std::string> tmpVec;
	int i = 0;

	while (std::getline(ss, tmp, ' ')) {
		i++;
		tmpVec.push_back(tmp);
	}

	// for (size_t i = 0; i < tmpVec.size(); i++)
	// 	std::cout << tmpVec[i] << "." << std::endl;
	// std::cout << ";" << std::endl;
	// std::cout << "i = " << i << std::endl;

	if (i < 5) {
		_irc.sendToClient(fd, ERR_CMD_USER);
		return 1;
	}

	// check for more stuff ?

	std::string realname;
	for (size_t i = 4; i < tmpVec.size(); i++)
		realname += (tmpVec[i] + " ");
	realname.substr(0, realname.length() - 1);

	client.setUserName(tmpVec[1]);
	client.setHostName(tmpVec[2]);
	// what is servername doing here
	client.setServerName(tmpVec[3]);
	client.setRealName(realname);
	
	client.setSetupStatus(0);
	printWelcome(client);
	return 0;
}

void Irc::IrcClientSetup::setupUser(Client& client) {
	int fd = client.getFd();
	std::string nick = client.getNickName();
	char buffer[BUFFER_SIZE];
	std::string input;

	size_t len = recv(fd, buffer, sizeof(buffer), 0);

	if (len > 0) {
		input = _irc.processInput(buffer, len);
		checkQuit(client, input);
		parseUser(client, fd, input);
	}
	
	else if (len == 0) {
		std::cout << nick << " has closed the connection" << std::endl;
		_irc.disconnectClient(client);
	}

	else {
		std::cerr << "Error : recv() from client " << nick << std::endl;
		_irc.sendToClient(fd, ERR_RECV);
	}
}

void Irc::IrcClientSetup::printWelcome(Client& client) {
	std::cout << "Client joined : " << std::endl;
	client.printClientInfo();

	std::string msg = ":" + _irc._serverName + " 001 " + client.getNickName() + " :You're all set, you can now chat :)\n";
	// std::string msg = RPL_WELCOME;
	_irc.sendToClient(client.getFd(), RPL_WELCOME);
	_irc.sendToClient(client.getFd(), RPL_YOURHOST);
	_irc.sendToClient(client.getFd(), RPL_CREATED);
	
	msg = client.getNickName() + " joined the server!\n";
	_irc.sendToAll(msg);
}
