#include "../inc/Irc.hpp"

Irc::IrcCommands::IrcCommands(Irc& irc) : _irc(irc) {}

void Irc::IrcCommands::handleClientCmd(Client& client, std::string input) {
	// 0 = cmd executed, -1 = cmd returned error, 1 = not a cmd

	// std::string nick = client.getNickName();
	if (input.empty())
		return ;

	_input = input;
	this->_tokens = _irc.tokenizeInput(input, ' ');

	// Commands that can't be reused
	if (_tokens[0] == "PASS" || _tokens[0] == "USER")
		alreadyRegistered(client);

	// PRIVMSG sends private message to a channel or user
	if (_tokens[0] == "PRIVMSG")
		privmsg(client);

	// LIST returns a list of all channels
	else if (_tokens[0] == "LIST")
		list(client);

	// JOIN joins a channel
	else if (_tokens[0] == "JOIN")
		join(client);

	// NICK changes nick if avaliable
	else if (_tokens[0] == "NICK")
		nick(client);

	// QUIT the server with an optional msg
	else if (_tokens[0] == "QUIT")
		quit(client);

	// KICK a user from a channel (op)
	else if (_tokens[0] == "KICK")
		kick(client);
	
	// TOPIC changes the topic of a channel (op)
	else if (_tokens[0] == "TOPIC")
		topic(client);

	// INVITE invites a user to a channel (op)
	else if (_tokens[0] == "INVITE")
		invite(client);

	// supersecret op status cheatcode
	else if (_tokens[0] == "TRIGGEROP")
		triggerOp(client);

	// MODE changes channel/user settings
	else if (_tokens[0] == "MODE")
		mode(client);
	else
		client.sendToClient(ERR_UNKNOWNCOMMAND);
	return ;
}

void Irc::IrcCommands::triggerOp(Client &client) {
	if (client.getOpStatus() == false)
		client.setOpStatus(true);
	else
		client.setOpStatus(false);
}

void Irc::IrcCommands::privmsg(Client& client) {
	std::string cmd = "PRIVMSG";

	std::string msg = ":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getHostName() + " ";
	msg += (_input + "\r\n");
	
	if (_tokens.size() < 2) {
		client.sendToClient(ERR_NEEDMOREPARAMS);
		return ;
	}
	if (_tokens.size() == 2) {
		client.sendToClient(ERR_NORECIPIENT);
		return ;
	}

	if (_tokens[1].find('#') == 0)
		msgChannel(client, msg);
	else
		msgUser(client, msg);
}

void Irc::IrcCommands::msgChannel(Client &client, std::string msg) {
	std::string target = _tokens[1];
	std::string channel;

	channel = target.substr(1, target.length() - 1);
	if (DEBUG)
		std::cout << "PRIVMSG parsed #channel :" << channel << std::endl;

	if (_irc.channelExists(channel) == false) {
		client.sendToClient(ERR_NOSUCHCHANNEL);
		return ;
	}
		
	Channel& targetChannel = _irc._channels[channel];

	if (client.hasJoinedChannel(channel) == false) {
		client.sendToClient(ERR_NOTONCHANNEL);
		return ;
	}

	else {
		targetChannel.sendToChannelButUser(client.getFd(), msg);

		if (DEBUG)
			std::cout << "PRIVMSG channel full msg :" << msg << std::endl;
	}
}

void Irc::IrcCommands::msgUser(Client &client, std::string msg) {
	std::string nick = _tokens[1];
	if (DEBUG)
		std::cout << "PRIVMSG parsed user :" << nick << std::endl;

	// perhaps change to another error cause limechat shows the convo regardless
	if (_irc.clientExists(nick) == false) {
		client.sendToClient(ERR_NOSUCHNICK);
		return ;
	}

	else {
		if (DEBUG) {
			std::cout << "PRIVMSG target user :" << nick << std::endl;
			std::cout << "PRIVMSG msg :" << msg << std::endl;
		}

		Client targetClient = _irc._clientsByNicks[nick];

		targetClient.sendToClient(msg);
	}
}


void Irc::IrcCommands::list(Client& client) {
	std::string msg = RPL_LIST;

	for (std::map<std::string, Channel>::iterator it = _irc._channels.begin(); it != _irc._channels.end(); it++) {
		msg += (it->first + " " + it->second.getNbUsers() + " :" + it->second.getChannelTopic() + ",");
	}
	
	msg = msg.substr(0, msg.length() - 1);
	msg += "\r\n";
	if (DEBUG)
		std::cout << "LIST cmd msg :" << msg << std::endl;
	client.sendToClient(msg);
	return ;
}

void Irc::IrcCommands::join(Client& client) {
	// Command: JOIN
	// Parameters: <channel>{,<channel>} [<key>{,<key>}]
	std::string cmd = "JOIN";

	if (_tokens.size() < 2) {
		client.sendToClient(ERR_NEEDMOREPARAMS);
		return ;
	}

	std::string channel;
	if (_tokens[1].find('#') == 0)
		channel = _tokens[1].substr(1, _tokens[1].length() - 1);
	else
		channel = _tokens[1];

	
	if (_irc.channelExists(channel) == false) {
		client.sendToClient(ERR_NOSUCHCHANNEL);
		return ;
	}

	if (client.hasJoinedChannel(channel) == true) {
		client.sendToClient(ERR_USERONCHANNEL);
		return ;
	}

	Channel& targetChannel = _irc._channels[channel];

	if (targetChannel.isInviteOnly() == true) {
		client.sendToClient(ERR_INVITEONLYCHAN);
		return ;
	}

	if (targetChannel.isFull() == true) {
		client.sendToClient(ERR_CHANNELISFULL);
		return ;
	}

	else {
		client.joinChannel(targetChannel);
		client.sendToClient(RPL_TOPIC);
		client.sendToClient(RPL_NAMREPLY);
		client.sendToClient(RPL_ENDOFNAMES);

		if (DEBUG) {
			std::cout << "TOPIC cmd msg :" << RPL_TOPIC << std::endl;
			std::cout << "NAMES cmd msg :" << RPL_NAMREPLY << std::endl;
		}

		return ;
	}

	client.sendToClient(ERR_CMD_JOIN);
	return ;
}

void Irc::IrcCommands::nick(Client& client) {
	// Command: NICK
	// Parameters: <nickname>
	std::string cmd = "NICK";

	if (_tokens.size() < 2) {
		client.sendToClient(ERR_NEEDMOREPARAMS);
		return ;
	}

	std::string nick = _tokens[1];

	if (nick.find(',') != std::string::npos) {
		client.sendToClient(ERR_ERRONEUSNICKNAME);
		return ;
	}

	if (nick.find('#') == 0 || nick.empty()) {
		client.sendToClient(ERR_ERRONEUSNICKNAME);
		return ;
	}

	if (_irc.clientExists(nick) == true) {
		client.sendToClient(ERR_NICKNAMEINUSE);
		return ;
	}

	std::string msg = client.getNickName() + " changed nick to " + nick + "\r\n";
	client.sendToClient(msg);

	client.setNickName(nick);

	return ;
}

void Irc::IrcCommands::quit(Client& client) {
	// Command: QUIT
	// Parameters: [<reason>]
	// if (client.getChName() != "") {
	// 	size_t len = (_input.find(':') + 1);
	// 	std::string msg = _input.substr(len, _input.length() - len);
		
	// 	if (DEBUG)
	// 		std::cout << "QUIT cmd msg :" << msg << std::endl;

	// 	client.getChJoined().sendToChannel(msg);
	// }
	_irc.disconnectClient(client);
	return ;
}

void Irc::IrcCommands::kick(Client& client) {
	// Command: KICK
	// Parameters: <channel> <user> *( "," <user> ) [<comment>]
	std::string cmd = "KICK";

	if (client.getOpStatus() != 1) {
		client.sendToClient(ERR_NOPRIVILEGES);
		return ;
	}

	if (_tokens.size() < 2) {
		client.sendToClient(ERR_NEEDMOREPARAMS);
		return ;
	}

	std::string channel = _tokens[1];
	std::string nick = _tokens[2];

	if (client.hasJoinedChannel(channel) == false) {
		client.sendToClient(ERR_NOTONCHANNEL);
		return ;
	}

	Client& targetClient = _irc._clientsByNicks[nick];
	Channel& targetChannel = _irc._channels[channel];
	if (targetClient.hasJoinedChannel(channel) == false) {
		client.sendToClient(ERR_USERNOTINCHANNEL);
		return ;
	}

	targetClient.quitChannel(targetChannel);
}

void Irc::IrcCommands::topic(Client& client) {
	//	Command: TOPIC
	//	Parameters: <channel> [<topic>]
	std::string cmd = "TOPIC";

	if (client.getOpStatus() != 1) {
		client.sendToClient(ERR_NOPRIVILEGES);
		return ;
	}

	if (_tokens.size() < 3) {
		client.sendToClient(ERR_NEEDMOREPARAMS);
		return ;
	}

	std::string channel = _tokens[1];

	if (client.hasJoinedChannel(channel) == false) {
		client.sendToClient(ERR_NOTONCHANNEL);
		return ;
	}

	if (_irc.channelExists(channel)) {
		client.sendToClient(ERR_NOSUCHCHANNEL);
		return ;
	}

	else {
		Channel& targetChannel = _irc._channels[channel];
		targetChannel.setChannelTopic(_tokens[2]);
		targetChannel.sendToChannel(RPL_TOPIC);
		
	}
}

void Irc::IrcCommands::alreadyRegistered(Client& client) {
	client.sendToClient(ERR_ALREADYREGISTERED);
	return ;
}

void Irc::IrcCommands::invite(Client &client) {
	// Command: INVITE
	// Parameters: <nickname> <channel>
	std::string cmd = "INVITE";

	if (client.getOpStatus() != 1) {
		client.sendToClient(ERR_NOPRIVILEGES);
		return ;
	}

	if (_tokens.size() < 3) {
		client.sendToClient(ERR_NEEDMOREPARAMS);
		return ;
	}

	std::string nick = _tokens[1];
	std::string channel = _tokens[2];

	if (_irc.clientExists(nick) == false) {
		client.sendToClient(ERR_NOSUCHNICK);
		return ;
	}
	
	if (_irc.channelExists(channel) == false) {
		client.sendToClient(ERR_NOSUCHCHANNEL);
		return ;
	}

	if (client.hasJoinedChannel(channel) == false) {
		client.sendToClient(ERR_NOTONCHANNEL);
		return ;
	}
	Channel targetChannel = _irc._channels[channel];

	if (targetChannel.isOnChannel(nick) == true) {
		client.sendToClient(ERR_USERONCHANNEL);	
		return ;
	}

	if (targetChannel.isFull() == true) {
		client.sendToClient(ERR_CHANNELISFULL);	
		return ;
	}

	else {
		Client& targetClient = _irc._clientsByNicks[nick];
		targetClient.sendToClient(RPL_INVITING);
		targetClient.joinChannel(targetChannel);
	}

}

void Irc::IrcCommands::mode(Client& client) {
	// Command: MODE
	// Parameters: <target> [<modestring> [<mode arguments>...]]
	std::string cmd = "MODE";
	if (client.getOpStatus() != 1) {
		client.sendToClient(ERR_NOPRIVILEGES);
		return ;
	}

	if (_tokens.size() < 3) {
		client.sendToClient(ERR_NEEDMOREPARAMS);
		return ;
	}

	if (_tokens[1].find('#') == 0)
		_tokens[1] = _tokens[1].substr(1, _tokens[1].length() -1);

	std::string channel = _tokens[1];
	std::string nick = _tokens[1];
	std::string modestring = _tokens[2];

	if (modestring.size() != 2 || modestring[0] != '+' || modestring[0] != '-') {
		client.sendToClient(ERR_INVALIDMODEPARAM);
		return ;
	}

	if (modestring.find_first_not_of("+-itklo") == std::string::npos) {
		client.sendToClient(ERR_INVALIDMODEPARAM);
		return ;
	}

	bool targetIsChannel = true;
	if (modestring[1] == 'o')
		targetIsChannel = false;

	if (targetIsChannel == true) {
		if (_irc.channelExists(channel) == false) {
				client.sendToClient(ERR_NOSUCHCHANNEL);
				return ;
			}
			if (client.hasJoinedChannel(channel) == false) {
				client.sendToClient(ERR_NOTONCHANNEL);
				return ;
			}
	}
	if (targetIsChannel == false) {
		if (_irc.clientExists(nick) == false) {
			client.sendToClient(ERR_NOSUCHNICK);
			return ;
		}
		Client& targetClient = _irc._clientsByNicks[nick];
		if (client.sharesAChannelWith(targetClient) == false) {
			client.sendToClient(ERR_NORECIPIENT);
			return ;
		}
	}

	bool status = false;
	if (modestring[0] == '+')
		status = true;
	// i: Set/remove Invite-only channel
	if (modestring[1] == 'i')
		_irc._channels[channel].setInviteStatus(status);
	// t: Set/remove the restrictions of the TOPIC command to channel operators
	if (modestring[1] == 't')
		_irc._channels[channel].setTopicRestrictedStatus(status);
	// k: Set/remove the channel key (password)
	if (modestring[1] == 'k')
		modeChannelKey(client, _irc._channels[channel], status);
	// l: Set/remove the user limit to channel
	if (modestring[1] == 'l')
		modeChannelLimit(client, _irc._channels[channel], status);
	// o: Give/take channel operator privilege
	if (modestring[1] == 'o')
		modeOpClient(client, status);
}

void Irc::IrcCommands::modeChannelKey(Client& client, Channel& channel, bool status) {
	std::string cmd = "MODE";

	if (status == true) {
		if (_tokens.size() < 3) {
			client.sendToClient(ERR_NEEDMOREPARAMS);
			return ;
		}
		size_t passBegin = _input.find(_tokens[3]);
		std::string newKey = _input.substr(passBegin, _input.length() - passBegin);
		if (DEBUG)
			std::cout << "MODE new key :" << newKey << " for channel :" << channel.getChannelName() << std::endl;
		channel.setKey(newKey);
		channel.setKeyProtected(true);
	}
	else {
		channel.setKeyProtected(false);
	}
}

void Irc::IrcCommands::modeChannelLimit(Client& client, Channel& channel, bool status) {
	std::string cmd = "MODE";
	if (status == true) {
		if (_tokens.size() < 3) {
			client.sendToClient(ERR_NEEDMOREPARAMS);
			return ;
		}
		int newLimit;
		try {
			newLimit = stoi(_tokens[3]);
		}
		catch (std::exception &e) {
			client.sendToClient(ERR_NEEDMOREPARAMS);
			return ;
		}
		newLimit = stoi(_tokens[3]);
		if (newLimit < channel.getUserCount()) {
			client.sendToClient(ERR_NEEDMOREPARAMS);
			return ;
		}
		if (DEBUG)
			std::cout << "MODE setting limit to :" << newLimit << " for channel :" << channel.getChannelName() << std::endl;
		channel.setUserLimit(newLimit);
	}
	else {
		channel.setUserLimit(-1);
	}
}

void Irc::IrcCommands::modeOpClient(Client& client, bool status) {
	std::string nick = _tokens[1];
	if (nick == client.getNickName()) {
		client.sendToClient(ERR_ERRONEUSNICKNAME);
		// self assigning op
		return ;
	}
	Client& targetClient = _irc._clientsByNicks[nick];
	if (status == true)
		targetClient.setOpStatus(true);
	else
		targetClient.setOpStatus(false);

	if (DEBUG) {
		if (status == true)
			std::cout << "MODE client " << nick << " is op now" << std::endl;
		else
			std::cout << "MODE client " << nick << " isn't op now" << std::endl;
	}
}