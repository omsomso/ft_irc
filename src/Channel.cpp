#include "../inc/Channel.hpp"

Channel::Channel() {}

Channel::Channel(std::string name, std::string topic) : _name(name), _topic(topic), _inviteOnly(0), _topicRestricted(0), _keyProtected(0), _key("password"), _userLimit(-1) {
	std::cout << "Created channel " << name << " with topic " << topic << std::endl;
}

Channel::~Channel() {}

std::string Channel::getChannelName() {
	(void) _userLimit;
	return this->_name;
}

std::string Channel::getChannelTopic() {
	return this->_topic;
}

std::string Channel::getChUserNamesStr() {
	std::string userNames = "";
	for (std::map<std::string, int>::iterator it = _users.begin(); it != _users.end(); it++)
		userNames += ("+" + it->first + " ");
	userNames.substr(0, userNames.length() - 1);
	return userNames;
}

std::vector<std::string> Channel::getChUserNamesVec() {
	std::vector<std::string> userNames;
	for (std::map<std::string, int>::iterator it = _users.begin(); it != _users.end(); it++)
		userNames.push_back("+" + it->first + " ");
	return userNames;
}

std::string Channel::getNbUsers() {
	return (std::to_string(this->_users.size()));
}

std::map<std::string, int>& Channel::getChUsers() {
	return this->_users;
}

bool Channel::isInviteOnly() {
	return this->_inviteOnly;
}

bool Channel::getTopicRestricedStatus() {
	return this->_topicRestricted;
}

bool Channel::isKeyProtected() {
	return this->_keyProtected;
}

int Channel::getUserLimit() {
	return this->_userLimit;
}

int Channel::getUserCount() {
	return this->_userCount;
}

std::string Channel::getKey() {
	return this->_key;
}

void Channel::setChannelName(std::string const name) {
	this->_name = name;
}

void Channel::setChannelTopic(std::string const topic) {
	this->_topic = topic;
}

void Channel::setInviteStatus(bool status) {
	this->_inviteOnly = status;
}

void Channel::setTopicRestrictedStatus(bool status) {
	this->_topicRestricted = status;
}

void Channel::setKeyProtected(bool status) {
	this->_keyProtected = status;
}

void Channel::setKey(std::string key) {
	this->_key = key;
}

void Channel::setUserLimit(int limit) {
	this->_userLimit = limit;
}

void Channel::incrementUserCount() {
	this->_userCount++;
}

void Channel::decrementUserCount() {
	this->_userCount--;
}

bool Channel::isFull() {
	if (_userLimit > 0 && _userCount >= _userLimit)
		return true;
	else
		return false;
}

void Channel::sendToChannel(std::string msg) {
	for (std::map<std::string, int>::iterator it = _users.begin(); it != _users.end(); it++)
		send(it->second, &msg[0], strlen(&msg[0]), 0);
}

void Channel::sendToChannelButUser(int fdExcluded, std::string msg) {
	for (std::map<std::string, int>::iterator it = _users.begin(); it != _users.end(); it++) {
		if (it->second != fdExcluded)
			send(it->second, &msg[0], strlen(&msg[0]), 0);
	}
}

int Channel::getUserFdFromNick(std::string nickName) {
	std::map<std::string, int>::iterator it = _users.begin();
	while (it != _users.end()) {
		if (it->first == nickName)
			break ;
		it++;
	}
	return it->second;
}

bool	Channel::isOnChannel(std::string nickName) {
	for (std::map<std::string, int>::iterator it = _users.begin(); it != _users.end(); it++) {
		if (it->first == nickName)
			return true;
	}
	return false;
}