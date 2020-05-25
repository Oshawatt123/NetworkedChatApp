#include "testFrame.h"

wxBEGIN_EVENT_TABLE(testFrame, wxFrame)
	EVT_BUTTON(10056, testFrame::OnNewPanel)
wxEND_EVENT_TABLE()

testFrame::testFrame() : wxFrame(nullptr, wxID_ANY, "Testing, testing...", wxPoint(30, 30), wxSize(800, 600))
{
	panel1 = new wxPanel(this);

	wxButton* panel1Button = new wxButton(panel1, 10056, "WOAH");

	panel2 = new wxPanel(this);

	wxStaticText* myText = new wxStaticText(panel2, wxID_ANY, "OH MY WORD NO WAY");

	panel1->Show();
	panel2->Hide();
}

testFrame::~testFrame()
{
}

void testFrame::OnNewPanel(wxCommandEvent& evt)
{
	panel1->Hide();
	panel2->Show();
}
