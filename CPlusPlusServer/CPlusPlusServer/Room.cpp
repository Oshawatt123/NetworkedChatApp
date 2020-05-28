#include "Room.h"

void Room::JoinRoom(Client& client)
{
	UsersInRoom.emplace_back(client);
}

void Room::LeaveRoom(Client* client)
{
	auto it = std::find(UsersInRoom.begin(), UsersInRoom.end(), client);
	//UsersInRoom.erase(std::remove(UsersInRoom.begin(), UsersInRoom.end(), client), UsersInRoom.end());
}

void Room::BroadcastMessage(std::string message)
{
}

void Room::WhisperMessage(std::string message, Client& recipient)
{
}