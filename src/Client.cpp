#include "../inc/Client.hpp"

Client::Client() : _setupStatus(3), _op(false) {}

Client::~Client() {}

int Client::readClientInput() {
	char tmpBuffer[BUFFER_SIZE];
	size_t len = recv(_fd, tmpBuffer, sizeof(tmpBuffer), 0);
	
	if (len >= BUFFER_SIZE) {
		clearCmdBuffer();
		sendToClient(ERR_INPUTTOOLONG);
		return 10;
	}

	if (len == 0) {
		return 0;
	}

	else if (len < 0) {
		std::cerr << "Error : recv() from client " << _nickName << std::endl;
		sendToClient(ERR_RECV);
		return -1;
	}

	else if (len > 0) {
		_cmdBuffer += tmpBuffer;
		if (_cmdBuffer.find("\n") != std::string::npos) {
			if (_cmdBuffer.size() >= BUFFER_SIZE) {
				clearCmdBuffer();
				sendToClient(ERR_INPUTTOOLONG);
				return 10;
			}
			return 1;
		}
	}
	return 10;
}

std::string	Client::getCmdBuffer() {
	return this->_cmdBuffer;
}

void Client::clearCmdBuffer() {
	this->_cmdBuffer = "";
}

void Client::sendChannelMembersToNewUser(Channel& joinedChannel) {
	std::map<std::string, int> chUsers = joinedChannel.getChUsers();
	for (std::map<std::string, int>::iterator it = chUsers.begin(); it != chUsers.end(); it++) {
		if (it->first != _nickName) {
			sendToClient(JOINEXISTINGUSERS);
		}
	}
}

void Client::joinChannel(Channel& channel) {
	channel.getChUsers().insert(std::pair<std::string, int>(_nickName, _fd));
	_channelsJoined.push_back(channel.getChannelName());
	channel.incrementUserCount();
	channel.sendToChannel(JOIN);
	sendChannelMembersToNewUser(channel);
	if (DEBUG) {
		std::cout << "JOIN cmd msg :" << JOIN << std::endl;
	}
}

void Client::quitChannel(Channel& channel) {
	channel.sendToChannel(PART);
	channel.decrementUserCount();
	_channelsJoined.erase(std::remove(_channelsJoined.begin(), _channelsJoined.end(), channel.getChannelName()));
	channel.getChUsers().erase(_nickName);
}

void Client::sendToClient(std::string msg) const {
	send(_fd, &msg[0], strlen(&msg[0]), 0);
}

std::string const Client::getNickName() const {
	return this->_nickName;
}

std::string const Client::getUserName() const {
	return this->_userName;
}

std::string const Client::getHostName() const {
	return this->_hostName;
}

std::string const Client::getServerName() const {
	return this->_serverName;
}

std::string const Client::getRealName() const {
	return this->_realName;
}

int Client::getFd() const {
	return this->_fd;
}

int Client::getSetupStatus() const {
	return this->_setupStatus;
}

bool Client::getOpStatus() const {
	return this->_op;
}

std::vector<std::string> Client::getChannelsJoined() {
	return this->_channelsJoined;
}

bool Client::hasJoinedChannel(std::string channel) {
	for (size_t i = 0; i < _channelsJoined.size(); i++) {
		if (_channelsJoined[i] == channel) {
			return true;
		}
	}
	return false;
}

void Client::setNickName(std::string const nickName) {
	this->_nickName = nickName;
}

void Client::setUserName(std::string const userName) {
	this->_userName = userName;
}

void Client::setHostName(std::string const hostName) {
	this->_hostName = hostName;
}

void Client::setServerName(std::string const serverName) {
	this->_serverName = serverName;
}

void Client::setRealName(std::string const realName) {
	this->_realName = realName;
}

void Client::setFd(int const fd) {
	this->_fd = fd;
}

void Client::setSetupStatus(int const status) {
	this->_setupStatus = status;
}

void Client::setOpStatus(bool const status) {
	this->_op = status;
}

bool Client::sharesAChannelWith(Client& target) {
	std::vector<std::string> vec1 = target.getChannelsJoined();
	std::vector<std::string> vec2 = getChannelsJoined();
	if (std::find_first_of(vec1.begin(), vec1.end(), vec2.begin(), vec2.end()) != vec1.end())
		return true;
	else
		return false;
}

void Client::printClientInfo() const {
	std::cout << GREY "Client fd\t: " << _fd << std::endl;
	std::cout << "Nickname\t: " << _nickName << std::endl;
	std::cout << "Username\t: " << _userName << std::endl;
	std::cout << "Hostname\t: " << _hostName << std::endl;
	std::cout << "Real name\t: " << _realName << std::endl;
	std::cout << "Setup status\t: " << _setupStatus << std::endl;
	std::cout << "Operator status\t: " << _op << std::endl;
	std::cout << END;
}
