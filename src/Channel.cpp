#include "../inc/Channel.hpp"

Channel::Channel() {}

Channel::Channel(std::string name, std::string topic, int id) : _name(name), _topic(topic), _id(id) {
	std::cout << "Created channel " << name << " with topic " << topic << std::endl;
}

std::string Channel::getChannelName() {
	return this->_name;
}

std::string Channel::getChannelTopic() {
	return this->_topic;
}

int Channel::getChannelId() {
	return this->_id;
}

std::string Channel::getUserNamesStr() {
	std::string users;
	for (std::map<int, Client>::iterator it = _users.begin(); it != _users.end(); it++)
		users += ("+" + it->second.getNickName() + " ");
	users.substr(0, users.length() - 1);
	return users;
}

std::vector<std::string> Channel::getUserNamesVec() {
	std::vector<std::string> users;
	for (std::map<int, Client>::iterator it = _users.begin(); it != _users.end(); it++)
		users.push_back(it->second.getNickName());
	return users;
}
// std::vector<std::string> Channel::getUsNames() {
// 	return this->_usNames;
// }

std::string Channel::getNbUsers() {
	return (std::to_string(this->_users.size()));
}

void Channel::setChannelName(std::string const name) {
	this->_name = name;
}

void Channel::setChannelTopic(std::string const topic) {
	this->_topic = topic;
}

void Channel::setChannelId(int id) {
	this->_id = id;
}

void Channel::addUser(Client& client) {
	client.setChName(this->_name);
	client.setChJoined(this);
	_users.insert(std::pair<int, Client>(client.getFd(), client));
	// _usNames.push_back(client.getNickName());
	std::string msg = client.getNickName() + " joined the channel\n";
	sendToChannel(msg);
	std::cout << client.getNickName() << " joined channel " << _name << std::endl;
}

void Channel::removeUser(Client& client) {
	std::string msg = client.getNickName() + " left the channel\n";
	sendToChannel(msg);
	client.setChName("");
	client.setChJoined(nullptr);
	_users.erase(client.getFd());
	// _usNames.erase(client.getNickName());
}

void Channel::sendToUser(int fd, std::string msg) {
	send(fd, &msg[0], strlen(&msg[0]), 0);
}

void Channel::sendToChannel(std::string msg) {
	for (std::map<int, Client>::iterator it = _users.begin(); it != _users.end(); it++)
		send(it->first, &msg[0], strlen(&msg[0]), 0);
}

