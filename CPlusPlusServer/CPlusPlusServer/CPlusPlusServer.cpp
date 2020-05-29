// CPlusPlusServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <WinSock2.h>
#include <vector>
#include <unordered_map>
#include <thread>
#include <algorithm>

#include "tinyxml2.h"

#include "Client.h"
#include "Room.h"

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

fd_set master;
timeval zeroTimeout;

int newID = 0;
int roomID = 0;

std::vector<Client*>	registeredClients;
std::vector<Client*>	activeClients;
std::vector<Room*>		openRooms;

std::unordered_map<SOCKET, Client> sockToClientMap;

static const int PSS_OK = 0;
static const int ERR_ROOMINVALID = 1;
static const int ERR_ROOMFULL = 2;
static const int ERR_NEEDMOREPARAMS = 3;
static const int ERR_TOOMANYPARAMS = 4;
static const int ERR_INVALIDLOGIN = 5;
static const int ERR_ACCESS = 6;

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

			// generate client information - unused for now
			clientSocket* newClientSocket = new clientSocket();
			newClientSocket->socket = newClient;
			newClientSocket->ID = newID;
			newID++;

			// add new client to list of clients
			FD_SET(newClient, &master);

			//clientSockets.emplace_back(newClientSocket);
			std::cout << "Client added!" << std::endl;
			std::cout << "Number of connected clients: " << std::endl;

			// allow access to the clientSockets vector again
			addingClient = false;
		}
	}
}

std::string WrapTextInElement(std::string textToWrap, std::string element)
{
	return "<" + element + ">" + textToWrap + "</" + element + ">";
}

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

	FD_ZERO(&master);
	FD_SET(serverSocket, &master);

	// create thread for getting new client connections
	std::thread newConnectionThread(getNewClients);

	tinyxml2::XMLDocument doc;

	// hard-coded clients for QoL
	Client* admin = new Client();
	admin->username = "admin";
	admin->password = "radiator";
	admin->ID = 0;

	registeredClients.emplace_back(admin);

	Client* QoL = new Client();
	QoL->username = "w";
	QoL->password = "w";
	QoL->ID = 1;

	registeredClients.emplace_back(QoL);

	newID = 2;

	// hard-coded rooms for QoL
	Room* firstRoom = new Room(roomID++);
	openRooms.emplace_back(firstRoom);

	while (true) // ew
	{
		char buffer[1024]; // data in buffer

		// listen to each client, as long as the other thread isn't editing the vector
		if (!addingClient)
		{
			fd_set copy = master;


			int socketCount = select(0, &copy, nullptr, nullptr, &zeroTimeout);

			for (int i = 0; i < socketCount; i++)
			{
				SOCKET sock = copy.fd_array[i]; // the socket we are listening to

				memset(buffer, 0, sizeof(buffer)); // clear buffer, to be safe

				int bytesIn = 0;

				// make sure the socket isn't the server socket. This can be used for single threaded networking
				// but I already have a seperate thread for accepting clients so leaving it like that
				if(sock != serverSocket)
					bytesIn = recv(sock, buffer, 1024, 0); // get bytes from client

				if (bytesIn <= 0)
				{
					// nothing recieved from client
				}
				else
				{
					std::cout << buffer << std::endl;
					// parse the buffer
					doc.Parse(buffer, sizeof(buffer));
					std::cout << doc.FirstChildElement()->Name() << std::endl;

					// get the first element, the command type
					std::string commandType = doc.FirstChildElement()->Name();

					// do something with what we have!
					if (commandType == "DISCONNECT")
					{
						std::cout << "Client disconnected : " << std::endl;
						sockToClientMap[sock].inUse = false;
						sockToClientMap[sock].socket = NULL;
						// disconnect the client
						FD_CLR(sock, &master);

						// minus 1 because the serverSocket is in the set
						std::cout << "Number of connected clients: " << copy.fd_count -1 << std::endl;
					}
					else if (commandType == "MESSAGE")
					{
						std::cout << "Message from " << copy.fd_array[i] << ": " << doc.FirstChildElement()->GetText() << std::endl;
						std::string message = doc.FirstChildElement()->GetText();

						// MAKE SURE WE CLEAN THE BUFFER BEFORE WE WRITE TO IT
						memset(buffer, 0, sizeof(buffer));
						memcpy(buffer, message.c_str(), message.length());

						Client* currClient = &sockToClientMap[sock];
						Room* clientRoom = currClient->currentRoom;
						if (clientRoom == nullptr)
						{
							memset(buffer, 0, sizeof(buffer)); // clear buffer for sending, and write to it
							std::string response = WrapTextInElement(std::to_string(ERR_ROOMINVALID), "ACRE");
							memcpy(buffer, response.c_str(), response.length());
							send(master.fd_array[i], buffer, sizeof(buffer), 0);
						}

						//broadcast to other users in the room
						clientRoom->BroadcastMessage(buffer, *currClient);
					}
					else if (commandType == "CREATE_USER")
					{
						std::string username = doc.FirstChildElement("CREATE_USER")->FirstChildElement("username")->GetText();
						std::string password = doc.FirstChildElement("CREATE_USER")->FirstChildElement("password")->GetText();
						std::string passwordCheck = doc.FirstChildElement("CREATE_USER")->FirstChildElement("passwordCheck")->GetText();

						// check user doesnt already exist
						bool usernameAvailable = true;
						for (auto client : registeredClients)
						{
							if (client->username == username)
							{
								usernameAvailable = false;
							}
						}

						// check password
						if (password == passwordCheck && usernameAvailable)
						{
							Client* newClient = new Client();
							newClient->username = username;
							newClient->password = password;

							registeredClients.emplace_back(newClient);

							// send OK
							memset(buffer, 0, sizeof(buffer)); // clear buffer for sending, and write to it
							std::string response = WrapTextInElement(std::to_string(PSS_OK), "ACRE");
							memcpy(buffer, response.c_str(), response.length());

							send(sock, buffer, sizeof(buffer), 0);
							std::cout << "RESPONSE SENT: " << buffer << std::endl;
						}
						else
						{
							memset(buffer, 0, sizeof(buffer)); // clear buffer for sending, and write to it
							std::string response = WrapTextInElement(std::to_string(ERR_INVALIDLOGIN), "ACRE");
							memcpy(buffer, response.c_str(), response.length());
							send(master.fd_array[i], buffer, sizeof(buffer), 0);
						}
					}
					else if (commandType == "LOGIN")
					{
						bool loginFound = false;
						std::string clientUsername = doc.FirstChildElement("LOGIN")->FirstChildElement("username")->GetText();
						std::string clientPassword = doc.FirstChildElement("LOGIN")->FirstChildElement("password")->GetText();

						std::cout << "LOGIN ATTEMPT => " << " Socket: " << sock << " Username: " << clientUsername << " Password: " << clientPassword << std::endl;

						// check all registered users
						for (auto user : registeredClients)
						{
							if (user->username == clientUsername && !user->inUse)
							{
								if (user->password == clientPassword)
								{
									loginFound = true;

									// init user. this must go first as sockToClientMap does not use a pointer
									user->socket = sock;
									user->inUse = true;

									// add to appropriate vectors/maps
									activeClients.emplace_back(user);
									sockToClientMap.insert(std::make_pair(sock, *user));
									std::cout << "LOGIN SUCCESS => " << " Socket: " << sock << " Username: " << clientUsername << " Password: " << clientPassword << std::endl;
									
									// send command to allow login
									memset(buffer, 0, sizeof(buffer)); // clear buffer for sending, and write to it
									std::string response = WrapTextInElement(std::to_string(PSS_OK), "ACRE");
									memcpy(buffer, response.c_str(), response.length());

									send(sock, buffer, sizeof(buffer), 0);
									std::cout << "RESPONSE SENT: " << buffer << std::endl;
								}
							}
						}
						if (!loginFound)
						{
							// say screw you!
							std::string response = WrapTextInElement(std::to_string(ERR_INVALIDLOGIN), "ACRE");
							memcpy(buffer, response.c_str(), response.length());
							send(sock, buffer, sizeof(buffer), 0);
						}
					}
					else if (commandType == "JOIN_ROOM")
					{
						int roomNumber = std::stoi(doc.FirstChildElement()->GetText());
						std::cout << "REQUEST TO JOIN ROOM " << roomNumber << " from SOCKET " << sock << std::endl;

						// if room exists
						if (roomNumber < openRooms.size())
						{
							openRooms[roomNumber]->JoinRoom(&sockToClientMap[sock]);

							memset(buffer, 0, sizeof(buffer)); // clear buffer for sending, and write to it
							std::string response = WrapTextInElement(std::to_string(PSS_OK), "ACRE");
							memcpy(buffer, response.c_str(), response.length());

							send(sock, buffer, sizeof(buffer), 0);
							std::cout << "RESPONSE SENT: " << buffer << std::endl;
						}
					}
					else if (commandType == "LIST_ROOMS")
					{
						Client* currClient = &sockToClientMap[sock];
						Room* clientRoom = currClient->currentRoom;
						std::string msg = "";
						msg.append("Open Rooms: \n");
						for (auto room : openRooms)
						{
							msg.append("Room " + std::to_string(room->ID) + "\n");
							clientRoom->WhisperMessage(msg, *currClient);
						}

					}
					else if (commandType == "WHISPER")
					{
						std::string recipientIDStr = doc.FirstChildElement("WHISPER")->FirstChildElement("user")->GetText();
						std::string whisperMessage = doc.FirstChildElement("WHISPER")->FirstChildElement("message")->GetText();

						try
						{
							int recipientID = std::stoi(recipientIDStr);
							Client* recipient = nullptr;

							for (auto client : activeClients)
							{
								if (client->ID == recipientID)
									recipient = client;
							}

							if (recipient != nullptr)
							{
								Client* currClient = &sockToClientMap[sock];
								Room* clientRoom = currClient->currentRoom;

								if(clientRoom != nullptr)
									clientRoom->WhisperMessage(whisperMessage, *recipient, &sockToClientMap[sock]);
							}
						}
						catch(std::exception e)
						{
							std::cout << "ERROR PARSING RECIPIENT ID" << std::endl;
						}
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