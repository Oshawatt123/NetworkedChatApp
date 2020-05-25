#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

// already included from wxWidgets. Errors from multiple definitions otherwise
//#include <WinSock2.h>
//#include <string>

#include "chatServer.h"

#include <vector>

#pragma comment(lib, "Ws2_32.lib")

class cMain : public wxFrame
{
public:
	cMain();
	~cMain();

	void TestForIncomingMessages();

public:

	wxBoxSizer* panelSizer = nullptr;

	//connect panel
	wxPanel* connectPanel = nullptr;
	wxStaticText* IPText = nullptr;
	wxTextCtrl* IPInput = nullptr;
	wxStaticText* portText = nullptr;
	wxTextCtrl* portInput = nullptr;
	wxButton* connectButton = nullptr;

	void OnConnectButtonClicked(wxCommandEvent& evt);

	// chat page
	wxPanel* chatPanel = nullptr;
	wxButton* button1 = nullptr;
	wxTextCtrl* textInput = nullptr;
	wxListBox* chatHistory = nullptr;
	

	// login page
	wxPanel* loginPanel = nullptr;
	wxStaticText* usernameText = nullptr;
	wxTextCtrl* usernameInput = nullptr;
	wxStaticText* passwordText = nullptr;
	wxTextCtrl* passwordInput = nullptr;
	wxButton* loginButton = nullptr;

	void OnLoginButtonClicked(wxCommandEvent& evt);

	wxStaticText* usernameNewText = nullptr;
	wxTextCtrl* usernameNewInput = nullptr;
	wxStaticText* passwordNewText = nullptr;
	wxTextCtrl* passwordNewInput = nullptr;
	wxStaticText* passwordCheckText = nullptr;
	wxTextCtrl* passwordCheckInput = nullptr;
	wxButton* registerButton = nullptr;

	void OnRegisterButtonClicked(wxCommandEvent& evt);

	bool waitingForLoginResponse = false;
	bool loginValid = false;
	bool waitingForRegisterResponse = false;

	// debug jazz
	wxListBox* debugBox = nullptr;
	wxStaticText* socketCountText = nullptr;
	wxStaticText* secondDebugText = nullptr;

	wxTimer tim;

	void OnButtonClicked(wxCommandEvent& evt);


	void ParseKeyDown(wxKeyEvent& evt);

	void OnTimer(wxTimerEvent& event);

	DECLARE_EVENT_TABLE();

private:

	void SendMessageToServer();

	int pingResult = 0;

	static const int TIMER_ID = 10010;

};