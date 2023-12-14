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

#include "definitions.hpp"

class Client;

class Channel {
	private:
		std::string _name;
		std::string _topic;
		bool		_inviteOnly;
		bool		_topicRestricted;
		bool		_keyProtected;
		std::string	_key;
		int			_userLimit;
		int			_userCount;

		std::map<std::string, int>	_users;

	public:
		Channel();
		Channel(std::string name, std::string topic);
		~Channel();

		std::string getChannelName() const;
		std::string getChannelTopic() const;
		std::string	getNbUsers() const;
		std::string	getChUserNamesStr();
		bool		isInviteOnly();
		bool		getTopicRestricedStatus();
		bool		isKeyProtected();
		int			getUserLimit();
		int			getUserCount();
		std::string	getKey();
		std::map<std::string, int>&		getChUsers();

		void	setChannelName(std::string const name);
		void	setChannelTopic(std::string const topic);
		void	setInviteStatus(bool status);
		void	setTopicRestrictedStatus(bool status);
		void	setKeyProtected(bool status);
		void	setKey(std::string key);
		void	setUserLimit(int limit);
		void	incrementUserCount();
		void	decrementUserCount();
		bool	isFull();
		void	changeNickName(std::string oldNick, std::string newNick);

		void	sendToChannel(std::string msg);
		void	sendToChannelButUser(int fdExcluded, std::string msg);
		bool	isOnChannel(std::string nickName);
};

#endif