#pragma once
#include <iostream>
#include <thread>
#include <map>
#include <queue>
#include "RecievedMessage.h"
#include "DBUser.h"
#include <mutex>
#include "Helper.h"
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <condition_variable>
#include <exception>
using namespace std;


class Server
{
public:
	Server();
	~Server();
	
	void server();
	

private:
	map<SOCKET, User*> _connectedUsers;
	queue<RecievedMessage*> _queRcvMessage;

	vector<DBUser> _database;

	SOCKET _socket;
	//DataBase _db;
	mutex _mtxRecievedMessages;

	std::condition_variable _msgQueueCondition;

	// Wake up when an action has been finished.
	std::condition_variable _edited;

	void bindAndListen();
	void clientHandler(SOCKET);
	void acceptClient();
	void safeDeleteUser(RecievedMessage*);

	User* getUserByName(string);
	User* getUserBySocket(SOCKET);

	User* handleSignin(RecievedMessage*);
	bool handleSignUp(RecievedMessage*);
	void handleSignOut(RecievedMessage*);


	RecievedMessage* buildRecievedMessage(SOCKET, int);
	void addRecievedMessage(RecievedMessage*);
	void handleRecievedMessages();

	bool checkUserAndPass(string, string);
	bool isUserExist(string);
	void addUser(string, string);

	string* disect(string);
};