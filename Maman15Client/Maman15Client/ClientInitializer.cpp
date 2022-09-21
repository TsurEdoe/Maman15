#include "ClientInitializer.h"

/*
    C'tor
*/
ClientInitializer::ClientInitializer()
{
	this->_rsaWrapper = NULL;
	this->_clientSocketHandler = NULL;
	this->_registrationHandler = NULL;
	this->_userName = "";
	this->_fileFullPath = "";
}

/*
    D'tor
*/
ClientInitializer::~ClientInitializer()
{
	delete this->_rsaWrapper;
	this->_rsaWrapper = NULL;

	delete this->_clientSocketHandler;
	this->_clientSocketHandler = NULL;

	delete this->_registrationHandler;
	this->_registrationHandler = NULL;
}

ClientSocketHandler* ClientInitializer::getClientSocketHandler()
{
	return this->_clientSocketHandler;
}

RSAWrapper* ClientInitializer::getRSAWrapper()
{
	return this->_rsaWrapper;
}

uint8_t* ClientInitializer::getClientUUID()
{
	return this->_clientUUID;
}

string ClientInitializer::getUserName()
{
	return this->_userName;
}

/*
    Initializes the client and returns the communication channel opened with the server
*/
bool ClientInitializer::initializeClient()
{
	if (!getTransferInformation())
	{
		cout << "ERROR: Failed initializing client!" << endl;
		return false;
	}

	this->_registrationHandler = new RegistrationHandler(this->_clientSocketHandler);

	if (FileUtils::doesFileExist(CLIENT_CONNECTION_INFO_FILE))
	{
		cout << "Found " << CLIENT_CONNECTION_INFO_FILE << " skipping registration." << endl;
		return getClientConnectionInfo();
	}
	else
	{
		cout << CLIENT_CONNECTION_INFO_FILE << " not found, going to registration." << endl;
		_registrationHandler->registerClient(this->_userName, _clientUUID);

		if (this->_clientUUID == NULL)
		{
			cout << "Error initializng client, failed registering with server" << endl;
			return false;
		}

		this->_rsaWrapper = new RSAWrapper();
		
		cout << "Client " << this->_userName << " registered, writing client connection information to " << CLIENT_CONNECTION_INFO_FILE << " file." << endl;

		string clientConnectionInfo[3];
		clientConnectionInfo[0] = this->_userName;
		clientConnectionInfo[1] = (char*)this->_clientUUID;
		clientConnectionInfo[2] = Base64Wrapper::encode(this->_rsaWrapper->getPrivateKey());
		
		string clientConnectionInfoBuffer = boost::algorithm::join(clientConnectionInfo, "\n");
		
		fstream fs;
		if (!FileUtils::fileRequestOpen(CLIENT_CONNECTION_INFO_FILE, fs))
		{
			cout << "ERROR: Connecting to server, file " << CLIENT_CONNECTION_INFO_FILE << " failed to open." << endl;
			return false;
		}

		if (!FileUtils::writeToFile(fs, (uint8_t*)clientConnectionInfoBuffer.c_str(), clientConnectionInfoBuffer.size()))
		{
			cout << "ERROR: Connecting to server, file " << CLIENT_CONNECTION_INFO_FILE << " failed to be read." << endl;
			FileUtils::closeFile(fs);
			return false;
		}
		
		cout << "Wrote client connection information successfully." << endl;
		FileUtils::closeFile(fs);
		return true;
	}
}

bool ClientInitializer::getClientConnectionInfo()
{
	uint8_t clientConnectionInfoBuffer[FILE_BUFFER_SIZE];

	fstream fs;
	if (!FileUtils::fileRequestOpen(CLIENT_CONNECTION_INFO_FILE, fs))
	{
		cout << "ERROR: Connecting to server, file " << CLIENT_CONNECTION_INFO_FILE << " failed to open." << endl;
		return false;
	}

	if (!FileUtils::readFromFile(fs, clientConnectionInfoBuffer, FILE_BUFFER_SIZE))
	{
		cout << "ERROR: Connecting to server, file " << CLIENT_CONNECTION_INFO_FILE << " failed to be read." << endl;
		FileUtils::closeFile(fs);
		return false;
	}

	FileUtils::closeFile(fs);

	vector<string> clientConnectionInfo;
	boost::split(clientConnectionInfo, clientConnectionInfoBuffer, boost::is_any_of("\n"));

	if (clientConnectionInfo.size() != 3)
	{
		cout << "ERROR: Connecting to server, client connection information not is in wrong pattern" << endl;
		return false;
	}

	if (this->_userName != clientConnectionInfo[0])
	{
		cout << "User name in " << TRANSFER_INFO_FILE << " file and " << CLIENT_CONNECTION_INFO_FILE << " file don't match" << endl;
		return false;
	}

	if (clientConnectionInfo[1].size() > UUID_LENGTH)
	{
		cout << "UUID in " << CLIENT_CONNECTION_INFO_FILE << " file is too long" << endl;
		return false;
	}
	
	memcpy_s(this->_clientUUID, UUID_LENGTH, clientConnectionInfo[1].c_str(), clientConnectionInfo.size());

	this->_rsaWrapper = new RSAWrapper(Base64Wrapper::decode(clientConnectionInfo[2]));

	cout << "SUCCESS: Transfer information read successfully!" << endl;

	return true;
}

bool ClientInitializer::getTransferInformation()
{
	uint8_t transferInfoBuffer[FILE_BUFFER_SIZE];

	fstream fs;
	if (!FileUtils::fileRequestOpen(TRANSFER_INFO_FILE, fs))
	{
		cout << "ERROR: Connecting to server, file " << TRANSFER_INFO_FILE << " failed to open." << endl;
		return false;
	}

	if (!FileUtils::readFromFile(fs, transferInfoBuffer, FILE_BUFFER_SIZE))
	{
		cout << "ERROR: Connecting to server, file " << TRANSFER_INFO_FILE << " failed to be read." << endl;
		FileUtils::closeFile(fs);
		return false;
	}

	vector<string> transferInformation;
	boost::split(transferInformation, transferInfoBuffer, boost::is_any_of("\n"));

	if (transferInformation.size() != 3)
	{
		cout << "ERROR: Connecting to server, transfer information not is in wrong pattern" << endl;
		FileUtils::closeFile(fs);
		return false;
	}

	vector<string> serverInformation;
	boost::split(serverInformation, transferInformation[0], boost::is_any_of(":"));

	if (serverInformation.size() != 2)
	{
		cout << "ERROR: Connecting to server, server connection information is in wrong pattern" << endl;
		FileUtils::closeFile(fs);
		return false;
	}

	uint8_t port;

	try
	{
		port = atoi(serverInformation[1].c_str());
	}
	catch(exception e)
	{
		cout << "ERROR: Connecting to server, given port is not a number" << e.what() << endl;
		FileUtils::closeFile(fs);
		return false;
	}

	// Server information is IP:PORT
	this->_clientSocketHandler = new ClientSocketHandler(serverInformation[0], port);

	this->_userName = serverInformation[1];
	this->_fileFullPath = serverInformation[2];

	cout << "SUCCESS: Transfer information read successfully!" << endl;

	FileUtils::closeFile(fs);
	return true;
}
