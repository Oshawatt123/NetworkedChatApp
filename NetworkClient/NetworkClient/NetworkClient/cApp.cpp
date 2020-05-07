#include "cApp.h"

wxIMPLEMENT_APP(cApp);

cApp::cApp()
{
}

cApp::~cApp()
{
	// no need to delete anything. wxWidgets does this for us
	// if we delete anything that has events referencing it
	// then we will get fatal errors

	// wxWidgets will delete things when it is safe to do so
}

bool cApp::OnInit()
{
	frame1 = new cMain();
	frame1->Show();

	return true;
}

void cApp::OnIdle()
{
	frame1->TestForIncomingMessages();
}
