#include "RecievedMessage.h"


RecievedMessage::RecievedMessage(SOCKET sock, int message, vector<string> values)
{
	_sock = sock;
	_messageCode = message;
	_values = values;
}

RecievedMessage::RecievedMessage(SOCKET sock, int message)
{
	_sock = sock;
	_messageCode = message;
}

RecievedMessage::~RecievedMessage()
{
}

User* RecievedMessage::getUser()
{
	return _user;
}

SOCKET RecievedMessage::getSock()
{
	return _sock;
}

void RecievedMessage::setUser(User* user)
{
	_user = user;
}

int RecievedMessage::getMessageCode()
{
	return _messageCode;
}

vector<string>& RecievedMessage::getValues()
{
	return _values;
}