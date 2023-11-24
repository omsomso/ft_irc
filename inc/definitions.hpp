#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP

#define PROMPT_PASS "Provide the server password (PASS <password>)\r\n"
#define PROMPT_NICK "Provide a nickname (NICK <nickname>)\r\n"
#define PROMPT_USER "Provide user info (USER <username> <hostname> <servername> <realname>\r\n"

#define CLIENT client.getNickName()
#define CCHAN client.getChName()
#define CHANNEL channel

// GREETINGS
//   RPL_WELCOME 001 <client> :Welcome to the <networkname> Network, <nick>[!<user>@<host>]"
#define RPL_WELCOME "001 " + CLIENT + " :Welcome to " + _irc._serverName + " " + CLIENT + ". You're all set, you can now use LIST to list channels and JOIN <channel> to join a channel, " + CLIENT + ":)\r\n"
//   RPL_YOURHOST 002 <client> :Your host is <servername>, running version <version>"
#define RPL_YOURHOST "002 " + CLIENT + " :Your host is " + _irc._serverName + "\r\n"
//   RPL_CREATED 003 <client> :This server was created <datetime>"
#define RPL_CREATED "003 " + CLIENT + " :This server was created today\r\n"


// LIST
//  RPL_LIST 322"<client> <channel> <client count> :<topic>"
#define RPL_LIST "322 " + CLIENT + " "
// RPL_LISTEND 323 "<client> :End of /LIST"
#define RPL_LISTEND "323 " + CLIENT + " :End of /LIST"
//   "<client> <channel> :<topic>"

// JOIN
#define JOIN CLIENT + " :" + CCHAN + "\r\n"
// RPL_NAMREPLY 353 "<client> <symbol> <channel> :[prefix]<nick>{ [prefix]<nick>}"
#define RPL_NAMREPLY "353 " + CLIENT + " = " + CCHAN + " :"
// RPL_ENDOFNAMES 366 "<client> <channel> :End of /NAMES list"
#define RPL_ENDOFNAMES CLIENT + " " + CCHAN + " :End of /NAMES list\r\n"
// RPL_TOPIC 332 <client> <channel> :<topic>"
#define RPL_TOPIC "332 " + CLIENT + " "
// ERR_NOSUCHCHANNEL 403  "<client> <channel> :No such channel"
#define ERR_NOSUCHCHANNEL "403 " + CLIENT + " " + CHANNEL + " :No such channel\r\n"
// ERR_NOSUCHNICK 401  "<client> <nickname> :No such nick/channel"
#define ERR_NOSUCHNICK "401 " + CLIENT + " " + target + " :No such nick/channel\r\n"

#define RPL_LUSERCLIENT "251" + "\r\n"

#define ERR_NICKNAMEINUSE "433 " + CLIENT + " :Nickanme \"" + nick + "\" is already in use\r\n"
#define ERR_PASSWDMISMATCH "464 Wrong password, try again!\r\n"
#define ERR_USERONCHANNEL "443 " + CLIENT + " :is already on channel\r\n"

#define ERR_CMD_PASS "263 Error : invalid PASS command! Try again\r\n"
#define ERR_CMD_NICK "263 Error : invalid NICK command! Try again\r\n"
#define ERR_CMD_USER "263 Error : invalid USER command! Try again\r\n"
#define ERR_CMD_JOIN "263 Error : invalid channel! Try again\r\n"

#define ERR_RECV "Error in received data\r\n"
#define UIDCLIENT_CLOSE "Unidentified client has closed the connection\r\n"
#define UIDCLIENT_RECV_ERR "Error : recv() from unidentified client\r\n"

#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define BLUE "\033[0;34m"
#define ORANGE "\033[33m"
#define GREY "\033[90m"
#define END	"\033[0m"

#endif