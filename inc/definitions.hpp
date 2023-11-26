#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP

#define DEBUG true
#define SKIP_ID false
#define BUFFER_SIZE 1024

#define PROMPT_PASS "Provide the server password (PASS <password>)\r\n"
#define PROMPT_NICK "Provide a nickname (NICK <nickname>)\r\n"
#define PROMPT_USER "Provide user info (USER <username> <hostname> <servername> <realname>\r\n"

#define CLIENT client.getNickName()
// #define CCHAN client.getChName()

// GREETINGS
//   RPL_WELCOME 001 <client> :Welcome to the <networkname> Network, <nick>[!<user>@<host>]"
#define RPL_WELCOME "001 " + CLIENT + " :Welcome to " + _irc._serverName + " " + CLIENT + ". You're all set, you can now use LIST to list channels and JOIN <channel> to join a channel, " + CLIENT + ":)\r\n"
//   RPL_YOURHOST 002 <client> :Your host is <servername>, running version <version>"
#define RPL_YOURHOST "002 " + CLIENT + " :Your host is " + _irc._serverName + "\r\n"
//   RPL_CREATED 003 <client> :This server was created <datetime>"
#define RPL_CREATED "003 " + CLIENT + " :This server was created today\r\n"

// ERR_UNKNOWNCOMMAND (421) "<client> <command> :Unknown command"
#define ERR_UNKNOWNCOMMAND "421 " + CLIENT + " " + _tokens[0] + " :Unknown command\r\n"
// LIST
//  RPL_LIST 322"<client> <channel> <client count> :<topic>"
#define RPL_LIST "322 " + CLIENT + " "
// RPL_LISTEND 323 "<client> :End of /LIST"
#define RPL_LISTEND "323 " + CLIENT + " :End of /LIST"
//   "<client> <channel> :<topic>"

// JOIN
#define JOIN _nickName + " :" + _chName + "\r\n"
// RPL_NAMREPLY 353 "<client> <symbol> <channel> :[prefix]<nick>{ [prefix]<nick>}"
#define RPL_NAMREPLY "353 " + CLIENT + " = " + targetChannel.getChannelName() + " :" + targetChannel.getChUserNamesStr() + "\r\n"
// RPL_ENDOFNAMES 366 "<client> <channel> :End of /NAMES list"
#define RPL_ENDOFNAMES CLIENT + " " + targetChannel.getChannelName() + " :End of /NAMES list\r\n"
// RPL_TOPIC 332 <client> <channel> :<topic>"
#define RPL_TOPIC "332 " + CLIENT + " " + channel + " :" + targetChannel.getChannelTopic() + "\r\n"
// ERR_NOSUCHCHANNEL 403  "<client> <channel> :No such channel"
#define ERR_NOSUCHCHANNEL "403 " + CLIENT + " " + channel + " :No such channel\r\n"
// ERR_NOSUCHNICK 401  "<client> <nickname> :No such nick/channel"
#define ERR_NOSUCHNICK "401 " + CLIENT + " " + nick + " :No such nick/channel\r\n"

// ERR_INPUTTOOLONG (417) "<client> :Input line was too long"
#define ERR_INPUTTOOLONG "417 " + CLIENT + " :Input line was too long\r\n"

#define RPL_LUSERCLIENT "251" + "\r\n"

#define ERR_NICKNAMEINUSE "433 " + CLIENT + " :Nickanme \"" + nick + "\" is already in use\r\n"
#define ERR_PASSWDMISMATCH "464 Wrong password, try again!\r\n"

// ERR_NORECIPIENT 411" <client> :No recipient given (<command>)"
#define ERR_NORECIPIENT "411 " + CLIENT +  " :No recipient given\r\n"

// CHANNEL USERS
// ERR_USERONCHANNEL 443 "<client> <nick> <channel> :is already on channel"
#define ERR_USERONCHANNEL "443 " + CLIENT + " " + channel + " :is already on channel\r\n"
// ERR_USERNOTINCHANNEL (441) "<client> <nick> <channel> :They aren't on that channel"
#define ERR_USERNOTINCHANNEL "441 " + CLIENT + " " + nick + " " + channel + " :They aren't on that channel\r\n"
// ERR_NOTONCHANNEL (442) "<client> <channel> :You're not on that channel"
#define ERR_NOTONCHANNEL "442 " + CLIENT + " " + channel + " :You're not on that channnel\r\n"
// ERR_INVITEONLYCHAN (473) "<client> <channel> :Cannot join channel (+i)"
#define ERR_INVITEONLYCHAN "473 " + CLIENT + " " + channel + " :Cannot join invite only channel\r\n"
// RPL_INVITING (341)  "<client> <nick> <channel>"
#define RPL_INVITING "341 " + CLIENT + " " + nick + " " + channel + "\r\n"
// ERR_BADCHANNELKEY (475)  "<client> <channel> :Cannot join channel (+k)"
#define ERR_BADCHANNELKEY "475 " + CLIENT + " " + channel + " :Cannot join channel (+k)\r\n"
// ERR_CHANNELISFULL (471)  "<client> <channel> :Cannot join channel (+l)"
#define ERR_CHANNELISFULL "471 " + CLIENT + " " + channel + " :Cannot join channel (+l)\r\n"
// NICK
// ERR_NONICKNAMEGIVEN 431
#define ERR_NONICKNAMEGIVEN "431 " + CLIENT + " :No nickname given\r\n"
// ERR_ERRONEUSNICKNAME 432 "<client> <nick> :Erroneus nickname"
#define ERR_ERRONEUSNICKNAME "432 " + CLIENT + nick + " :Erroneus nickname\r\n"

// ERR_NEEDMOREPARAMS (461)  "<client> <command> :Not enough parameters"
#define ERR_NEEDMOREPARAMS "461 " + CLIENT + " " + cmd + " :Not enough parameters\r\n"
// ERR_ALREADYREGISTERED (462) "<client> :You may not reregister"
#define ERR_ALREADYREGISTERED "462 " + CLIENT + " :You may not reregister\r\n"

// OPERATOR
// ERR_NOPRIVILEGES (481)  "<client> :Permission Denied- You're not an IRC operator"
#define ERR_NOPRIVILEGES "481 " + CLIENT + " :Permission Denied- You're not an IRC operator\r\n"
// ERR_INVALIDMODEPARAM (696) "<client> <target chan/user> <mode char> <parameter> :<description>"
#define ERR_INVALIDMODEPARAM "696 " + CLIENT + " " + channel + " " + modestring + " :Invalid MODE parameter!\r\n"

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