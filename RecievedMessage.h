#pragma once

#include <vector>
#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include "Helper.h"
#include "User.h"
using namespace std;

class RecievedMessage
{
public:
	RecievedMessage(SOCKET,int);
	RecievedMessage(SOCKET, int, vector<string>);
	~RecievedMessage();
	User* getUser();
	SOCKET getSock();		
	void setUser(User*);
	int getMessageCode();
	vector<string>& getValues();
private:
	SOCKET _sock;
	User* _user;
	int _messageCode;
	vector<string> _values;
};

