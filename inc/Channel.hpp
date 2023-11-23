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

// #include "../inc/Irc.hpp"
#include "../inc/Client.hpp"
#include "../inc/definitions.hpp"

class Channel {
	private:
	std::string 			_name;
	std::string 			_topic;
	int						_id;
	std::map<int, Client>	_users;

	public:
	Channel(std::string name, std::string topic, int id);
	
	void		setChannelName(std::string name);
	void		setChannelTopic(std::string topic);
	void 		setChannelId(int id);

	std::string getChannelName();
	std::string getChannelTopic();
	int			getChannelId();


	void addUser(Client& client);
	void removeUser(Client& client);
	void sendToUser(int fd, std::string mess);
	void sendToChannel(std::string mess);

};

#endif