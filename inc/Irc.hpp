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

// #include "IrcCommands.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "definitions.hpp"

#define DEBUG true
#define SKIP_ID false
#define BUFFER_SIZE 1024

class Irc {
	public:
		Irc(int const port, std::string const pass, std::string const serverName);
		int setupServer();
		int monitor();

		class IrcCommands {
			private:
				Irc& _irc;

			public:
				IrcCommands(Irc& irc);

				int	handleClientCmd(Client& client, std::vector<std::string> tokens);
				int	list(Client& client);
				int	names(Client& client);
				int	join(Client& client, std::vector<std::string> tokens);
				int	nick(Client& client, std::vector<std::string> tokens);
				int topic(Client& client, std::vector<std::string> tokens);
				int quit(Client& client, std::vector<std::string> tokens);

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

				int		parseNick(Client& client, int fd, std::string input);
				int		parseUser(Client& client, int fd, std::string input);

				void	printWelcome(Client& client);
		};

	private:
		int					_port;
		std::string const	_pass;
		std::string const	_serverName;

		IrcCommands		_cmd;
		IrcClientSetup	_setup;

		int					_serverSocket;
		struct sockaddr*	_serverAddress;
	
		std::vector<pollfd>				_fds;
		std::map<int, Client>			_clients;
		std::vector<std::string>		_clNames;
		std::map<std::string, Channel>	_channels;
		std::vector<std::string>		_chNames;

		int			addClient();
		char*		fixBufferSymbols(char buffer[], size_t len);
		std::string	processInput(char buffer[], size_t len);

		void		disconnectClient(Client& client);
		void		sendToClient(int fd, std::string msg);
		void		sendToAll(std::string msg);

		int			handleClient(Client& client);
		int			handleClientCmd(Client& client, std::string input);

};

	// void sigAccept(void);
	// void sigHandle(int sig);

#endif