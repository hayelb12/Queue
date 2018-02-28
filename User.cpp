#include "User.h"
#include "Helper.h"
#include  "Protocol.h"

User::User(string username,SOCKET sock )
{
	_username = username;
	_sock = sock;
}

User::~User()
{
	_username = "";
	_sock = 0;
	
}

void User::send(string message)
{
	Helper::sendData(_sock, message);
}

string User::getUsername()
{
	return _username;
}

SOCKET User::getSocket()
{
	return _sock;
}
