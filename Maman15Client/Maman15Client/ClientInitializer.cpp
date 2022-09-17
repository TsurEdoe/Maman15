#include "ClientInitializer.h"

/*
    C'tor
*/
ClientInitializer::ClientInitializer(ClientSocketHandler* clientSocketHandler) : _clientSocketHandler { clientSocketHandler }, _registrationHandler(clientSocketHandler) {}

/*
    Initializes the client and returns the communication channel opened with the server
*/
ClientSocketHandler* ClientInitializer::initializeClient()
{
	if (!getTransferInformation())
	{
		cout << "ERROR: Failed initializing client!" << endl;
		return nullptr;
	}

	if (FileUtils::doesFileExist(CLIENT_CONNECTION_INFO_FILE))
	{
		cout << "Found " << CLIENT_CONNECTION_INFO_FILE << " skipping registration." << endl;
	}
	else
	{
		cout << CLIENT_CONNECTION_INFO_FILE << " not found, going to registration." << endl;
		_registrationHandler.registerClient(this->_userName);
	}
}

/*
	
*/
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
		fs.close();
		return false;
	}

	vector<string> transferInformation;
	boost::split(transferInformation, transferInfoBuffer, boost::is_any_of("\n"));

	if (transferInformation.size() != 3)
	{
		cout << "ERROR: Connecting to server, transfer information not is in wrong pattern" << endl;
		fs.close();
		return false;
	}

	vector<string> serverInformation;
	boost::split(serverInformation, transferInformation[0], boost::is_any_of(":"));

	if (serverInformation.size() != 2)
	{
		cout << "ERROR: Connecting to server, server connection information is in wrong pattern" << endl;
		fs.close();
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
		fs.close();
		return false;
	}

	// Server information is IP:PORT
	this->_clientSocketHandler = new ClientSocketHandler(serverInformation[0], port);

	this->_userName = serverInformation[1];
	this->_fileFullPath = serverInformation[2];

	cout << "SUCCESS: Transfer information read successfully!" << endl;

	fs.close();
	return true;
}
