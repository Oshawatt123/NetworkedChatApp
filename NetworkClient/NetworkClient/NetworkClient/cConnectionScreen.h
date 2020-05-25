#pragma once

#include "chatServer.h"
#include "cMain.h"

class cConnectionScreen : public wxPanel
{
public:
	cConnectionScreen();
	~cConnectionScreen();

public:

	// connect page

	DECLARE_EVENT_TABLE();
};