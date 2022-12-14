#pragma once
#include "ClientSocketHandler.h"
#include "ServerResponse.h"
#include "ClientRequest.h"

/*
	Handles the client registration to the server
*/
class RegistrationHandler
{
private:
	ClientSocketHandler* _clientSocketHandler;

public:
	RegistrationHandler(ClientSocketHandler* clientSocketHandler);
	bool registerClient(string clientUserName, uint8_t* clientUUIDBuffer);
};