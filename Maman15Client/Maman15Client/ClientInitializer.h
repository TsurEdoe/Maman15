#pragma once
#include "ClientSocketHandler.h"
#include "RegistrationHandler.h"
#include "FileUtils.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <exception>

#define TRANSFER_INFO_FILE			"transfer.info"
#define CLIENT_CONNECTION_INFO_FILE "me.info"
#define FILE_BUFFER_SIZE			1024
using namespace std;

/*
	Handles Client initialization:
	1. Reads transfer.info
	2. Opens connection to the server
	3. Registers if needed
*/
class ClientInitializer
{
private:
	ClientSocketHandler* _clientSocketHandler;
	RegistrationHandler _registrationHandler;
	string _userName;
	string _fileFullPath;

	bool getTransferInformation();
public:
	ClientInitializer();
	ClientSocketHandler* initializeClient();
};