#include "chatServer.h"

chatServer* chatServer::sInstance = NULL;

chatServer* chatServer::Instance()
{
	if (sInstance == nullptr)
	{
		sInstance = new chatServer();
	}
	return sInstance;
}

int chatServer::checkForMessage(std::string& buf)
{
	if (pingResult == SOCKET_ERROR)
	{
		// failed to connect with server!
		buf = "NO CONNECTION TO SERVER";
		return NO_CONNECTION;
	}
	else
	{
		// get incoming messages

		fd_set copy = master;

		ZeroMemory(buffer, 1024);

		int socketCount = select(0, &copy, nullptr, nullptr, &zeroTimeout);
		std::string socketCountString = "Number of active sockets: " + std::to_string(socketCount);
		//socketCountText->SetLabel(socketCountString);

		for (int i = 0; i < socketCount; i++)
		{
			SOCKET sock = copy.fd_array[i];

			memset(buffer, 0, sizeof(buffer));
			int bytesIn = 0;

			bytesIn = recv(serverSocket, buffer, sizeof(buffer), 0);
			std::string debugMsg = "Bytes received: " + std::to_string(bytesIn);

			if (bytesIn > 0)
			{
				doc.Parse(buffer, sizeof(buffer));

				std::string commandType = doc.FirstChildElement()->Name();

				if (commandType == "MESSAGE")
				{
					// return message text
					buf = doc.FirstChildElement()->GetText();
					return SV_MESSAGE;
				}
				else if (commandType == "ACRE")
				{
					buf = doc.FirstChildElement()->GetText();
					return SV_RESPONSE;
				}
			}
		}
	}

	// no message from server
	buf = "NO MESSAGE FROM SERVER";
	return NO_MESSAGE;
}

int chatServer::sendMessage(MESSAGE_TYPE messageType, std::string msg)
{
	if (pingResult != SOCKET_ERROR)
	{
		std::string packet = "";
		if (messageType == RAD_MESSAGE)
		{
			if (msg == "")
			{
				return 0;
			}
			packet.append("<MESSAGE>");
			packet.append(msg);
			packet.append("</MESSAGE>");
		}
		else if (messageType == RAD_DISCONNECT)
		{
			packet.append("<DISCONNECT>");
			packet.append("</DISCONNECT>");
		}
		else if (messageType == RAD_LOGIN)
		{
			packet.append("<LOGIN>");
			packet.append(msg);
			packet.append("</LOGIN>");
		}
		else if (messageType == RAD_REGISTER)
		{
			packet.append("<CREATE_USER>");
			packet.append(msg);
			packet.append("</CREATE_USER>");
		}
		else if (messageType == RAD_JOINROOM)
		{
			packet.append("<JOIN_ROOM>");
			packet.append(msg);
			packet.append("</JOIN_ROOM>");
		}
		else if (messageType == RAD_LISTROOM)
		{
			packet.append("<LIST_ROOMS>");
			packet.append("</LIST_ROOMS>");
		}
		else if (messageType == RAD_WHISPER)
		{
			packet.append("<WHISPER>");
			packet.append(msg);
			packet.append("</WHISPER>");
		}

		if (packet != "")
		{
			ZeroMemory(buffer, 1024);

			memcpy(&buffer, packet.c_str(), packet.length());

			send(serverSocket, buffer, 1024, 0);

			return 1;
		}
		
		return 0;
	}
	else
	{
		return 0;
	}
}

int chatServer::connectToServer(std::string _IP, std::string port)
{
	IP = _IP;

	serverSocket = socket(AF_INET, SOCK_STREAM, 0); // create socket to the server;

	serverAddr.sin_addr.s_addr = inet_addr(IP.c_str()); // set server address
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(std::stoi(port));

	pingResult = connect(serverSocket, (SOCKADDR*)& serverAddr, sizeof(serverAddr));

	if (pingResult == SOCKET_ERROR)
	{
		// no connection found to server
	}
	else
	{
		FD_ZERO(&master);
		FD_SET(serverSocket, &master);
	}

	return pingResult;
}

void chatServer::Release()
{
	sendMessage(RAD_DISCONNECT);
	closesocket(serverSocket);
	WSACleanup();

	delete sInstance;
	sInstance = nullptr;
}

chatServer::chatServer()
{
	WSAStartup(MAKEWORD(2, 0), &WSAData);
	

	// test connection with server
}

chatServer::~chatServer()
{
}
