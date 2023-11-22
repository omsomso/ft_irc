#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <arpa/inet.h>
#include <fcntl.h>

// #include "../inc/Irc.hpp"

class Client {
	private:
	std::string _nickName;
	std::string _hostName;
	std::string _realName;
	int			_id;

	public:
	std::string getNickName();
	std::string getHostName();
	std::string getRealName();
	int			getId();

	void		setNickName(std::string nickName);
	void		setHostName(std::string hostName);
	void		setRealName(std::string realName);
	void		setId(int id);

};

#endif