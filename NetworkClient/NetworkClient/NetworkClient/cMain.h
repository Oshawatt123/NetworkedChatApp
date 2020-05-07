#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
//#include <WinSock2.h>
//#include <string>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

#include "wx/wx.h"

class cMain : public wxFrame
{
public:
	cMain();
	~cMain();

	void TestForIncomingMessages();

public:
	wxButton* button1 = nullptr;
	wxTextCtrl* textInput = nullptr;
	wxListBox* chatHistory = nullptr;

	void OnButtonClicked(wxCommandEvent& evt);

	wxDECLARE_EVENT_TABLE();

private:
	WSADATA WSAData;

	SOCKET serverSocket;
	SOCKADDR_IN serverAddr;

	std::string IP = "127.0.0.1";

	int pingResult = 0;

};