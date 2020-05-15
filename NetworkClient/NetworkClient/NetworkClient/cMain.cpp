#include "cMain.h"

wxBEGIN_EVENT_TABLE(cMain, wxFrame)
	EVT_BUTTON(10001, OnButtonClicked)
	EVT_TIMER(TIMER_ID, cMain::OnTimer)
wxEND_EVENT_TABLE()

cMain::cMain() :	wxFrame(nullptr, wxID_ANY, "Radiator Network", wxPoint(30,30), wxSize(800, 600)),
					tim(this, TIMER_ID)
{
	WSAStartup(MAKEWORD(2, 0), &WSAData);
	serverSocket = socket(AF_INET, SOCK_STREAM, 0); // create socket to the server;

	serverAddr.sin_addr.s_addr = inet_addr(IP.c_str()); // set server address
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(7777);

	// test connection with server
	pingResult = connect(serverSocket, (SOCKADDR*)& serverAddr, sizeof(serverAddr));

	FD_ZERO(&master);
	FD_SET(serverSocket, &master);

	button1 = new wxButton(this, 10001, "Click Me!", wxPoint(10, 10), wxSize(150, 50));
	textInput = new wxTextCtrl(this, wxID_ANY, "", wxPoint(10, 70), wxSize(300, 30));
	chatHistory = new wxListBox(this, wxID_ANY, wxPoint(10, 110), wxSize(300, 300));
	debugBox = new wxListBox(this, wxID_ANY, wxPoint(410, 110), wxSize(300, 300));
	socketCountText = new wxStaticText(this, 10101, "Notice me senpai", wxPoint(410, 10), wxSize(300, 300));

	tim.Start(1);
}

cMain::~cMain()
{
	std::string disconnectString = "<DISCONNECT>";
	disconnectString.append("</DISCONNECT>");
	send(serverSocket, disconnectString.c_str(), disconnectString.length(), 0);

	closesocket(serverSocket);
	WSACleanup();
}

void cMain::TestForIncomingMessages()
{
	socketCountText->SetLabel("WHYYYY");
	debugBox->AppendString("Testing....");

	if (pingResult == SOCKET_ERROR)
	{
		// failed to connect with server!
		socketCountText->SetLabel("Failed to connect to server!");
	}
	else
	{
		// get incoming messages

		fd_set copy = master;

		char buffer[1024];

		int socketCount = select(0, &copy, nullptr, nullptr, &zeroTimeout);
		std::string socketCountString = "Number of active sockets: " + std::to_string(socketCount);
		socketCountText->SetLabel(socketCountString);
		debugBox->AppendString(socketCountString);

		for (int i = 0; i < socketCount; i++)
		{
			SOCKET sock = copy.fd_array[i];

			memset(buffer, 0, sizeof(buffer));
			int bytesIn = 0;

			bytesIn = recv(serverSocket, buffer, sizeof(buffer), 0);
			std::string debugMsg = "Bytes received: " + std::to_string(bytesIn);
			debugBox->AppendString(debugMsg);

			if (bytesIn > 0)
			{
				doc.Parse(buffer, sizeof(buffer));

				std::string commandType = doc.FirstChildElement()->Name();

				if (commandType == "MESSAGE")
				{
					chatHistory->AppendString(doc.FirstChildElement()->GetText());
				}
			}
		}
	}
	debugBox->AppendString("Test over....");
	debugBox->AppendString("");
}

void cMain::OnButtonClicked(wxCommandEvent& evt)
{
	chatHistory->AppendString(textInput->GetValue());

	std::string msg;
	msg.append("<MESSAGE>");
	msg.append(textInput->GetValue().ToStdString());
	msg.append("</MESSAGE>");

	// send the message to the server
	send(serverSocket, msg.c_str(), msg.length(), 0);
	evt.Skip();
}

void cMain::OnTimer(wxTimerEvent& event)
{
	TestForIncomingMessages();
}
