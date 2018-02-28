#pragma once

#include <iostream>
#include<stdlib.h>

using namespace std;
class DBUser
{
public:
	DBUser();
	DBUser(string, string);
	~DBUser();

	string getUsername();
	string getPass();

	void setUsername(string);
	void setPass(string);

private:
	string _username;
	string _pass;
};

