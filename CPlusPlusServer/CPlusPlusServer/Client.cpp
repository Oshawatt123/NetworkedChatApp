#include "Client.h"

Client::Client()
{
}

Client::~Client()
{
}

bool Client::operator ==(const Client& b) const
{
	return username == b.username;
}
