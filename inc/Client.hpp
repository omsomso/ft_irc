#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <arpa/inet.h>
#include <fcntl.h>

// #include "Channel.hpp"
#include "definitions.hpp"
// #include "Irc.hpp"

class Channel;

class Client {
	private:
		std::string _nickName;
		std::string _userName;
		std::string _hostName;
		std::string _serverName;
		std::string _realName;
		std::string	_chName;
		int			_fd;
		int			_setupStatus;
		bool		_op;
		Channel*	_chJoined;

	public:
		Client();
		std::string const getNickName() const;
		std::string const getUserName() const;
		std::string const getHostName() const;
		std::string const getServerName() const;
		std::string const getRealName() const;
		int			getFd() const;
		int			getSetupStatus() const;
		bool		getOpStatus() const;
		std::string	getChName() const;
		Channel&	getChJoined() const;

		void		setNickName(std::string const nickName);
		void		setUserName(std::string const userName);
		void		setHostName(std::string const hostName);
		void		setServerName(std::string const serverName);
		void		setRealName(std::string const realName);
		void		setFd(int const fd);
		void		setSetupStatus(int const status);
		void		setOpStatus(bool const status);
		void		setChName(std::string const chname);
		void		setChJoined(Channel* channel);

		void		printClientInfo() const;
};

#endif