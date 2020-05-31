#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <WinSock2.h>
#include <iostream>

#include "Client.h"

class Room
{
public:
	std::unordered_map<std::string, std::string> nickNames;

	int ID;
	int numUsersInRoom;
	int maxCapacity = 10;

public:
	void JoinRoom(Client* client);

	void LeaveRoom(Client* client);

	void BroadcastMessage(std::string message, Client& sender);

	void WhisperMessage(std::string message, Client& recipient, Client* sender = nullptr);

	std::string GetUsersList();

	Room(int id);
	~Room();

private:
	std::vector<Client*> UsersInRoom;
};