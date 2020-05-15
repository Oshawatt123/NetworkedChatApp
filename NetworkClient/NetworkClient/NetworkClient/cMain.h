#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

// already included from wxWidgets. Errors from multiple definitions otherwise
//#include <WinSock2.h>
//#include <string>

#include <vector>

#pragma comment(lib, "Ws2_32.lib")

#include "wx/wx.h"

#include "tinyxml2.h"

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
	wxListBox* debugBox = nullptr;
	wxStaticText* socketCountText = nullptr;

	wxTimer tim;

	void OnButtonClicked(wxCommandEvent& evt);

	void OnTimer(wxTimerEvent& event);

	DECLARE_EVENT_TABLE();

private:
	WSADATA WSAData;

	SOCKET serverSocket;
	SOCKADDR_IN serverAddr;

	fd_set master;
	timeval zeroTimeout;

	std::string IP = "127.0.0.1";

	int pingResult = 0;

	tinyxml2::XMLDocument doc;

	static const int TIMER_ID = 10010;

};