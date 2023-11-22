#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <arpa/inet.h>
#include <vector>
#include <fcntl.h>
#include "../inc/Irc.hpp"

int main() {
	// add signals for graceful shutdown
	Irc irc;
	irc.setupServer();
	irc.monitor();
	return 0;
}