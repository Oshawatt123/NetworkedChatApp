#pragma once

//#include "wx/wx.h"

#include "cMain.h"
#include "testFrame.h"
#include "chatServer.h"

class cApp : public wxApp
{
public:
	cApp();
	~cApp();

private:
	cMain* mainFrame = nullptr;

public:
	bool OnInit();
	void onIdle(wxIdleEvent& event);
};

