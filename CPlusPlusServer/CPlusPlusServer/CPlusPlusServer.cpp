// CPlusPlusServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <WinSock2.h>
#include <vector>
#include <thread>
#include <algorithm>

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

	while (true) // ew
	{
		char buffer[1024]; // data in buffer

		// listen to each client
		if (!addingClient)
		{
			for (auto client : clientSockets)
			{
				memset(buffer, 0, sizeof(buffer)); // clear buffer, to be safe

				if (recv(client->socket, buffer, sizeof(buffer), 0))
				{
					std::string bufferString = buffer;
					// do something with what we have!
					if (bufferString == "DISCONNECT")
					{
						std::cout << "Client disconnected : " << client->ID << std::endl;
						std::cout << clientSockets.size() << std::endl;
						// disconnect the client
						clientSockets.erase(std::remove(clientSockets.begin(), clientSockets.end(), client));
						delete client;
						client = nullptr;

						std::cout << clientSockets.size() << std::endl;
					}
					else
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

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
