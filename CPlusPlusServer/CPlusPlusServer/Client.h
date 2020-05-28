#pragma once
#include <string>

class Room;
class Client
{
public:
	std::string username;
	std::string password;

	Room* currentRoom = nullptr;

	Client();
	~Client();

	bool operator ==(const Client& b) const;
};