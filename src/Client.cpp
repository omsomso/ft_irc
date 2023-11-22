#include "../inc/Client.hpp"
	
std::string Client::getNickName() {
	return this->_nickName;
}

std::string Client::getHostName() {
	return this->_hostName;
}

std::string Client::getRealName() {
	return this->_realName;
}

int			Client::getId() {
	return this->_id;
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

void Client::setId(int id) {
		this->_id = id;
}
