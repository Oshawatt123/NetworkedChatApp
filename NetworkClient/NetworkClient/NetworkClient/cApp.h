#pragma once

//#include "wx/wx.h"

#include "cMain.h"

class cApp : public wxApp
{
public:
	cApp();
	~cApp();

private:
	cMain* frame1 = nullptr;

public:
	bool OnInit();
	void onIdle(wxIdleEvent& event);
};

