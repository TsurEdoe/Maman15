#pragma once
#include "ClientSocketHandler.h"
#include "ClientRequest.h"
#include "ServerResponse.h"

class RegistrationHandler
{
private:
	ClientSocketHandler* _clientSocketHandler;

public:
	RegistrationHandler(ClientSocketHandler* clientSocketHandler);
	bool registerClient(string clientUserName);
};