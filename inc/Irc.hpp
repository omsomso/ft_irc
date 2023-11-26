#ifndef IRC_HPP
#define IRC_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <map>
#include <vector>
#include <sstream>
#include <algorithm>

// #include "IrcCommands.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "definitions.hpp"

class Irc {
	public:
		Irc(int const port, std::string const pass, std::string const serverName);
		int setupServer();
		int monitor();

	private:
		class IrcCommands {
			private:
				Irc& _irc;
				std::string _input;
				std::vector<std::string> _tokens;

			public:
				IrcCommands(Irc& irc);

				void	handleClientCmd(Client& client, std::string input);
				void	privmsg(Client& client);
				void	msgChannel(Client &client, std::string msg);
				void	msgUser(Client &client, std::string msg);
				void	list(Client& client);
				void	names(Client& client);
				void	join(Client& client);
				void	nick(Client& client);
				void	topic(Client& client);
				void	quit(Client& client);
				void	kick(Client& client);
				void	invite(Client &client);
				void	triggerOp(Client &client);
				void	mode(Client& client);
				void	modeChannelKey(Client& client, Channel& channel, bool status);
				void	modeChannelLimit(Client& client, Channel& channel, bool status);
				void	modeOpClient(Client& client, bool status);
				void	alreadyRegistered(Client& client);

		};

		class IrcClientSetup {
			private:
				Irc& _irc;
			
			public:
				IrcClientSetup(Irc& irc);

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

		IrcCommands		_cmd;
		IrcClientSetup	_setup;

		int					_serverSocket;
		struct sockaddr*	_serverAddress;
	
		std::vector<pollfd>				_fds;
		std::map<int, Client>			_clients;
		std::map<std::string, Client>	_clientsByNicks;
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
};

	// void sigAccept(void);
	// void sigHandle(int sig);

#endif