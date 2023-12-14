#include "../inc/Server.hpp"

std::string Server::Commands::getBotMessage() const {
    std::string Greeting = BLUE;
    Greeting.append("\n\n█████████████████████████████████████████████████\n");
    Greeting.append("█     __ __ __  ____ ______ ____   ___  ______  █\n");  // Aligned
    Greeting.append("█    /  ]  |  |/    |      |    \\ /   \\|      | █\n");  // Corrected with double backslashes
    Greeting.append("█   /  /|  |  |  o  |      |  o  )     |      | █\n");  // Aligned
    Greeting.append("█  /  / |  _  |     |_|  |_|     |  O  |_|  |_| █\n");  // Aligned
    Greeting.append("█ /   \\_|  |  |  _  | |  | |  O  |     | |  |   █\n");  // Corrected with double backslashes
    Greeting.append("█ \\     |  |  |  |  | |  | |     |     | |  |   █\n");  // Corrected with double backslashes
    Greeting.append("█  \\____|__|__|__|__| |__| |_____|\\___/  |__|   █\n");  // Corrected with double backslashes
    Greeting.append("█                                               █\n");  // Aligned
    Greeting.append("█████████████████████████████████████████████████\n");
    Greeting.append(BLUE "█ Usage: CHATBOT [COMMAND_NUMBER]               █\n");
    Greeting.append(BLUE "█ Available Commands:                           █\n");
    Greeting.append(BLUE "█ " CYAN "[0]" END " : List your details " BLUE "                      █\n");
    Greeting.append(BLUE "█ " CYAN "[1]" END " : List joined channels " BLUE "                   █\n");
    Greeting.append(BLUE "█ " CYAN "[2]" END " : Show online users count " BLUE "                █\n");
    Greeting.append(BLUE "█ " CYAN "[3]" END " : List all channels " BLUE "                      █\n");
    Greeting.append(BLUE "█ " CYAN "[4]" END " : Channel info " BLUE "                           █\n");
    Greeting.append(BLUE "█ " CYAN "[5]" END " : Server info " BLUE "                            █\n");
    Greeting.append(BLUE "█████████████████████████████████████████████████\n\n\n\n");
    Greeting.append(END);
    return Greeting;
}


std::string Server::Commands::handleChatBotRequest(const std::vector<std::string>& tokens, Client& client) {
    if (tokens.empty()) {
        return getBotMessage();
    }

    std::string response;

    if (tokens.size() > 1) {
        if (tokens[1] == "0") {
            // Client details
            response = "Nickname: " + client.getNickName() + "\n" +
                       "Username: " + client.getUserName() + "\n" +
                       "Hostname: " + client.getHostName() + "\n" +
                       "Server Name: " + client.getServerName() + "\n" +
                       "Real Name: " + client.getRealName() + "\n";
        } else if (tokens[1] == "1") {
            // List joined channels
            std::vector<std::string> channelsJoined = client.getChannelsJoined();
            response = "Joined Channels: \n";
            for (std::vector<std::string>::const_iterator it = channelsJoined.begin(); it != channelsJoined.end(); ++it) {
                response += *it + "\n";
            }
            if (channelsJoined.empty()) {
                response += "No joined channels.\n";
            }
        } else if (tokens[1] == "2") {
            response = "Online Users: " + std::to_string(_server._clients.size()) + "\n"; 
        } else if (tokens[1] == "3") {
            response = listAllChannels();
        } else if (tokens[1] == "4") {
            if (tokens.size() == 3) {
                response = channelInfo(tokens[2]);
            } else {
                response = "Usage: ChatBot 4 [CHANNEL NAME]\n";
            }
        } else if (tokens[1] == "5") {
            response = serverInfo();
        } else {
            response = getBotMessage();
        }
    } else {
        response = getBotMessage();
    }

    return response;
}


std::string Server::Commands::serverInfo() const {
    std::string serverInfo("Server Name: " + this->_server._serverName + "\n"); // Adjust _serverName based on actual attribute
    serverInfo.append("Online Users: " + std::to_string(this->_server._clients.size()) + "\n"); // Assuming _clients map stores all connected clients
    // serverInfo.append("Max Online Users: " + std::to_string(this->_maxOnlineCount) + "\n"); // Update if max online users count is tracked
    serverInfo.append("Number of Channels: " + std::to_string(this->_server._channels.size()) + "\n"); // Update based on _channels map
    return serverInfo;
}


std::string Server::Commands::channelInfo(const std::string& channelName) const {
    std::map<std::string, Channel>::const_iterator it = this->_server._channels.find(channelName);
    if (it != this->_server._channels.end()) {
        const Channel& channel = it->second;
        std::string channelInfo = "Channel Name: " + channel.getChannelName() + "\n";
        channelInfo += "Online Users: " + channel.getNbUsers() + "\n";
        channelInfo += "Channel Topic: " + channel.getChannelTopic() + "\n";
        // Add more details as needed
        return channelInfo;
    }
    return "No channel named '" + channelName + "' found.\n";
}

std::string fillIt(const std::string& text, size_t length) {
    if (text.length() >= length) {
        return text.substr(0, length); // Cut the string if it's too long
    }
    return text + std::string(length - text.length(), ' '); // Pad with spaces
}

std::string Server::Commands::listAllChannels() const {
    std::string channelList = YELLOW;
    channelList.append("███████████████████████████████████████████\n");
    channelList.append("█ Channel Name █ Online Users █   Topic   █\n");
    channelList.append("███████████████████████████████████████████\n");
    std::map<std::string, Channel>::const_iterator it;
    for (it = this->_server._channels.begin(); it != this->_server._channels.end(); ++it) {
        channelList.append(YELLOW "█" END + fillIt(it->first, 13));
        channelList.append(YELLOW "█" END + fillIt(it->second.getNbUsers(), 15));
        channelList.append(YELLOW "█" END + fillIt(it->second.getChannelTopic(), 11) + YELLOW "█\n");
    }
    channelList.append(YELLOW "███████████████████████████████████████████\n" END);
    return channelList;
}