#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "../inc/definitions.hpp"
// #include "../inc/Irc.hpp"

class Client {
	private:
	std::string _nickName;
	std::string _userName;
	std::string _hostName;
	std::string _realName;
	int			_fd;
	int			_setupStatus;
	bool		_op;
	int			_channelJoined;

	public:
	Client();
	std::string getNickName();
	std::string getUserName();
	std::string getHostName();
	std::string getRealName();
	int			getFd();
	int			getSetupStatus();
	bool		getOpStatus();
	int			getChannelJoined();

	void		setNickName(std::string nickName);
	void		setUserName(std::string userName);
	void		setHostName(std::string hostName);
	void		setRealName(std::string realName);
	void		setFd(int fd);
	void		setSetupStatus(int status);
	void		setOpStatus(bool status);
	void		setChannelJoined(int channel);

	void		printClientInfo();
};

#endif