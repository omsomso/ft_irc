#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP

#define NICK_REQUEST "Provide a nickname with the NICK command (NICK <nickname>)\n"
#define USER_REQUEST "Provide user info with the USER command (USER <username> <hostname> <servername> :<realname>\n"
#define NICK_ERROR "Error : invalid NICK command! Try again.\n"
#define USER_ERROR "Error : invalid USER command! Try again.\n"
#define PASS_PROMPT "Please provide the server password (PASS <password>):\n"
#define WRONG_PASS "Wrong password, try again!\n"
#define RECV_ERR "Error in received data\n"

#define UIDCLIENT_CLOSE "Unidentified client has closed the connection\n"
#define UIDCLIENT_RECV_ERR "Error : recv() from unidentified client\n"

#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define BLUE "\033[0;34m"
#define ORANGE "\033[33m"
#define GREY "\033[90m"
#define END	"\033[0m"

#endif