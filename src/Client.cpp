#include "../inc/Client.hpp"

Client::Client() : _chName(""), _setupStatus(3), _op(false), _chJoined(nullptr) {}

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

std::string Client::getChName() const {
	return this->_chName;
}

Channel& Client::getChJoined() const {
	return *this->_chJoined;
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

void Client::setChName(std::string const chname) {
	this->_chName = chname;
}

void Client::setChJoined(Channel* channel) {
	this->_chJoined = channel;
}

void Client::printClientInfo() const {
	std::cout << GREY "Client fd\t: " << _fd << std::endl;
	std::cout << "Nickname\t: " << _nickName << std::endl;
	std::cout << "Username\t: " << _userName << std::endl;
	std::cout << "Hostname\t: " << _hostName << std::endl;
	std::cout << "Real name\t: " << _realName << std::endl;
	std::cout << "Setup status\t: " << _setupStatus << std::endl;
	std::cout << "Operator status\t: " << _op << std::endl;
	// std::cout << "Channel joined\t: " << _channelJoined << END << std::endl;
	std::cout << END;
}
