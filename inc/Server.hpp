#ifndef SERVER_HPP
#define SERVER_HPP

#include <fcntl.h>
#include <pthread.h>

#include <iostream>
#include <sstream>
#include <fstream>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <poll.h>
#include <map>
#include <vector>
#include <algorithm>

#include "Channel.hpp"
#include "Client.hpp"
#include "definitions.hpp"


struct FileTransferData {
    std::string filename;
    std::string peerIP;
    int port;
    long long fileSize;
};

enum setupStatus {
	REGISTERED,
	USER,
	NICK,
	PASS,
};

class Server {
	public:
		Server(int const port, std::string const pass, std::string const serverName);
		int setupServer();
		int monitor();
		static bool	quitFlag;

	private:
		class Commands {
			private:
				Server& _server;
				std::string _input;
				std::vector<std::string> _tokens;

			public:
				Commands(Server& server);

				void	handleClientCmd(Client& client, std::string input);
				void	privmsg(Client& client);
				void	msgChannel(Client &client, std::string msg);
				void	msgUser(Client &client, std::string msg);
				void	list(Client& client);
				void	names(Client& client);
				void	join(Client& client);
				void	part(Client& client);
				void	nick(Client& client);
				void	topic(Client& client);
				void	quit(Client& client);
				void	kick(Client& client);
				void	invite(Client &client);
				void	triggerOp(Client &client);
				void	mode(Client& client);
				void	modeChannelKey(Client& client, Channel& targetChannel, bool status);
				void	modeChannelLimit(Client& client, Channel& targetChannel, bool status);
				void	modeOpClient(Client& client, bool status);
				void	alreadyRegistered(Client& client);
				void	handleDCC(const std::string& command, Client& client);

				// Chatbot methods
				std::string getBotMessage() const;
				std::string handleChatBotRequest(const std::vector<std::string>& tokens, Client& client);
				std::string serverInfo() const;
				std::string channelInfo(const std::string& channelName) const;
				std::string listAllChannels() const;
		};

		class ClientSetup {
			private:
				Server& _server;
			
			public:
				ClientSetup(Server& server);

				void	setupNewClients(Client& client, int fd);
				void	checkQuit(Client& client, std::string input);

				void	promptPass(Client& client);
				void	promptNick(Client& client);
				void	promptUser(Client& client);

				void	checkPass(Client& client);
				void	setupNick(Client& client);
				void	setupUser(Client& client);

				void	parsePass(Client& client, std::string input);
				int		parseNick(Client& client, std::string input);
				int		parseUser(Client& client, std::string input);

				void	printWelcome(Client& client);
		};

		int					_port;
		std::string const	_pass;
		std::string const	_serverName;

		Commands			_cmd;
		ClientSetup			_setup;

		int					_serverSocket;
		struct sockaddr*	_serverAddress;
	
		std::vector<pollfd>				_fds;
		std::map<int, Client>			_clients;
		std::map<std::string, Client*>	_clientsByNicks;
		std::map<std::string, Channel>	_channels;

		int			addClient();
		char*		fixBufferSymbols(char buffer[], size_t len);

		void		disconnectClient(Client& client);
		void		sendToAll(std::string msg);

		int			handleClient(Client& client);
		int			handleClientCmd(Client& client, std::string input);

		std::vector<std::string> tokenizeInput(std::string input, char sep);
		std::string	processInput(char buffer[], size_t len);
		bool	channelExists(std::string channel);
		bool	clientExists(std::string nick);
		void	sendToJoinedChannels(Client& client, std::string msg, std::string opt);
		void	disconnectAllClients();
		static void	signalHandler(int signal);
};

#endif