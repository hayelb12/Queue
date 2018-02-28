#include "DBUser.h"

#include <iostream>
#include <stdlib.h>

DBUser::DBUser(string username, string pass)
{
	this->setPass(pass);
	this->setUsername(username);
}

DBUser::DBUser()
{
	setPass("");
	setUsername("");
}


DBUser::~DBUser()
{
	this->_pass = "";
	this->_username = "";
}

string DBUser::getPass()
{
	return this->_pass;
}

string DBUser::getUsername()
{
	return this->_username;
}

void DBUser::setPass(string pass)
{
	_pass = pass;
}

void DBUser::setUsername(string username)
{
	this->_username = username;
}