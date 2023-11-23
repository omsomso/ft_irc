#include "../inc/Channel.hpp"

Channel::Channel(std::string name, std::string topic, int id) : _name(name), _topic(topic), _id(id) {
	std::cout << "Created channel " << name << " with topic " << topic << std::endl;

}

void Channel::setChannelName(std::string name) {
	this->_name = name;
}

void Channel::setChannelTopic(std::string topic) {
	this->_topic = topic;
}

void Channel::setChannelId(int id) {
	this->_id = id;
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

void Channel::addUser(Client& client) {
	client.setChannelJoined(_id);
	_users.insert(std::pair<int, Client>(client.getFd(), client));
}

void Channel::sendToUser(int fd, std::string mess) {
	send(fd, &mess[0], strlen(&mess[0]), 0);
}

void Channel::sendToChannel(std::string mess) {
	
	for (std::map<int, Client>::iterator it = _users.begin(); it != _users.end(); it++)
		send(it->first, &mess[0], strlen(&mess[0]), 0);
}
