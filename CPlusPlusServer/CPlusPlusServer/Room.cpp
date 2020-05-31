#include "Room.h"

void Room::JoinRoom(Client* client)
{
	std::cout << "USER JOINING ROOM " << ID << " : " << client->socket << std::endl;
	client->currentRoom = this;
	UsersInRoom.emplace_back(client);

	numUsersInRoom++;
}

void Room::LeaveRoom(Client* client)
{
	std::cout << "USER LEAVING ROOM " << ID << " : " << client->socket << std::endl;
	UsersInRoom.erase(std::remove(UsersInRoom.begin(), UsersInRoom.end(), client), UsersInRoom.end());
	numUsersInRoom--;
}

void Room::BroadcastMessage(std::string message, Client& sender)
{
	// send to all users in room, except the user that sent the message
	for (auto client : UsersInRoom)
	{
		if (sender.socket != client->socket)
		{
			std::string msg = "<MESSAGE>" "[" + sender.username + "]" "[" + std::to_string(sender.ID) + "]: " + message + "</MESSAGE>";
			std::cout << "SENDING MESSAGE TO : " << client->socket << std::endl;
			send(client->socket, msg.c_str(), msg.size(), 0);
		}
	}
}

void Room::WhisperMessage(std::string message, Client& recipient, Client* sender)
{
	// server is sending the message
	if (sender == NULL)
	{
		std::string msg = "<MESSAGE>" + message + "</MESSAGE>";
		std::cout << "SERVER SENDING MESSAGE TO : " << recipient.socket << std::endl;
		send(recipient.socket, msg.c_str(), msg.size(), 0);
		std::cout << "MESSAGE SENT: " << msg << std::endl;
	}
	else
	{
		for (auto client : UsersInRoom)
		{
			if (client->socket == recipient.socket)
			{
				std::string msg = "<MESSAGE>" "Message from: " + sender->username + " :" + message + "</MESSAGE>";
				std::cout << "WHISPERING MESSAGE TO : " << client->socket << std::endl;
				send(client->socket, msg.c_str(), msg.size(), 0);
			}
		}
	}
}

std::string Room::GetUsersList()
{
	std::string returnMsg = "\n";

	for (auto client : UsersInRoom)
	{
		returnMsg.append("User " + std::to_string(client->ID) + ": " + client->username);
	}

	return returnMsg;
}

Room::Room(int id)
{
	ID = id;
	numUsersInRoom = 0;
}

Room::~Room()
{
	// clean up client pointers
	for (auto client : UsersInRoom)
	{
		client = nullptr;
		// no delete because we dont want to delete the client, they might be doing something still
		// i doubt it, but they might
	}
}