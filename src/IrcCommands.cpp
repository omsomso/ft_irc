#include "../inc/Irc.hpp"

Irc::IrcCommands::IrcCommands(Irc& irc) : _irc(irc) {}

int Irc::IrcCommands::handleClientCmd(Client& client, std::vector<std::string> tokens) {
	// 0 = cmd executed, -1 = cmd returned error, 1 = not a cmd
	int ret = 1;

	// NAMES returns all users of a channel
	if (tokens[0] == "NAMES")
		ret = names(client);

	// LIST returns a list of all channels
	else if (tokens[0] == "LIST")
		ret = list(client);

	// JOIN joins a channel
	else if (tokens[0] == "JOIN")
		ret = join(client, tokens);

	// NICK changes nick if avaliable
	else if (tokens[0] == "NICK")
		ret = nick(client, tokens);

	// quit the server with an optional msg
	else if (tokens[0] == "QUIT")
		ret = quit(client, tokens);

	return ret;
}

int Irc::IrcCommands::list(Client& client) {
	//  RPL_LIST "<client> <channel> <client count> :<topic>"
	std::string msg = RPL_LIST;

	for (std::map<std::string, Channel>::iterator it = _irc._channels.begin(); it != _irc._channels.end(); it++) {
		msg += (it->first + " " + it->second.getNbUsers() + " :" + it->second.getChannelTopic() + ",");
	}
	
	msg = msg.substr(0, msg.length() - 1);
	msg += "\r\n";
	if (DEBUG)
		std::cout << "LIST cmd msg :" << msg << std::endl;
	_irc.sendToClient(client.getFd(), msg);
	// if (DEBUG)
	// 	std::cout << "LISTEND cmd msg :" << msg << std::endl;
	// _irc.sendToClient(client.getFd(), RPL_LISTEND);
	return 0;
}

int Irc::IrcCommands::names(Client& client) {
	std::string tmp;
	for (size_t i = 0; i < _irc._clNames.size(); i++) {
		tmp += (_irc._clNames[i] + ", ");
	}
	_irc.sendToClient(client.getFd(), RPL_NAMREPLY + tmp + "\r\n");
	return 0;
}

int Irc::IrcCommands::join(Client& client, std::vector<std::string> tokens) {
	// RPL_TOPIC 332 "<client> <channel> :<topic>"

	std::string channelName = tokens[1].substr(1, tokens[1].length() - 1);
	if (DEBUG)
		std::cout << "JOIN ch token name :" << channelName << std::endl;
	std::string msg = RPL_TOPIC;
	if (_irc._channels.find(channelName) != _irc._channels.end()) {
		_irc._channels[channelName].addUser(client);
		msg += (channelName + " :" + client.getChJoined().getChannelTopic() + "\r\n");
		if (DEBUG)
			std::cout << "TOPIC cmd msg :" << msg << std::endl;
		_irc.sendToClient(client.getFd(), msg);
		return 0;
	}

	_irc.sendToClient(client.getFd(), ERR_CMD_JOIN);
	return -1;
}

int Irc::IrcCommands::nick(Client& client, std::vector<std::string> tokens) {
	std::string nick = tokens[1];

	if (nick.empty() || nick.find_first_of(" ") != std::string::npos) {
		_irc.sendToClient(client.getFd(), ERR_CMD_NICK);
		return -1;
	}
	
	for (size_t i = 1; i < _irc._clNames.size(); i++) {
		if (_irc._clNames[i] == nick) {
			_irc.sendToClient(client.getFd(), ERR_NICKNAMEINUSE);
			return -1;
		}
	}

	std::string msg = client.getNickName() + " changed nick to " + nick + "\r\n";
	_irc.sendToClient(client.getFd(), msg);

	client.setNickName(nick);

	return 0;
}

int Irc::IrcCommands::quit(Client& client, std::vector<std::string> tokens) {
	if (!client.getChName().empty()) {
		std::string msg;
		for (size_t i = 1; i < tokens.size(); i++)
			msg += tokens[i] + " ";
		msg.substr(2, msg.length() - 2);
		if (DEBUG)
			std::cout << "QUIT cmd msg :" << msg << std::endl;
		client.getChJoined().sendToChannel(msg);
	}
	_irc.disconnectClient(client);
	return 0;
}