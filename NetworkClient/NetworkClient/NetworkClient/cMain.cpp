#include "cMain.h"

wxBEGIN_EVENT_TABLE(cMain, wxFrame)
	EVT_BUTTON(10001, OnConnectButtonClicked)
	EVT_BUTTON(10002, OnLoginButtonClicked)
	EVT_BUTTON(10003, OnRegisterButtonClicked)
	EVT_BUTTON(10005, OnButtonClicked)
	EVT_TIMER(TIMER_ID, cMain::OnTimer)
	EVT_KEY_DOWN(ParseKeyDown)
wxEND_EVENT_TABLE()

cMain::cMain() :	wxFrame(nullptr, wxID_ANY, "Radiator Network", wxPoint(30,30), wxSize(800, 600)),
					tim(this, TIMER_ID)
{

	panelSizer = new wxBoxSizer(wxHORIZONTAL);

	// connect panel initialization
	connectPanel = new wxPanel(this);
	IPText = new wxStaticText(connectPanel, wxID_ANY, "IP", wxPoint(10, 50), wxSize(100, 20));
	IPInput = new wxTextCtrl(connectPanel, wxID_ANY, "", wxPoint(10, 70), wxSize(300, 30));
	portText = new wxStaticText(connectPanel, wxID_ANY, "Port", wxPoint(10, 110), wxSize(100, 20));
	portInput = new wxTextCtrl(connectPanel, wxID_ANY, "", wxPoint(10, 130), wxSize(300, 30));
	connectButton = new wxButton(connectPanel, 10001, "Connect", wxPoint(10, 170), wxSize(150, 50));

	connectPanel->SetSize(this->GetClientSize());

	// login panel initialization
	loginPanel = new wxPanel(this);

	usernameText = new wxStaticText(loginPanel, wxID_ANY, "Username", wxPoint(10, 50), wxSize(100, 20));
	usernameInput = new wxTextCtrl(loginPanel, wxID_ANY, "", wxPoint(10, 70), wxSize(150, 30));
	passwordText = new wxStaticText(loginPanel, wxID_ANY, "Password", wxPoint(10, 110), wxSize(100, 20));
	passwordInput = new wxTextCtrl(loginPanel, wxID_ANY, "", wxPoint(10, 130), wxSize(150, 30));
	loginButton = new wxButton(loginPanel, 10002, "Login", wxPoint(10, 170), wxSize(150, 50));

	usernameNewText = new wxStaticText(loginPanel, wxID_ANY, "Username", wxPoint(200, 50), wxSize(100, 20));
	usernameNewInput = new wxTextCtrl(loginPanel, wxID_ANY, "", wxPoint(200, 70), wxSize(150, 30));
	passwordNewText = new wxStaticText(loginPanel, wxID_ANY, "Password", wxPoint(200, 110), wxSize(100, 20));
	passwordNewInput = new wxTextCtrl(loginPanel, wxID_ANY, "", wxPoint(200, 130), wxSize(150, 30));
	passwordCheckText = new wxStaticText(loginPanel, wxID_ANY, "Password verification", wxPoint(200, 170), wxSize(200, 20));
	passwordCheckInput = new wxTextCtrl(loginPanel, wxID_ANY, "", wxPoint(200, 190), wxSize(150, 30));
	registerButton = new wxButton(loginPanel, 10003, "Register", wxPoint(200, 230), wxSize(150, 50));

	loginPanel->SetSize(this->GetClientSize());

	// chat panel initialization
	chatPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	button1 = new wxButton(chatPanel, 10005, "Send", wxPoint(330, 20), wxSize(50, 30));
	textInput = new wxTextCtrl(chatPanel, wxID_ANY, "", wxPoint(10, 20), wxSize(300, 30));
	chatHistory = new wxListBox(chatPanel, wxID_ANY, wxPoint(10, 70), wxSize(300, 300));

	chatPanel->SetSize(this->GetClientSize());

	debugBox = new wxListBox(this, wxID_ANY, wxPoint(410, 110), wxSize(300, 300));
	//socketCountText = new wxStaticText(this, 10101, "Notice me senpai", wxPoint(410, 10));
	secondDebugText = new wxStaticText(this, 10101, "Notice me senpai", wxPoint(410, 10), wxSize(300, 10));

	connectPanel->Show();
	loginPanel->Hide();
	chatPanel->Hide();

	tim.Start(1);
}

cMain::~cMain()
{
	chatServer::Instance()->sendMessage(RAD_DISCONNECT);

	delete panelSizer;
	panelSizer = nullptr;
}

void cMain::TestForIncomingMessages()
{
	//socketCountText->SetLabel("WHYYYY");
	//debugBox->AppendString("Testing....");
	std::string stringbuffer = "";

	int response = chatServer::Instance()->checkForMessage(stringbuffer);

	secondDebugText->SetLabel(stringbuffer);
	if (response == NO_MESSAGE || response == NO_CONNECTION)
	{

	}
	else if (response == SV_MESSAGE)
	{
		std::stringstream ss(stringbuffer.c_str());
		std::string to;

		while (std::getline(ss, to, '\n')) {
			chatHistory->Append(to);
		}

		chatHistory->SetSelection(chatHistory->GetCount() - 1);
		debugBox->Append("SV_MESSAGE");
	}
	else if (response == SV_RESPONSE)
	{
		if (stringbuffer == PSS_OK)
		{
			debugBox->Append("SV_RESPONSE: PSS_OK");
			if (waitingForLoginResponse)
			{
				waitingForLoginResponse = false;
				loginValid = true;

				loginPanel->Hide();
				chatPanel->Show();

				chatHistory->Append("Welcome to the Radiator chat application!");
				chatHistory->Append("You are currently not in a room. To list the");
				chatHistory->Append("available rooms, type !LIST_ROOMS");
				chatHistory->Append("Once you have chosen a room, type !JOIN_ROOM x");
				chatHistory->Append("where x is the room number you wish to join.");
			}
			else if (waitingForRegisterResponse)
			{
				waitingForRegisterResponse = false;
				debugBox->Append("Registration successful! Safe to login.");
			}
			else if (joiningRoom)
			{
				joiningRoom = false;
				chatHistory->Append("Successfully joined room");
				chatHistory->Append("");
			}
		}
		else if (stringbuffer == ERR_INVALIDLOGIN)
		{
			waitingForLoginResponse = false;
			debugBox->Append("Unsuccessful login attempt. Try again");
		}
		else if (stringbuffer == ERR_ROOMINVALID)
		{
			joiningRoom = false;
			chatHistory->Append("Attempt at joining the room was fruitful. The room does not exist");
			chatHistory->Append("");
			debugBox->Append("ERR_ROOMINVALID");
		}
		else if (stringbuffer == ERR_ROOMFULL)
		{
			joiningRoom = false;
			chatHistory->Append("Attempt at jining the room was fruitful. The room is out of.... room");
			chatHistory->Append("");
			debugBox->Append("ERR_ROOMFULL");
		}
		else if (stringbuffer == ERR_NEEDMOREPARAMS)
		{
			chatHistory->Append("An unexcepted error has occured with your command.");
			chatHistory->Append("Not enough parameters were sent to the server");
			chatHistory->Append("or the parameters sent were invalid / corrupted in transit.");
			chatHistory->Append("Please try again.");
			chatHistory->Append("");
			debugBox->Append("ERR_NEEDMOREPARAMS");
		}
		else if (stringbuffer == ERR_TOOMANYPARAMS)
		{
			chatHistory->Append("An unexcepted error has occured with your command.");
			chatHistory->Append("Too many parameters were sent to the server. Please try again");
			chatHistory->Append("");
			debugBox->Append("ERR_TOOMANYPARAMS");
		}
		else if (stringbuffer == ERR_ACCESS)
		{
			chatHistory->Append("You do not have access to this command at this time");
			chatHistory->Append("");
			debugBox->Append("ERR_ACCESS");
		}
		else if (stringbuffer == ERR_INVALIDRGSTR)
		{
			waitingForRegisterResponse = false;
			debugBox->Append("Username taken. Try another.");
		}
		else
		{
			debugBox->Append("SV_RESPONSE: UNDEFINED");
		}
	}
	
	//debugBox->AppendString("Test over....");
	//debugBox->AppendString("");
}

void cMain::OnConnectButtonClicked(wxCommandEvent& evt)
{
	std::string iptoconnect = IPInput->GetValue().ToStdString();
	if (iptoconnect == "")
	{
		iptoconnect = "127.0.0.1";
	}
	std::string porttoconnect = portInput->GetValue().ToStdString();
	if (porttoconnect == "")
	{
		porttoconnect = "7777";
	}

	if (chatServer::Instance()->connectToServer(iptoconnect, porttoconnect) == SOCKET_ERROR)
	{
		// show a dialog or summin saying how there was no good connection
		debugBox->Append("[ERR] Error connecting to server. If the issue persists, the server is likely down. Try localhosting you own.");
	}
	else
	{
		connectPanel->Hide();
		loginPanel->Show();
	}
}

void cMain::OnLoginButtonClicked(wxCommandEvent& evt)
{
	std::string userName = usernameInput->GetValue().ToStdString();
	std::string password = passwordInput->GetValue().ToStdString();
	if (userName == "" || password == "")
	{
		// show a dialog box to say you did goof
		debugBox->Append("Please input something into the following fields:");
		debugBox->Append("     Username");
		debugBox->Append("     Password");
	}
	else
	{
		std::string loginPacket = "";

		loginPacket.append("<username>" + userName + "</username>");
		loginPacket.append("<password>" + password + "</password>");

		chatServer::Instance()->sendMessage(RAD_LOGIN, loginPacket);
		username = userName;
		waitingForLoginResponse = true;
	}
	
}

void cMain::OnRegisterButtonClicked(wxCommandEvent& evt)
{
	waitingForLoginResponse = false;
	std::string userName = usernameNewInput->GetValue().ToStdString();
	std::string password = passwordNewInput->GetValue().ToStdString();
	std::string passwordCheck = passwordCheckInput->GetValue().ToStdString();
	if (userName == "" || password == "" || passwordCheck == "")
	{
		// show a dialog box to say you did goof
		debugBox->Append("Please input something into the following fields:");
		debugBox->Append("     Username");
		debugBox->Append("     Password");
		debugBox->Append("     Password verification");
	}
	else
	{
		std::string registerPacket = "";

		registerPacket.append("<username>" + userName + "</username>");
		registerPacket.append("<password>" + password + "</password>");
		registerPacket.append("<passwordCheck>" + passwordCheck + "</passwordCheck>");

		chatServer::Instance()->sendMessage(RAD_REGISTER, registerPacket);
		debugBox->Append("Waiting for confirmation of registration...");
		waitingForRegisterResponse = true;
	}
}

void cMain::OnButtonClicked(wxCommandEvent& evt)
{
	SendMessageToServer();

	evt.Skip();
}

void cMain::ParseKeyDown(wxKeyEvent& evt)
{
	evt.Skip();

	if (evt.GetKeyCode() == WXK_RETURN)
	{
		SendMessageToServer();
		evt.Skip(false);
	}
}

void cMain::OnTimer(wxTimerEvent& event)
{
	TestForIncomingMessages();
	tim.Start();
}

void cMain::SendMessageToServer()
{
	std::string msg = textInput->GetValue().ToStdString();
	if (msg[0] == '!')
	{
		std::string command = getCommandFromString(msg);

		if (command == "LIST_ROOMS")
		{
			chatHistory->Append("Asking server for rooms");
			chatServer::Instance()->sendMessage(RAD_LISTROOM);
		}
		else if (command == "JOIN_ROOM")
		{
			std::string roomNumber = msg;
			roomNumber.erase(0, 10);
			try
			{
				int roomNumer = std::stoi(roomNumber);
				chatHistory->Append("Joining room...");
				chatServer::Instance()->sendMessage(RAD_JOINROOM, roomNumber);
				joiningRoom = true;
			}
			catch (std::exception e)
			{
				chatHistory->Append("Room number invalid. Try again");
			}
		}
		else if (command == "WHISPER")
		{
			std::stringstream ss(msg.c_str());
			std::string to;

			std::vector<std::string> params;

			while (std::getline(ss, to, ' ')) {
				params.emplace_back(to);
			}

			if (params.size() >= 3)
			{

				std::string recipient = params[1];
				std::string message = "";
				for (unsigned int i = 2; i < params.size(); i++)
				{
					message.append(params[i] + " ");
				}

				std::string packetString = "";
				packetString.append("<user>" + recipient + "</user>");
				packetString.append("<message>" + message + "</message>");

				chatServer::Instance()->sendMessage(RAD_WHISPER, packetString);
			}
			else
			{
				chatHistory->Append("NOT ENOUGH PARAMETERS IN COMMAND");
			}
		}
		else if (command == "LIST_USERS")
		{
			chatHistory->Append("Asking server for users...");
			chatServer::Instance()->sendMessage(RAD_LISTUSER);
		}
		else if (command == "LEAVE_ROOM")
		{
			chatHistory->Append("Leaving room...");
			chatServer::Instance()->sendMessage(RAD_LEAVEROOM);
		}
	}
	else
	{
		// check for < or >
		std::string checkString = "<";

		size_t found = msg.find("<");
		if (found == std::string::npos)
		{

			chatHistory->AppendString("[" + username + "]: " + msg);

			chatServer::Instance()->sendMessage(RAD_MESSAGE, textInput->GetValue().ToStdString());
		}
		else
		{
			chatHistory->Append("");
			chatHistory->Append("############ERROR#############");
			chatHistory->Append("Illegal character found: '<'");
			chatHistory->Append("Message not sent");
			chatHistory->Append("##############################");
			chatHistory->Append("");
		}
	}
	textInput->SetValue("");

	chatHistory->SetSelection(chatHistory->GetCount() - 1);
}

std::string cMain::getCommandFromString(std::string msg)
{
	bool spaceFound = false;
	std::string command = "";

	// start from 1 to ignore the '!'
	for (unsigned int i = 1; i < msg.length(); i++)
	{
		if (msg[i] == ' ')
			break;
		command.append(msg.substr(i, 1));
	}

	return command;
}