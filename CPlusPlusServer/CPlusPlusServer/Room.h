#pragma once
#include <vector>
#include <unordered_map>
#include <string>

#include "Client.h"

class Room
{
public:

	std::vector<Client> UsersInRoom;

	std::unordered_map<std::string, std::string> nickNames;

public:
	void JoinRoom(Client& client);

	void LeaveRoom(Client& client);

	void BroadcastMessage(std::string message);

	void WhisperMessage(std::string message, Client& recipient);
};

