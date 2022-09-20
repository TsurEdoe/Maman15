#pragma once
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <exception>
#include <string>
#include "ClientSocketHandler.h"
#include "RegistrationHandler.h"
#include "FileUtils.h"
#include "RSAWrapper.h"
#include "Base64Wrapper.h"

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
	RegistrationHandler* _registrationHandler;
	string _userName;
	string _fileFullPath;
	uint8_t _clientUUID[UUID_LENGTH] = { 0 };
	RSAWrapper* _rsaWrapper;

	bool getTransferInformation();
	bool getClientConnectionInfo();
public:
	ClientInitializer();
	~ClientInitializer();
	bool initializeClient();

	string getUserName();
	string getFileFullPath();
	uint8_t* getClientUUID();
	ClientSocketHandler* getClientSocketHandler();
	RSAWrapper* getRSAWrapper();
};