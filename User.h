#pragma once
using namespace std;
#include<stdlib.h>
#include<iostream>
#include<random>
#include <WinSock2.h>
#include <Windows.h>

class User
{
public:
	User(string,SOCKET);
	~User();
	void send(string);
	string getUsername();
	SOCKET getSocket();

private:
	string _username;
	SOCKET _sock;
	};

