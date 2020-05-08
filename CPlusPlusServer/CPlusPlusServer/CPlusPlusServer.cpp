// CPlusPlusServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <WinSock2.h>
#include <vector>
#include <thread>
#include <algorithm>

#include "tinyxml2.h"

#pragma comment(lib, "Ws2_32.lib")

struct clientSocket
{
	SOCKET socket;
	int ID;
};

static bool serverRunning = true;
static bool addingClient = false;

SOCKET serverSocket;
SOCKET newClient;
SOCKADDR_IN clientAddr;

std::vector<clientSocket*> clientSockets;

int newID = 0;

// this is run on a different thread
void getNewClients()
{
	while (serverRunning)
	{
		int clientAddrSize = sizeof(clientAddr); // size of client address
		// stops execution, seperate thread needed
		newClient = accept(serverSocket, (SOCKADDR*)& clientAddr, &clientAddrSize); // listen for new connections

		if (newClient != INVALID_SOCKET) // a new client has appeared
		{
			addingClient = true;
			std::cout << "Client connected!" << std::endl;

			// generate client information
			clientSocket* newClientSocket = new clientSocket();
			newClientSocket->socket = newClient;
			newClientSocket->ID = newID;
			newID++;

			// add new client to list of clients
			clientSockets.emplace_back(newClientSocket);
			std::cout << "Client added!" << std::endl;
			std::cout << "Number of connected clients: " << clientSockets.size() << std::endl;

			// allow access to the clientSockets vector again
			addingClient = false;
		}
	}
}

		//recv(clientSocket, buffer, sizeof(buffer), 0); // read data from the incoming connection, with no flags bc we dont care what it is
				 //system(buffer);

				//std::cout << buffer << std::endl;

				//create response from server
				//std::string responseText = "response from server: ";
				//responseText += buffer;

				//memcpy(buffer, responseText.c_str(), responseText.length()); // copy the server response into our buffer

				//send(clientSocket, buffer, sizeof(buffer), 0); // send our buffer to the client's socket

				//closesocket(clientSocket); // close the connection

				//std::cout << "Client Disconnected" << std::endl;

int main()
{
    std::cout << "Crikey Riley thats a big socket!\n";

	WSADATA WSAData; // data about windows socket implementation

	SOCKADDR_IN serverAddr; // addresses of the server and the client

	WSAStartup(MAKEWORD(2, 0), &WSAData);

	serverSocket = socket(AF_INET,	// we are using IPV4
		SOCK_STREAM,				// we want to stream data
		0);							// we dont care what protocol we use

	serverAddr.sin_addr.s_addr = INADDR_ANY; // listen to ANY IP that pings us
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(7777); // host port number

	bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)); // bind the socket to the address
	listen(serverSocket, 0); // listen to incoming connections

	// create thread for getting new client connections
	std::thread newConnectionThread(getNewClients);

	tinyxml2::XMLDocument doc;

	while (true) // ew
	{
		char buffer[1024]; // data in buffer

		// listen to each client, as long as the other thread isn't editing the vector
		if (!addingClient)
		{
			for (auto client : clientSockets)
			{
				memset(buffer, 0, sizeof(buffer)); // clear buffer, to be safe

				// get information from the client
				if (recv(client->socket, buffer, sizeof(buffer), 0))
				{
					std::cout << "Buffer: " << buffer << std::endl;

					// parse the buffer
					doc.Parse(buffer, sizeof(buffer));
					std::cout << doc.FirstChildElement()->Name() << std::endl;

					// get the first element, the command type
					std::string commandType = doc.FirstChildElement()->Name();

					// do something with what we have!
					if (commandType == "DISCONNECT")
					{
						std::cout << "Client disconnected : " << client->ID << std::endl;

						// disconnect the client
						clientSockets.erase(std::remove(clientSockets.begin(), clientSockets.end(), client));
						delete client;
						client = nullptr;

						std::cout << "Number of connected clients: " << clientSockets.size() << std::endl;
					}
					else if (commandType == "MESSAGE")
					{
						std::cout << buffer << std::endl;
					}
				}
			}
		}
	}

	// clean up WINSOCK jazz
	WSACleanup();

	// clean the thread
	serverRunning = false;
	newConnectionThread.join();

	return 0;
}