#include "../inc/Irc.hpp"

Irc::IrcCommands::IrcCommands(Irc& irc) : _irc(irc) {}

void Irc::IrcCommands::handleClientCmd(Client& client, std::string input) {
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
	else if (_tokens[0] == "DCC")
		handleDCC(input, client);

	// MODE changes channel/user settings
	else if (_tokens[0] == "MODE")
		mode(client);
	else
		client.sendToClient(ERR_UNKNOWNCOMMAND);
	return ;
}

void Irc::IrcCommands::triggerOp(Client &client) {
	if (client.getOpStatus() == false) {
		client.setOpStatus(true);
		client.sendToClient(RPL_YOUREOPER);
	}
	else
		client.setOpStatus(false);
}

void Irc::IrcCommands::privmsg(Client& client) {
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

void Irc::IrcCommands::msgChannel(Client &client, std::string msg) {
	std::string channel = _tokens[1];
	if (_tokens[1].find('#') == 0)
		channel.erase(0, 1);

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

	if (_irc.clientExists(nick) == false) {
		client.sendToClient(ERR_NOSUCHNICK);
		return ;
	}

	else {
		if (DEBUG) {
			std::cout << "PRIVMSG target user :" << nick << std::endl;
			std::cout << "PRIVMSG msg :" << msg << std::endl;
		}

		Client targetClient = *_irc._clientsByNicks[nick];

		targetClient.sendToClient(msg);
	}
}


void Irc::IrcCommands::list(Client& client) {
	std::string msg = RPL_LIST;

	for (std::map<std::string, Channel>::iterator it = _irc._channels.begin(); it != _irc._channels.end(); it++) {
		msg += (it->first + " " + it->second.getNbUsers() + " :" + it->second.getChannelTopic());
		msg += "\r\n";
		if (DEBUG)
			std::cout << "LIST cmd msg :" << msg << std::endl;
		client.sendToClient(msg);
		msg = RPL_LIST;
	}
	
	return ;
}

void Irc::IrcCommands::join(Client& client) {
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

void Irc::IrcCommands::names(Client& client) {
	std::string cmd = "NAMES";
	std::string nick = client.getNickName();

	if (_tokens.size() < 2) {
		client.sendToClient(ERR_NEEDMOREPARAMS);
		return ;
	}

	std::string channel = _tokens[1];
	if (_tokens[1].find('#') == 0)
		channel.erase(0, 1);

	if (_irc.channelExists(channel) == false) {
		client.sendToClient(ERR_NOSUCHCHANNEL);
		return ;
	}

	if (client.hasJoinedChannel(channel) == false) {
		client.sendToClient(ERR_NOTONCHANNEL);
		return ;
	}

	Channel& targetChannel = _irc._channels[channel];

	client.sendToClient(RPL_NAMREPLY);
	client.sendToClient(RPL_ENDOFNAMES);
}

void Irc::IrcCommands::part(Client& client) {
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

	if (_irc.channelExists(channel) == false) {
		client.sendToClient(ERR_NOSUCHCHANNEL);
		return ;
	}

	if (client.hasJoinedChannel(channel) == false) {
		client.sendToClient(ERR_NOTONCHANNEL);
		return ;
	}

	Channel& targetChannel = _irc._channels[channel];
	client.quitChannel(targetChannel);
}

void Irc::IrcCommands::nick(Client& client) {
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

	if (_irc.clientExists(nick) == true) {
		client.sendToClient(ERR_NICKNAMEINUSE);
		return ;
	}

	// change nick in _clientsByNicks
	std::map<std::string, Client*>::iterator it = _irc._clientsByNicks.find(client.getNickName());
	Client* tmpclient = it->second;
	_irc._clientsByNicks.erase(it);
	_irc._clientsByNicks.insert(std::pair<std::string, Client*>(nick, tmpclient));

	// change nick in every joined channel
	for (std::map<std::string, Channel>::iterator ct = _irc._channels.begin(); ct != _irc._channels.end(); ct++) {
		if (client.hasJoinedChannel(ct->first)) {
			ct->second.changeNickName(client.getNickName(), nick);
		}
	}

	_irc.sendToJoinedChannels(client, nick , "NICK");
	client.sendToClient(NICKCHANGESUCCESS);
	client.setNickName(nick);
}

void Irc::IrcCommands::quit(Client& client) {
	// Command: QUIT
	// Parameters: [<reason>]
	if (_tokens.size() > 1) {

		size_t len = (_input.find(':') + 1);
		std::string msg = _input.substr(len, _input.length() - len);
		
		if (DEBUG)
			std::cout << "QUIT cmd msg :" << msg << std::endl;

		_irc.sendToJoinedChannels(client, msg, "QUIT");
	}
	_irc.disconnectClient(client);
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
	if (channel.find('#') == 0)
		channel.erase(0, 1);

	if (client.hasJoinedChannel(channel) == false) {
		client.sendToClient(ERR_NOTONCHANNEL);
		return ;
	}

	std::string nick = _tokens[2];

	if (_irc.clientExists(nick) == false) {
		client.sendToClient(ERR_NOSUCHNICK);
		return ;
	}

	Client& targetClient = *_irc._clientsByNicks[nick];
	Channel& targetChannel = _irc._channels[channel];

	if (DEBUG)
		std::cout << "KICK target channel name :" << targetChannel.getChannelName() << std::endl << "KICK target nick :" << targetClient.getNickName() << std::endl;
	if (targetClient.hasJoinedChannel(channel) == false) {
		client.sendToClient(ERR_USERNOTINCHANNEL);
		return ;
	}
	targetClient.quitChannel(targetChannel);
	targetClient.sendToClient(YOUWEREKICKED);
}

void Irc::IrcCommands::topic(Client& client) {
	//	Command: TOPIC
	//	Parameters: <channel> [<topic>]
	std::string cmd = "TOPIC";


	if (_tokens.size() < 3) {
		client.sendToClient(ERR_NEEDMOREPARAMS);
		return ;
	}

	std::string channel = _tokens[1];
	if (channel.find('#') == 0)
		channel.erase(0, 1);

	if (_irc.channelExists(channel) == false) {
		client.sendToClient(ERR_NOSUCHCHANNEL);
		return ;
	}
	
	Channel& targetChannel = _irc._channels[channel];

	if (client.hasJoinedChannel(channel) == false) {
		client.sendToClient(ERR_NOTONCHANNEL);
		return ;
	}

	if (targetChannel.getTopicRestricedStatus() == true && client.getOpStatus() != 1) {
		client.sendToClient(ERR_NOPRIVILEGES);
		return ;
	}

	if (_tokens[2].empty())
		return ;

	size_t topicLen = _input.find(_tokens[2]) + _tokens[2].length();
	std::string newTopic; 
	newTopic = _input.substr(topicLen, _input.length() - topicLen);
	targetChannel.setChannelTopic(_tokens[2]);
	targetChannel.sendToChannel(RPL_TOPIC);
}

void Irc::IrcCommands::alreadyRegistered(Client& client) {
	client.sendToClient(ERR_ALREADYREGISTERED);
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
	if (channel.find('#') == 0)
		channel.erase(0, 1);

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
	Channel& targetChannel = _irc._channels[channel];

	if (targetChannel.isOnChannel(nick) == true) {
		client.sendToClient(ERR_USERONCHANNEL);	
		return ;
	}

	if (targetChannel.isFull() == true) {
		client.sendToClient(ERR_CHANNELISFULL);	
		return ;
	}

	else {
		Client& targetClient = *_irc._clientsByNicks[nick];
		targetClient.sendToClient(RPL_INVITING);
		targetClient.joinChannel(targetChannel);
		targetClient.sendToClient(RPL_TOPIC);
		targetClient.sendToClient(RPL_NAMREPLY);
		targetClient.sendToClient(RPL_ENDOFNAMES);
	}
}

void Irc::IrcCommands::mode(Client& client) {
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
		Client& targetClient = *_irc._clientsByNicks[nick];
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

void Irc::IrcCommands::modeChannelKey(Client& client, Channel& targetChannel, bool status) {
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

void Irc::IrcCommands::modeChannelLimit(Client& client, Channel& targetChannel, bool status) {
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

void Irc::IrcCommands::modeOpClient(Client& client, bool status) {
	std::string nick = _tokens[1];
	if (nick == client.getNickName()) {
		client.sendToClient(ERR_ERRONEUSNICKNAME);
		return ;
	}
	Client& targetClient = *_irc._clientsByNicks[nick];
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

// FILE TRANSFER STUFF
// USE /DCC SEND <path/to/file> 127.0.0.1 1096 <filesize> <recipient_nick> to SEND
// USE /DCC ACCEPT <path/to/file> 127.0.0.1 1096 <filesize> <sender_nick> to RECEIVE
void* fileSendThread(void* arg) {
    FileTransferData* data = static_cast<FileTransferData*>(arg);
    std::cout << "Sender Thread Started, waiting for receiver to connect..." << std::endl;

    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        delete data;
        return NULL;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(data->port);

    if (bind(serverSock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        close(serverSock);
        delete data;
        return NULL;
    }

    if (listen(serverSock, 1) < 0) {
        std::cerr << "Listen failed" << std::endl;
        close(serverSock);
        delete data;
        return NULL;
    }

    int clientSock = accept(serverSock, NULL, NULL);
    if (clientSock < 0) {
        std::cerr << "Accept failed" << std::endl;
        close(serverSock);
        delete data;
        return NULL;
    }
    std::cout << "Connection accepted from receiver" << std::endl;

    std::ifstream fileStream(data->filename.c_str(), std::ifstream::binary);
    if (!fileStream) {
        std::cerr << "File open failed: " << data->filename << std::endl;
        close(clientSock);
        close(serverSock);
        delete data;
        return NULL;
    }

    char buffer[1024];
    while (fileStream.read(buffer, sizeof(buffer)) || fileStream.gcount()) {
        ssize_t sentBytes = send(clientSock, buffer, fileStream.gcount(), 0);
        if (sentBytes < 0) {
            std::cerr << "Error sending data" << std::endl;
            break;
        }
        std::cout << "Sent " << sentBytes << " bytes" << std::endl;
    }

    fileStream.close();
    close(clientSock);
    close(serverSock);
    std::cout << "Sender Thread Ended" << std::endl;
    delete data;
    return NULL;
}

void* fileReceiveThread(void* arg) {
    FileTransferData* data = static_cast<FileTransferData*>(arg);
    std::string newFilename = "received_" + data->filename;
    std::cout << "Receiver Thread Started, connecting to sender..." << std::endl;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        delete data;
        return NULL;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(data->port);
    inet_pton(AF_INET, data->peerIP.c_str(), &addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        close(sock);
        delete data;
        return NULL;
    }
    std::cout << "Connected to sender" << std::endl;

    // Extract filename from the full path
    std::string fullPath(data->filename);
    size_t lastSlashPos = fullPath.find_last_of("/\\");
    std::string filename = fullPath.substr(lastSlashPos + 1);

    // Create new file path
    std::string newFilePath = "received_files/" + filename;

    // Ensure the 'received_files' directory exists
    system("mkdir -p received_files");

    std::ofstream fileStream(newFilePath.c_str(), std::ofstream::binary);
    if (!fileStream) {
        std::cerr << "File open failed: " << newFilePath << std::endl;
        return NULL;
    }

    char buffer[1024];
    int bytesRead;
    while ((bytesRead = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
        fileStream.write(buffer, bytesRead);
        std::cout << "Received " << bytesRead << " bytes" << std::endl;
    }

    fileStream.close();
    close(sock);
    std::cout << "Receiver Thread Ended" << std::endl;

    delete data;
    return NULL;
}

void Irc::IrcCommands::handleDCC(const std::string& input, Client& client) {
    std::vector<std::string> dcc_tokens = _irc.tokenizeInput(input, ' ');

    if (dcc_tokens.size() < 5 || dcc_tokens[0] != "DCC" || (dcc_tokens[1] != "SEND" && dcc_tokens[1] != "ACCEPT")) {
        client.sendToClient("Unknown or incomplete DCC command.");
        return;
    }

    // Handle DCC SEND
    if (dcc_tokens[1] == "SEND") {
        FileTransferData* data = new FileTransferData();
        data->filename = dcc_tokens[2];
        data->peerIP = dcc_tokens[3];
        data->port = atoi(dcc_tokens[4].c_str());
        data->fileSize = dcc_tokens.size() > 5 ? atoll(dcc_tokens[5].c_str()) : 0;

        pthread_t threadId;
        pthread_create(&threadId, NULL, fileSendThread, data);
        pthread_detach(threadId);
		
		// Notify the recipient
        std::string recipient = dcc_tokens.size() > 6 ? dcc_tokens[6] : "";
        if (!_irc.clientExists(recipient)) {
            client.sendToClient("Recipient not found.");
            return;
        }

        Client& recipientClient = *_irc._clientsByNicks[recipient];
        std::string notification = "Client " + client.getNickName() + " wants to send you " + data->filename + " on " + data->peerIP + " port " + std::to_string(data->port);
        recipientClient.sendToClient(notification);

        std::string response = "DCC SEND request sent to " + recipient;
        client.sendToClient(response);
    }
    // DCC ACCEPT
	if (dcc_tokens[1] == "ACCEPT") {
    FileTransferData* data = new FileTransferData();
    data->filename = dcc_tokens[2];
    data->port = atoi(dcc_tokens[4].c_str());
    // fileSize or position is not typically used in ACCEPT handling

    pthread_t threadId;
    pthread_create(&threadId, NULL, fileReceiveThread, data);
    pthread_detach(threadId);

    std::string response = "DCC ACCEPT recognized: Receiving file " + data->filename + " on port " + std::to_string(data->port);
    client.sendToClient(response);
	}
}
