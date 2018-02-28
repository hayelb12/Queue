#include "Validator.h"

//done
bool Validator::isPasswordValid(string password)
{
	if (password.length() < 4 || password.length()>20)
		return false;

	if (password.find(' ') != string::npos)
		return false;

	string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	const char* upChars = chars.c_str();

	if (password.find(upChars) == string::npos)
		return false;
	chars = "abcdefghijklmnopqrstuvxyz";
	const char* lowChars = chars.c_str();

	if (password.find(lowChars) == string::npos)
		return false;
	
	chars = "0123456789";
	const char* nums = chars.c_str();

	if (password.find(nums) == string::npos)
		return false;

	return true;
}
//done
bool Validator::isUsernameValid(string username)
{
	if (username == "")
		return false;
	
	if (username.find(' ') != string::npos)
		return false;

	if ((username[0] <'a' || username[0]> 'z') && (username[0] <'A' || username[0]> 'Z'))
		return false;

	if (username.length() > 20 || username.length() < 4)
		return false;
	return true;

}
