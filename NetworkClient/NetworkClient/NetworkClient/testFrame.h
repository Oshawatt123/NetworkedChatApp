#pragma once

#include "wx\wx.h"

class testFrame : public wxFrame
{
public:
	testFrame();
	~testFrame();

	wxPanel* panel1 = nullptr;
	wxPanel* panel2 = nullptr;

public:

	void OnNewPanel(wxCommandEvent& evt);

	DECLARE_EVENT_TABLE();
};

