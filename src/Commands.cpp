#include "../inc/Server.hpp"

Server::Commands::Commands(Server& server) : _server(server) {}

void Server::Commands::handleClientCmd(Client& client, std::string input) {
	if (input.empty())
		return ;

	_input = input;
	this->_tokens = _server.tokenizeInput(input, ' ');

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

	// NAMES lists all users in a channel
	else if (_tokens[0] == "NAMES")
		names(client);
	
	// PART leaves a channel
	else if (_tokens[0] == "PART")
		part(client);

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

	// Handle Direct Client-to-Client
	else if (_tokens[0] == "CHATBOT") {
        std::string chatBotResponse = handleChatBotRequest(_tokens, client);
        client.sendToClient(chatBotResponse);
    }

	// Handle ChatBot
	else if (_tokens[0] == "CHATBOT")
		handleDCC(input, client);

	// MODE changes channel/user settings
	else if (_tokens[0] == "MODE")
		mode(client);
	else
		client.sendToClient(ERR_UNKNOWNCOMMAND);
	return ;
}

void Server::Commands::triggerOp(Client &client) {
	if (client.getOpStatus() == false) {
		client.setOpStatus(true);
		client.sendToClient(RPL_YOUREOPER);
	}
	else
		client.setOpStatus(false);
}

void Server::Commands::privmsg(Client& client) {
	std::string cmd = "PRIVMSG";

	std::string msg = ":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getHostName() + " :I";
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

void Server::Commands::msgChannel(Client &client, std::string msg) {
	std::string channel = _tokens[1];
	if (_tokens[1].find('#') == 0)
		channel.erase(0, 1);

	if (DEBUG)
		std::cout << "PRIVMSG parsed #channel :" << channel << std::endl;

	if (_server.channelExists(channel) == false) {
		client.sendToClient(ERR_NOSUCHCHANNEL);
		return ;
	}
		
	Channel& targetChannel = _server._channels[channel];

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

void Server::Commands::msgUser(Client &client, std::string msg) {
	std::string nick = _tokens[1];
	if (DEBUG)
		std::cout << "PRIVMSG parsed user :" << nick << std::endl;

	if (_server.clientExists(nick) == false) {
		client.sendToClient(ERR_NOSUCHNICK);
		return ;
	}

	else {
		if (DEBUG) {
			std::cout << "PRIVMSG target user :" << nick << std::endl;
			std::cout << "PRIVMSG msg :" << msg << std::endl;
		}

		Client targetClient = *_server._clientsByNicks[nick];

		targetClient.sendToClient(msg);
	}
}


void Server::Commands::list(Client& client) {
	std::string msg = RPL_LIST;

	for (std::map<std::string, Channel>::iterator it = _server._channels.begin(); it != _server._channels.end(); it++) {
		msg += (it->first + " " + it->second.getNbUsers() + " :" + it->second.getChannelTopic());
		msg += "\r\n";
		if (DEBUG)
			std::cout << "LIST cmd msg :" << msg << std::endl;
		client.sendToClient(msg);
		msg = RPL_LIST;
	}
	
	return ;
}

void Server::Commands::join(Client& client) {
	// Command: JOIN
	// Parameters: <channel>{,<channel>} [<key>{,<key>}]
	std::string cmd = "JOIN";
	std::string nick = client.getNickName();

	if (_tokens.size() < 2) {
		client.sendToClient(ERR_NEEDMOREPARAMS);
		return ;
	}

	std::string channel = _tokens[1];
	if (_tokens[1].find('#') == 0)
		channel.erase(0, 1);
	
	if (_server.channelExists(channel) == false) {
		client.sendToClient(ERR_NOSUCHCHANNEL);
		return ;
	}

	if (client.hasJoinedChannel(channel) == true) {
		client.sendToClient(ERR_USERONCHANNEL);
		return ;
	}

	Channel& targetChannel = _server._channels[channel];
	if (targetChannel.isInviteOnly() == true) {
		client.sendToClient(ERR_INVITEONLYCHAN);
		return ;
	}

	if (targetChannel.isFull() == true) {
		client.sendToClient(ERR_CHANNELISFULL);
		return ;
	}

	if (targetChannel.isKeyProtected() == true) {
		if (_tokens.size() < 3) {
			client.sendToClient(ERR_BADCHANNELKEY);
			return ;
		}
		std::string key = _tokens[2];
		if (DEBUG)
			std::cout << "JOIN parsed key :" << key << std::endl;
		if (key != targetChannel.getKey()) {
			client.sendToClient(ERR_BADCHANNELKEY);
			return ;
		}
	}

	client.joinChannel(targetChannel);
	client.sendToClient(RPL_TOPIC);
	client.sendToClient(RPL_NAMREPLY);
	client.sendToClient(RPL_ENDOFNAMES);

	if (DEBUG) {
		std::cout << "TOPIC cmd msg :" << RPL_TOPIC << std::endl;
		std::cout << "NAMES cmd msg :" << RPL_NAMREPLY << std::endl;
	}
}

void Server::Commands::names(Client& client) {
	std::string cmd = "NAMES";
	std::string nick = client.getNickName();

	if (_tokens.size() < 2) {
		client.sendToClient(ERR_NEEDMOREPARAMS);
		return ;
	}

	std::string channel = _tokens[1];
	if (_tokens[1].find('#') == 0)
		channel.erase(0, 1);

	if (_server.channelExists(channel) == false) {
		client.sendToClient(ERR_NOSUCHCHANNEL);
		return ;
	}

	if (client.hasJoinedChannel(channel) == false) {
		client.sendToClient(ERR_NOTONCHANNEL);
		return ;
	}

	Channel& targetChannel = _server._channels[channel];

	client.sendToClient(RPL_NAMREPLY);
	client.sendToClient(RPL_ENDOFNAMES);
}

void Server::Commands::part(Client& client) {
	// Command: PART
	// Parameters: <channel>{,<channel>} [<reason>]
	std::string cmd = "PART";
	std::string nick = client.getNickName();

	if (_tokens.size() < 2) {
		client.sendToClient(ERR_NEEDMOREPARAMS);
		return ;
	}

	std::string channel = _tokens[1];
	if (_tokens[1].find('#') == 0)
		channel.erase(0, 1);

	if (_server.channelExists(channel) == false) {
		client.sendToClient(ERR_NOSUCHCHANNEL);
		return ;
	}

	if (client.hasJoinedChannel(channel) == false) {
		client.sendToClient(ERR_NOTONCHANNEL);
		return ;
	}

	Channel& targetChannel = _server._channels[channel];
	client.quitChannel(targetChannel);
}

void Server::Commands::nick(Client& client) {
	// Command: NICK
	// Parameters: <nickname>
	std::string cmd = "NICK";

	if (_tokens.size() < 2) {
		client.sendToClient(ERR_NEEDMOREPARAMS);
		return ;
	}
	
	if (_tokens.size() > 2) {
		client.sendToClient(ERR_NEEDMOREPARAMS);
		return ;
	}

	std::string nick = _tokens[1];
	if (nick.find(',') != std::string::npos) {
		client.sendToClient(ERR_ERRONEUSNICKNAME);
		return ;
	}

	if (nick.find('#') == 0 || nick.find(':') == 0 || nick.empty()) {
		client.sendToClient(ERR_ERRONEUSNICKNAME);
		return ;
	}

	if (nick.find_first_of(",") != std::string::npos) {
		client.sendToClient(ERR_ERRONEUSNICKNAME);
		return ;
	}

	if (_server.clientExists(nick) == true) {
		client.sendToClient(ERR_NICKNAMEINUSE);
		return ;
	}

	// change nick in _clientsByNicks
	std::map<std::string, Client*>::iterator it = _server._clientsByNicks.find(client.getNickName());
	Client* tmpclient = it->second;
	_server._clientsByNicks.erase(it);
	_server._clientsByNicks.insert(std::pair<std::string, Client*>(nick, tmpclient));

	// change nick in every joined channel
	for (std::map<std::string, Channel>::iterator ct = _server._channels.begin(); ct != _server._channels.end(); ct++) {
		if (client.hasJoinedChannel(ct->first)) {
			ct->second.changeNickName(client.getNickName(), nick);
		}
	}

	_server.sendToJoinedChannels(client, nick , "NICK");
	client.sendToClient(NICKCHANGESUCCESS);
	client.setNickName(nick);
}

void Server::Commands::quit(Client& client) {
	// Command: QUIT
	// Parameters: [<reason>]
	if (_tokens.size() > 1) {

		size_t len = (_input.find(':') + 1);
		std::string msg = _input.substr(len, _input.length() - len);
		
		if (DEBUG)
			std::cout << "QUIT cmd msg :" << msg << std::endl;

		_server.sendToJoinedChannels(client, msg, "QUIT");
	}
	_server.disconnectClient(client);
}

void Server::Commands::kick(Client& client) {
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
	if (channel.find('#') == 0)
		channel.erase(0, 1);

	if (client.hasJoinedChannel(channel) == false) {
		client.sendToClient(ERR_NOTONCHANNEL);
		return ;
	}

	std::string nick = _tokens[2];

	if (_server.clientExists(nick) == false) {
		client.sendToClient(ERR_NOSUCHNICK);
		return ;
	}

	Client& targetClient = *_server._clientsByNicks[nick];
	Channel& targetChannel = _server._channels[channel];

	if (DEBUG)
		std::cout << "KICK target channel name :" << targetChannel.getChannelName() << std::endl << "KICK target nick :" << targetClient.getNickName() << std::endl;
	if (targetClient.hasJoinedChannel(channel) == false) {
		client.sendToClient(ERR_USERNOTINCHANNEL);
		return ;
	}
	targetClient.quitChannel(targetChannel);
	targetClient.sendToClient(YOUWEREKICKED);
}

void Server::Commands::topic(Client& client) {
	//	Command: TOPIC
	//	Parameters: <channel> [<topic>]
	std::string cmd = "TOPIC";

	if (_tokens.size() < 2) {
		client.sendToClient(ERR_NEEDMOREPARAMS);
		return ;
	}

	std::string channel = _tokens[1];
	if (channel.find('#') == 0)
		channel.erase(0, 1);

	if (_server.channelExists(channel) == false) {
		client.sendToClient(ERR_NOSUCHCHANNEL);
		return ;
	}
	
	Channel& targetChannel = _server._channels[channel];

	if (client.hasJoinedChannel(channel) == false) {
		client.sendToClient(ERR_NOTONCHANNEL);
		return ;
	}

	if (_tokens.size() == 2) {
		client.sendToClient(RPL_TOPIC);
		return ;
	}

	if (targetChannel.getTopicRestricedStatus() == true && client.getOpStatus() != 1) {
		client.sendToClient(ERR_NOPRIVILEGES);
		return ;
	}

	if (_tokens[2].empty())
		return ;

	size_t topicStart = (_input.find(_tokens[2]) + 1);
	std::string newTopic;
	newTopic = _input.substr(topicStart, _input.length() - topicStart);
	targetChannel.setChannelTopic(newTopic);
	targetChannel.sendToChannel(RPL_TOPIC);
}

void Server::Commands::alreadyRegistered(Client& client) {
	client.sendToClient(ERR_ALREADYREGISTERED);
}

void Server::Commands::invite(Client &client) {
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
	if (channel.find('#') == 0)
		channel.erase(0, 1);

	if (_server.clientExists(nick) == false) {
		client.sendToClient(ERR_NOSUCHNICK);
		return ;
	}
	
	if (_server.channelExists(channel) == false) {
		client.sendToClient(ERR_NOSUCHCHANNEL);
		return ;
	}

	if (client.hasJoinedChannel(channel) == false) {
		client.sendToClient(ERR_NOTONCHANNEL);
		return ;
	}
	Channel& targetChannel = _server._channels[channel];

	if (targetChannel.isOnChannel(nick) == true) {
		client.sendToClient(ERR_USERONCHANNEL);	
		return ;
	}

	if (targetChannel.isFull() == true) {
		client.sendToClient(ERR_CHANNELISFULL);	
		return ;
	}

	else {
		Client& targetClient = *_server._clientsByNicks[nick];
		targetClient.sendToClient(RPL_INVITING);
		targetClient.joinChannel(targetChannel);
		targetClient.sendToClient(RPL_TOPIC);
		targetClient.sendToClient(RPL_NAMREPLY);
		targetClient.sendToClient(RPL_ENDOFNAMES);
	}
}

void Server::Commands::mode(Client& client) {
	// Command: MODE
	// Parameters: <target> [<modestring> [<mode arguments>...]]
	std::string cmd = "MODE";
	if (client.getOpStatus() == false) {
		client.sendToClient(ERR_NOPRIVILEGES);
		return ;
	}

	if (_tokens.size() < 3) {
		client.sendToClient(ERR_NEEDMOREPARAMS);
		return ;
	}

	if (_tokens[1].find('#') == 0)
		_tokens[1].erase(0, 1);

	std::string channel = _tokens[1];
	std::string nick = _tokens[1];
	std::string modestring = _tokens[2];

	if (DEBUG)
		std::cout << "modestring :" << modestring << std::endl;
	if (modestring.size() != 2 && ( modestring[0] != '+' || modestring[0] != '-')) {
		client.sendToClient(ERR_INVALIDMODEPARAM);
		return ;
	}

	if (modestring.find_first_not_of("+-itklo") != std::string::npos) {
		client.sendToClient(ERR_INVALIDMODEPARAM);
		return ;
	}

	bool targetIsChannel = true;
	if (modestring[1] == 'o')
		targetIsChannel = false;

	if (targetIsChannel == true) {
		if (_server.channelExists(channel) == false) {
				client.sendToClient(ERR_NOSUCHCHANNEL);
				return ;
			}
			if (client.hasJoinedChannel(channel) == false) {
				client.sendToClient(ERR_NOTONCHANNEL);
				return ;
			}
	}
	if (targetIsChannel == false) {
		if (_server.clientExists(nick) == false) {
			client.sendToClient(ERR_NOSUCHNICK);
			return ;
		}
		Client& targetClient = *_server._clientsByNicks[nick];
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
		_server._channels[channel].setInviteStatus(status);
	// t: Set/remove the restrictions of the TOPIC command to channel operators
	if (modestring[1] == 't')
		_server._channels[channel].setTopicRestrictedStatus(status);
	// k: Set/remove the channel key (password)
	if (modestring[1] == 'k')
		modeChannelKey(client, _server._channels[channel], status);
	// l: Set/remove the user limit to channel
	if (modestring[1] == 'l')
		modeChannelLimit(client, _server._channels[channel], status);
	// o: Give/take channel operator privilege
	if (modestring[1] == 'o')
		modeOpClient(client, status);
}

void Server::Commands::modeChannelKey(Client& client, Channel& targetChannel, bool status) {
	std::string cmd = "MODE";

	if (status == true) {
		if (_tokens.size() < 4) {
			client.sendToClient(ERR_NEEDMOREPARAMS);
			return ;
		}
		if (_tokens.size() > 4) {
			client.sendToClient(ERR_INVALIDKEY);
			return ;
		}
		size_t passBegin = _input.find(_tokens[3]);
		std::string newKey = _input.substr(passBegin, _input.length() - passBegin);
		if (DEBUG)
			std::cout << "MODE new key :" << newKey << " for channel :" << targetChannel.getChannelName() << std::endl;
		targetChannel.setKey(newKey);
		targetChannel.setKeyProtected(true);
	}
	else {
		targetChannel.setKeyProtected(false);
	}
}

void Server::Commands::modeChannelLimit(Client& client, Channel& targetChannel, bool status) {
	std::string cmd = "MODE";
	std::string channel = targetChannel.getChannelName();
	std::string value = _tokens[3];
	if (status == true) {
		if (_tokens.size() < 4) {
			client.sendToClient(ERR_NEEDMOREPARAMS);
			return ;
		}
		int newLimit;
		try {
			newLimit = stoi(_tokens[3]);
		}
		catch (std::exception &e) {
			client.sendToClient(ERR_INVALIDMODEVAL);
			return ;
		}
		newLimit = stoi(_tokens[3]);
		if (newLimit < targetChannel.getUserCount()) {
			client.sendToClient(ERR_CHANNELLIMIT);
			return ;
		}
		if (DEBUG)
			std::cout << "MODE setting limit to :" << newLimit << " for channel :" << targetChannel.getChannelName() << std::endl;
		targetChannel.setUserLimit(newLimit);
	}
	else {
		targetChannel.setUserLimit(-1);
	}
}

void Server::Commands::modeOpClient(Client& client, bool status) {
	std::string nick = _tokens[1];
	if (nick == client.getNickName()) {
		client.sendToClient(ERR_ERRONEUSNICKNAME);
		return ;
	}
	Client& targetClient = *_server._clientsByNicks[nick];
	if (status == true && targetClient.getOpStatus() == false) {
		targetClient.setOpStatus(true);
		targetClient.sendToClient(RPL_THEYREOPER);
	}
	else
		targetClient.setOpStatus(false);

	if (DEBUG) {
		if (status == true)
			std::cout << "MODE client " << nick << " is op now" << std::endl;
		else
			std::cout << "MODE client " << nick << " isn't op now" << std::endl;
	}
}
