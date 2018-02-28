#include "Server.h"
#include "Validator.h"
#include "Protocol.h"
using namespace std;

#define MESSAGE_CODE_LENGTH 3
#define USER_BYTE_LENGTH 2
#define PASS_BYTE_LENGTH 2

//done
Server::Server()
{
	
	//_db = DataBase();
	

	//creating a socket
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_socket== INVALID_SOCKET)
		throw std::exception(__FUNCTION__ " - socket");
}

//done
Server::~Server()
{
	this->_connectedUsers.clear();
	try
	{
		// the only use of the destructor should be for freeing 
		// resources that was allocated in the constructor
		::closesocket(_socket);
	}
	catch (...) {}
}

//done ,also not a C'tor
void Server::server()
{
	this->bindAndListen();

	// create new thread for handling message
	std::thread tr(&Server::clientHandler, _socket);
	tr.detach();

	while (true)
	{
		// the main thread is only accepting clients 
		// and add then to the list of handlers
		TRACE("accepting client...");
		acceptClient();
	}
}

//done
void Server::bindAndListen()
{
	struct sockaddr_in sa = { 0 };
	sa.sin_port = htons(PORT);
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = IFACE;
	// again stepping out to the global namespace
	if (::bind(_socket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - bind");
	TRACE("binded");

	if (::listen(_socket, SOMAXCONN) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - listen");
	TRACE("listening...");
}

//done
void Server::acceptClient()
{
	SOCKET client_socket = accept(_socket, NULL, NULL);
	if (client_socket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__);

	TRACE("Client accepted !");
	// create new thread for client	and detach from it
	std::thread tr(&Server::clientHandler, this, client_socket);
	tr.detach();
}

//done
void Server::clientHandler(SOCKET client_socket)
{
	RecievedMessage* currRcvMsg = nullptr;
	try
	{
		// get the first message code
		int msgCode = Helper::getMessageTypeCode(client_socket);

		while (msgCode != 0 && msgCode!= CLIENT_APP_EXIT)
		{
			currRcvMsg = buildRecievedMessage(client_socket, msgCode);
			addRecievedMessage(currRcvMsg);

			msgCode = Helper::getMessageTypeCode(client_socket);
		}

		currRcvMsg = buildRecievedMessage(client_socket, msgCode);
		addRecievedMessage(currRcvMsg);

	}
	catch (const std::exception& e)
	{
		std::cout << "Exception was catch in function clientHandler. socket=" << client_socket << ", what=" << e.what() << std::endl;
		currRcvMsg = this->buildRecievedMessage(client_socket, CLIENT_APP_EXIT);
		addRecievedMessage(currRcvMsg);
	}
	closesocket(client_socket);
}

//done
User* Server::getUserByName(string username)
{
	map<SOCKET, User*>::iterator it;
	for (it = this->_connectedUsers.begin(); it != _connectedUsers.end(); ++it)
	{
		if (it->second->getUsername() == username)
			return it->second;
	}
	return nullptr;
}

//done
User* Server::getUserBySocket(SOCKET client_socket)
{
	map<SOCKET, User*>::iterator it;
	for (it = this->_connectedUsers.begin(); it != _connectedUsers.end(); ++it)
	{
		if (it->second->getSocket() == client_socket)
			return it->second;
	}
	return nullptr;
}

//done
void Server::handleRecievedMessages()
{
	int msgCode = 0;
	SOCKET clientSock = 0;
	string userName;
	while (true)
	{
		try
		{
			unique_lock<mutex> lck(_mtxRecievedMessages);

			// Wait for clients to enter the queue.
			if (_queRcvMessage.empty())
				_msgQueueCondition.wait(lck);

			// in case the queue is empty.
			if (_queRcvMessage.empty())
				continue;

			RecievedMessage* currMessage = _queRcvMessage.front();
			_queRcvMessage.pop();
			lck.unlock();

			currMessage->setUser(getUserBySocket(clientSock));

			if (msgCode == CLIENT_SIGN_IN)
			{
				handleSignin(currMessage);
			}

			else if (msgCode == CLIENT_SIGN_OUT)
			{
				safeDeleteUser(currMessage);
			}
			else if (msgCode == CLIENT_SIGN_UP)
			{
				handleSignUp(currMessage);
			}
			else if (msgCode == CLIENT_APP_EXIT)
			{
				safeDeleteUser(currMessage);
			}
			else
			{
				safeDeleteUser(currMessage);
			}

			delete currMessage;
		}
		catch (...)
		{
			RecievedMessage* currMessage = _queRcvMessage.front();
			_queRcvMessage.pop();

			currMessage->setUser(getUserBySocket(clientSock));
			safeDeleteUser(currMessage);
		}
	}
}

//done
void Server::safeDeleteUser(RecievedMessage* msg)
{
	SOCKET s = msg->getSock();
	this->handleSignOut(msg);
	try
	{
		::closesocket(s);
	}
	catch (...){}

}

//done
User* Server::handleSignin(RecievedMessage* msg)
{
	if (!checkUserAndPass(msg->getValues()[0],msg->getValues()[1]))
	{
		//send failure
		string data = "" + SERVER_SIGN_IN_WRONG_DETAILS; 
		Helper::sendData(msg->getSock(), data);
		return nullptr;
	}
	if (this->getUserByName(msg->getValues()[0]) != nullptr)//the user is already connected
	{
		//send failure
		string data = "" + SERVER_SIGN_IN_ALREADY_CONNECTED;
		Helper::sendData(msg->getSock(), data);
		return nullptr;
	}
	User* user = new User(msg->getValues()[0], msg->getSock());
	string data = "" + SERVER_SIGN_IN_SUCCESS;
	user->send(data);
	return user;

} 

//done
RecievedMessage* Server::buildRecievedMessage(SOCKET client_socket, int msgCode)
{
	unique_lock<mutex> lck(_mtxRecievedMessages);

	vector<string> values;
	if (msgCode == CLIENT_SIGN_IN)
	{
		string* vals = disect(Helper::getStringPartFromSocket(client_socket, 47));
		values.push_back(vals[0]);
		values.push_back(vals[1]);

	}
	else if (msgCode == CLIENT_SIGN_OUT)
	{
		
	}
	else if (msgCode == CLIENT_SIGN_UP)
	{
		string* vals = disect(Helper::getStringPartFromSocket(client_socket, 47));
		values.push_back(vals[0]);
		values.push_back(vals[1]);

	}


	RecievedMessage* msg = new RecievedMessage(client_socket, msgCode, values);
	return msg;
}

//done
void Server::addRecievedMessage(RecievedMessage* msg)
{
	unique_lock<mutex> lck(_mtxRecievedMessages);

	this->_queRcvMessage.push(msg);
	lck.unlock();
	_msgQueueCondition.notify_all();
}   

//done
void Server::handleSignOut(RecievedMessage* msg)
{
	if (msg->getUser() != nullptr)
	{
		User* user = msg->getUser();
		this->_connectedUsers.erase(find(_connectedUsers.begin(), _connectedUsers.end(), user));
	}
}

//done
bool Server::handleSignUp(RecievedMessage* msg)
{
	vector<string> val = msg->getValues();
	if (Validator::isPasswordValid(val[1]))
	{
		string data = SERVER_SIGN_UP_PASS_ILLEGAL + "";
		msg->getUser()->send(data);
		return false;
	}
	else if (Validator::isUsernameValid(val[0]))
	{
		string data = SERVER_SIGN_UP_USER_ILLEGAL + "";
		msg->getUser()->send(data);
		return false;
	}
	else if (this->isUserExist(val[0]))
	{
		string data = SERVER_SIGN_UP_USER_EXISTS + "";
		getUserBySocket(msg->getSock())->send(data);
		return false;
	}
	else
	{
		addUser(val[0], val[1]);
		string data = SERVER_SIGN_UP_SUCCESS + "";
		msg->getUser()->send(data);
		return true;

	}
}



//this string disects message to username and password
string* Server::disect(string _data)
{
	string* all = new string[2];

	//position in disection
	int count = MESSAGE_CODE_LENGTH - 1;

	try {
		//username length
		int user_length = atoi(_data.substr(MESSAGE_CODE_LENGTH - 1, USER_BYTE_LENGTH).c_str());

		count += USER_BYTE_LENGTH;

		//username in the first slot in all array of strings
		all[0] = _data.substr(count, user_length);

		count += user_length;

		//password length
		int pass_length = atoi(_data.substr(count, PASS_BYTE_LENGTH).c_str());

		count += PASS_BYTE_LENGTH;

		all[1] = _data.substr(count, pass_length);
	}
	catch (...) {}
	

	return all;




}


//done
bool Server::checkUserAndPass(string username, string pass)
{
	vector<DBUser>::iterator it;
	for (it = _database.begin(); it != _database.end(); it++)
	{
		if ((it._Ptr->getUsername().compare(username) == 0) && (it._Ptr->getPass().compare(pass) == 0))
		{
			return true;
		}
	}

	return false;
}

//done
bool Server::isUserExist(string username)
{
	vector<DBUser>::iterator it;
	for (it = _database.begin(); it != _database.end(); it++)
	{
		if (it._Ptr->getUsername() == username)
		{
			return true;
		}
	}

	return false;
}

//done
void Server::addUser(string username, string pass)
{
	DBUser user =  DBUser(username, pass);
	_database.insert(_database.begin(), user);
}