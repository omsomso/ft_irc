#include "../inc/Client.hpp"

Client::Client() : _setupStatus(2) {};

std::string Client::getNickName() {
	return this->_nickName;
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

void Client::setNickName(std::string nickName) {
	this->_nickName = nickName;
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