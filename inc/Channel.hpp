#ifndef CHANNEL_HPP
#define CHANNEL_HPP

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
#include <stdlib.h>

// #include "Irc.hpp"
#include "Client.hpp"
#include "definitions.hpp"

class Channel {
	private:
		std::string 			_name;
		std::string 			_topic;
		int						_id;
		std::map<int, Client>	_users;
		// std::vector<std::string> _usNames;

	public:
		Channel();
		Channel(std::string name, std::string topic, int id);

		// std::vector<std::string>	getUsNames();

		std::string getChannelName();
		std::string getChannelTopic();
		int			getChannelId();
		std::string	getNbUsers();

		void		setChannelName(std::string const name);
		void		setChannelTopic(std::string const topic);
		void 		setChannelId(int id);

		void 		addUser(Client& client);
		void 		removeUser(Client& client);
		void 		sendToUser(int fd, std::string msg);
		void 		sendToChannel(std::string msg);

};

#endif