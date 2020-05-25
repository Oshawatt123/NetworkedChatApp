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

	chatServer::Instance()->Release();
}

bool cApp::OnInit()
{
	mainFrame = new cMain();
	mainFrame->Show();

	return true;
}

void cApp::onIdle(wxIdleEvent& event)
{
}
