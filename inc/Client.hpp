#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <map>
#include <vector>

#include "Channel.hpp"
#include "definitions.hpp"

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
		std::vector<std::string> _channelsJoined;

		std::string _cmdBuffer;

	public:
		Client();
		~Client();

		int 		readClientInput();
		std::string	getCmdBuffer();
		void		clearCmdBuffer();

		void	joinChannel(Channel& channel);
		void	quitChannel(Channel& channel);
		void	sendToClient(std::string msg) const;

		std::string const getNickName() const;
		std::string const getUserName() const;
		std::string const getHostName() const;
		std::string const getServerName() const;
		std::string const getRealName() const;
		int			getFd() const;
		int			getSetupStatus() const;
		bool		getOpStatus() const;
		std::string	getChName() const;
		// Channel&	getChJoined() const;

		std::vector<std::string>	getChannelsJoined();
		bool	hasJoinedChannel(std::string channel);

		void	setNickName(std::string const nickName);
		void	setUserName(std::string const userName);
		void	setHostName(std::string const hostName);
		void	setServerName(std::string const serverName);
		void	setRealName(std::string const realName);
		void	setFd(int const fd);
		void	setSetupStatus(int const status);
		void	setOpStatus(bool const status);
		bool	sharesAChannelWith(Client& target);

		void	printClientInfo() const;
};

#endif