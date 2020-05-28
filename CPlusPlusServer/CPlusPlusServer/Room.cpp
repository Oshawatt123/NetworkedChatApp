#include "Room.h"

void Room::JoinRoom(Client& client)
{
	std::cout << "USER JOINING ROOM : " << client.socket << std::endl;
	UsersInRoom.emplace_back(&client);
}

void Room::LeaveRoom(Client* client)
{
	std::cout << "USER LEAVING ROOM : " << client->socket << std::endl;
	UsersInRoom.erase(std::remove(UsersInRoom.begin(), UsersInRoom.end(), client), UsersInRoom.end());
}

void Room::BroadcastMessage(std::string message)
{
	// send to all users in room
	for (auto client : UsersInRoom)
	{
		std::cout << "SENDING MESSAGE TO : " << client->socket << std::endl;
		send(client->socket, message.c_str(), message.size(), 0);
	}
}

void Room::WhisperMessage(std::string message, Client& recipient)
{

}