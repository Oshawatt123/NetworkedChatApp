#include "cMain.h"

wxBEGIN_EVENT_TABLE(cMain, wxFrame)
	EVT_BUTTON(10001, OnButtonClicked)
wxEND_EVENT_TABLE()

cMain::cMain() : wxFrame(nullptr, wxID_ANY, "Radiator Network", wxPoint(30,30), wxSize(800, 600))
{
	WSAStartup(MAKEWORD(2, 0), &WSAData);
	serverSocket = socket(AF_INET, SOCK_STREAM, 0); // create socket to the server;

	serverAddr.sin_addr.s_addr = inet_addr(IP.c_str()); // set server address
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(7777);

	// test connection with server
	pingResult = connect(serverSocket, (SOCKADDR*)& serverAddr, sizeof(serverAddr));

	button1 = new wxButton(this, 10001, "Click Me!", wxPoint(10, 10), wxSize(150, 50));
	textInput = new wxTextCtrl(this, wxID_ANY, "", wxPoint(10, 70), wxSize(300, 30));
	chatHistory = new wxListBox(this, wxID_ANY, wxPoint(10, 110), wxSize(300, 300));
}

cMain::~cMain()
{
	std::string disconnectString = "DISCONNECT";
	send(serverSocket, disconnectString.c_str(), disconnectString.length(), 0);

	closesocket(serverSocket);
	WSACleanup();
}

void cMain::TestForIncomingMessages()
{
	if (pingResult == SOCKET_ERROR)
	{
		// failed to connect with server!
	}
	else
	{
		// get incoming messages
	}
}

void cMain::OnButtonClicked(wxCommandEvent& evt)
{
	chatHistory->AppendString(textInput->GetValue());
	wxString msg = textInput->GetValue();

	// send the message to the server
	send(serverSocket, msg.c_str(), msg.length(), 0);
	evt.Skip();
}