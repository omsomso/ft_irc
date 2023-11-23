#include "../inc/Client.hpp"

Client::Client() : _setupStatus(3), _op(false), _channelJoined(-1) {};

std::string Client::getNickName() {
	return this->_nickName;
}

std::string Client::getUserName() {
	return this->_userName;
}

std::string Client::getHostName() {
	return this->_hostName;
}

std::string Client::getRealName() {
	return this->_realName;
}

int Client::getFd() {
	return this->_fd;
}

int Client::getSetupStatus() {
	return this->_setupStatus;
}

bool Client::getOpStatus() {
	return this->_op;
}

int Client::getChannelJoined() {
	return this->_channelJoined;
}

void Client::setNickName(std::string nickName) {
	this->_nickName = nickName;
}

void Client::setUserName(std::string userName) {
	this->_userName = userName;
}

void Client::setHostName(std::string hostName) {
	this->_hostName = hostName;
}

void Client::setRealName(std::string realName) {
	this->_realName = realName;
}

void Client::setFd(int fd) {
	this->_fd = fd;
}

void Client::setSetupStatus(int status) {
	this->_setupStatus = status;
}

void Client::setOpStatus(bool status) {
	this->_op = status;
}

void Client::setChannelJoined(int channel) {
	this->_channelJoined = channel;
}

void Client::printClientInfo() {
	std::cout << GREY "Client fd\t: " << _fd << std::endl;
	std::cout << "Nickname\t: " << _nickName << std::endl;
	std::cout << "Username\t: " << _userName << std::endl;
	std::cout << "Hostname\t: " << _hostName << std::endl;
	std::cout << "Real name\t: " << _realName << std::endl;
	std::cout << "Setup status\t: " << _setupStatus << std::endl;
	std::cout << "Operator status\t: " << _op << std::endl;
	std::cout << "Channel joined\t: " << _channelJoined << END << std::endl;
}
