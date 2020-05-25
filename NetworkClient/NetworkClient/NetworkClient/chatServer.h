#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "wx/wx.h"
#pragma comment(lib, "Ws2_32.lib")

#include <string>

#include "tinyxml2.h"

enum MESSAGE_TYPE
{
	RAD_MESSAGE,
	RAD_DISCONNECT,
	RAD_LOGIN,
	RAD_REGISTER,
	RAD_WHISPER,
	RAD_ACKN
};

static const int NO_CONNECTION	= 0;
static const int NO_MESSAGE		= 1;
static const int SV_MESSAGE		= 2;
static const int SV_RESPONSE	= 3;

static const std::string PSS_OK = "0";
static const std::string ERR_ROOMINVALID = "1";
static const std::string ERR_ROOMFULL = "2";
static const std::string ERR_NEEDMOREPARAMS = "3";
static const std::string ERR_TOOMANYPARAMS = "4";
static const std::string ERR_INVALIDLOGIN = "5";
static const std::string ERR_ACCESS = "6";

class chatServer
{
public:
	static chatServer* Instance();

	int checkForMessage(std::string& buf);
	int pingServer();
	int sendMessage(MESSAGE_TYPE messageType, std::string msg = "");
	int connectToServer(std::string _IP, std::string port);

	void Release();

private:
	chatServer();
	~chatServer();

	static chatServer* sInstance;

	WSADATA WSAData;

	SOCKET serverSocket;
	SOCKADDR_IN serverAddr;

	fd_set master;
	timeval zeroTimeout;

	std::string IP = "127.0.0.1";

	int pingResult = SOCKET_ERROR;

	char buffer[1024];

	tinyxml2::XMLDocument doc;
};

