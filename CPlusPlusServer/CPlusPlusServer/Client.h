#pragma once
#include <string>
#include <WinSock2.h>

class Room;
class Client
{
public:
	std::string username;
	std::string password;

	SOCKET socket;
	int inUse = false;
	int ID;

	Room* currentRoom = nullptr;

	Client();
	~Client();

	bool operator ==(const Client& b) const;
};