#include "../inc/Server.hpp"

std::string Server::Commands::getBotMessage(Client& client) const {
	// std::string Greeting = ;
	std::string Greeting;
	std::string form = (":chatbot PRIVMSG " + client.getNickName() + " :");
	for (int i = 0; i < 2; i++)
		client.sendToClient("\n");
	Greeting = ("█████████████████████████████████████████████████\n");
	client.sendToClient(form + Greeting);
	Greeting =("█     __ __ __  ____ ______ ____   ___  ______  █\n");
	client.sendToClient(form + Greeting);
	Greeting = ("█    /  ]  |  |/    |      |    \\ /   \\|      | █\n");
	client.sendToClient(form + Greeting);
	Greeting = ("█   /  /|  |  |  o  |      |  o  )     |      | █\n");
	client.sendToClient(form + Greeting);
	Greeting = ("█  /  / |  _  |     |_|  |_|     |  O  |_|  |_| █\n");
	client.sendToClient(form + Greeting);
	Greeting = ("█ /   \\_|  |  |  _  | |  | |  O  |     | |  |   █\n");
	client.sendToClient(form + Greeting);
	Greeting = ("█ \\     |  |  |  |  | |  | |     |     | |  |   █\n");
	client.sendToClient(form + Greeting);
	Greeting = ("█  \\____|__|__|__|__| |__| |_____|\\___/  |__|   █\n");
	client.sendToClient(form + Greeting);
	Greeting = ("█                                               █\n");
	client.sendToClient(form + Greeting);
	Greeting = ("█████████████████████████████████████████████████\n");
	client.sendToClient(form + Greeting);
	Greeting = ("█ Usage: CHATBOT [COMMAND_NUMBER]               █\n");
	client.sendToClient(form + Greeting);
	Greeting = ("█ Available Commands:                           █\n");
	client.sendToClient(form + Greeting);
	Greeting = ("█ " "[0]"  " : List your details "  "                      █\n");
	client.sendToClient(form + Greeting);
	Greeting = ("█ "  "[1]"  " : List joined channels "  "                   █\n");
	client.sendToClient(form + Greeting);
	Greeting = ("█ "  "[2]"  " : Show online users count "  "                █\n");
	client.sendToClient(form + Greeting);
	Greeting = ("█ "  "[3]"  " : List all channels "  "                      █\n");
	client.sendToClient(form + Greeting);
	Greeting = ("█ "  "[4]"  " : Channel info "  "                           █\n");
	client.sendToClient(form + Greeting);
	Greeting = ("█ "  "[5]"  " : Server info "  "                            █\n");
	client.sendToClient(form + Greeting);
	Greeting = ("█████████████████████████████████████████████████\n");
	for (int i = 0; i < 3; i++)
		client.sendToClient("\n");
	client.sendToClient(form + Greeting);
	// Greeting = (END + "\n");
	client.sendToClient(form + Greeting);
	return Greeting;
}


std::string Server::Commands::handleChatBotRequest(const std::vector<std::string>& tokens, Client& client) {
	if (tokens.empty())
		return getBotMessage(client);

	std::string response;
	std::string form = (":chatbot PRIVMSG " + client.getNickName() + " :");

	if (tokens.size() > 1)
	{
		if (tokens[1] == "0") {
			std::string opStatusStr = client.getOpStatus() ? "true" : "false";
			// Client details
			response = "Nickname: " + client.getNickName() + "\n";
			client.sendToClient(form + response);
			response = "Username: " + client.getUserName() + "\n";
			client.sendToClient(form + response);
			response = "Hostname: " + client.getHostName() + "\n";
			client.sendToClient(form + response);
			response = "Server Name: " + client.getServerName() + "\n";
			client.sendToClient(form + response);
			response = "Real Name: " + client.getRealName() + "\n";
			client.sendToClient(form + response);
			response = "Operator status: " + opStatusStr + "\n";
			client.sendToClient(form + response);
		}
		else if (tokens[1] == "1")
		{
			// List joined channels
			std::vector<std::string> channelsJoined = client.getChannelsJoined();
			response = "Joined Channels: \n";
			client.sendToClient(form + response);
			for (std::vector<std::string>::const_iterator it = channelsJoined.begin(); it != channelsJoined.end(); ++it) {
				response = *it + "\n";
				client.sendToClient(form + response);
			}
			if (channelsJoined.empty()) {
				response = "No joined channels.\n";
				client.sendToClient(form + response);
			}
		}
		else if (tokens[1] == "2") {
			response = "Online Users: " + std::to_string(_server._clients.size() - 1) + "\r\n";
			client.sendToClient(form + response);
		}
		else if (tokens[1] == "3")
			response = listAllChannels(client);
		else if (tokens[1] == "4")
		{
			if (tokens.size() == 3)
				response = channelInfo(tokens[2], client);
			else
				response = "Usage: ChatBot 4 [CHANNEL NAME]\n";
		}
		else if (tokens[1] == "5")
			response = serverInfo(client);
		else
			response = getBotMessage(client);
	}
	else
		response = getBotMessage(client);
	
	return response + "\r\n";
}


std::string Server::Commands::serverInfo(Client& client) const {
	std::string form = (":chatbot PRIVMSG " + client.getNickName() + " :");
	std::string serverInfo("Server Name: " + this->_server._serverName + "\n");
	client.sendToClient(form + serverInfo);
	serverInfo = ("Online Users: " + std::to_string(this->_server._clients.size()) + "\n");
	client.sendToClient(form + serverInfo);
	serverInfo = ("Number of Channels: " + std::to_string(this->_server._channels.size()) + "\n");
	client.sendToClient(form + serverInfo);
	return serverInfo;
}


std::string Server::Commands::channelInfo(const std::string& channelName, Client& client) const {
	std::string form = (":chatbot PRIVMSG " + client.getNickName() + " :");
	std::map<std::string, Channel>::const_iterator it = this->_server._channels.find(channelName);
	if (it != this->_server._channels.end()) {
		const Channel& channel = it->second;
		std::string channelInfo = "Channel Name: " + channel.getChannelName() + "\n";
		client.sendToClient(form + channelInfo);
		channelInfo = "Online Users: " + channel.getNbUsers() + "\n";
		client.sendToClient(form + channelInfo);
		channelInfo = "Channel Topic: " + channel.getChannelTopic() + "\n";
		client.sendToClient(form + channelInfo);

		return channelInfo;
	}
	client.sendToClient(form + "No channel named '" + channelName + "' found.\n");
	return "No channel named '" + channelName + "' found.\n";
}

std::string fillIt(const std::string& text, size_t length) {
	if (text.length() >= length) {
		return text.substr(0, length);
	}
	return text + std::string(length - text.length(), ' ');
}

std::string Server::Commands::listAllChannels(Client& client) const {
	std::string channelList;
	std::string form = (":chatbot PRIVMSG " + client.getNickName() + " :");
	client.sendToClient(form + "\n");
	channelList = ("███████████████████████████████████████████\n");
	client.sendToClient(form + channelList);
	channelList = ("█ Channel Name █ Online Users █   Topic   █\n");
	client.sendToClient(form + channelList);
	channelList = ("███████████████████████████████████████████\n");
	client.sendToClient(form + channelList);
	std::map<std::string, Channel>::const_iterator it;
	for (it = this->_server._channels.begin(); it != this->_server._channels.end(); ++it) {
		channelList = "";
		channelList.append( "█"  + fillIt(it->first, 14));
		channelList.append( "█"  + fillIt(it->second.getNbUsers(), 15));
		channelList.append( "█"  + fillIt(it->second.getChannelTopic(), 11) +  "█\n");
		client.sendToClient(form + channelList);
	}
	channelList = ( "███████████████████████████████████████████\n" );
	client.sendToClient(form + channelList);
	client.sendToClient(form + "\n");
	return channelList;
}