#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <arpa/inet.h>
#include <vector>
#include <fcntl.h>
#include "../inc/Irc.hpp"

int testPort(std::string port) {
	if (!port.find_first_not_of("1234567890")) {
		std::cerr << "Error : invalid port." << std::endl;
		return 1;
	}
	try {
		(stol(port));
	}
	catch (std::exception &e) {
		std::cerr << "Error : invalid port." << std::endl;
		return 1;
	}
	long porttest = stol(port);
	if (porttest < 1024 || porttest > 65535) {
		std::cerr << "Error : invalid port." << std::endl;
		return 1;
	}
	return 0;
}

int testPass(std::string pass) {
	if (pass.empty()) {
		std::cerr << "Error : empty password." << std::endl;
		return 1;
	}
	return 0;
}

int main(int ac, char** av) {
	if (ac != 3) {
		std::cerr << "Error : usage => ./ircserv <port> <password>" << std::endl;
		return 0;
	}
	std::string port = av[1];
	std::string pass = av[2];
	
	if (testPort(port))
		return 0;
	if (testPass(pass))
		return 0;

	Irc irc(stoi(port), pass, "Super-Serveur");
	std::cout << "Starting server on port " << port << " with password " << pass << std::endl;
	if (irc.setupServer())
		return 0;
	if (irc.monitor())
		return 0;
	return 0;
}