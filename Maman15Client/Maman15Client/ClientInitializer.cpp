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
	this->_fileName = "";
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

/*
	Getter
*/
ClientSocketHandler* ClientInitializer::getClientSocketHandler()
{
	return this->_clientSocketHandler;
}

/*
	Getter
*/
RSAWrapper* ClientInitializer::getRSAWrapper()
{
	return this->_rsaWrapper;
}

/*
	Getter
*/
uint8_t* ClientInitializer::getClientUUID()
{
	return this->_clientUUID;
}

/*
	Getter
*/
string ClientInitializer::getUserName()
{
	return this->_userName;
}

/*
	Getter
*/
string ClientInitializer::getFileName()
{
	return this->_fileName;
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

	// Found me.info
	if (FileUtils::doesFileExist(CLIENT_CONNECTION_INFO_FILE))
	{
		cout << "Found " << CLIENT_CONNECTION_INFO_FILE << " skipping registration." << endl;
		return getClientConnectionInfo();
	}
	// Registers client
	else
	{
		cout << CLIENT_CONNECTION_INFO_FILE << " not found, going to registration." << endl;
		bool registrationResult = _registrationHandler->registerClient(this->_userName, _clientUUID);

		if (!registrationResult || this->_clientUUID == NULL)
		{
			cout << "Error initializng client, failed registering with server" << endl;
			return false;
		}

		this->_rsaWrapper = new RSAWrapper();
		
		cout << "Client " << this->_userName << " registered, writing client connection information to " << CLIENT_CONNECTION_INFO_FILE << " file." << endl;
		
		// Parses client information
		string clientConnectionInfo[3];
		clientConnectionInfo[0] = this->_userName;
		std::vector<uint8_t> uuidVector(std::begin(_clientUUID), std::end(_clientUUID));
		clientConnectionInfo[1] = Base64Wrapper::hex(uuidVector);
		clientConnectionInfo[2] = Base64Wrapper::encode(this->_rsaWrapper->getPrivateKey());
		
		string clientConnectionInfoBuffer = boost::algorithm::join(clientConnectionInfo, "\n");
		
		// Writing client information into me.info file
		fstream fs;
		if (!FileUtils::fileRequestOpen(CLIENT_CONNECTION_INFO_FILE, fs, true))
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

/*
	Reads lines from a given file and returns the information as a vector of strings
*/
vector<string> ClientInitializer::readInformationFile(string fileName)
{
	vector<string> fileInformation;
	uint8_t* fileInfoBuffer = NULL;

	fstream fs;
	if (!FileUtils::fileRequestOpen(fileName, fs))
	{
		cout << "ERROR: Connecting to server, file " << fileName << " failed to open." << endl;
		return fileInformation;
	}

	uint32_t informationFileSize = FileUtils::calculateFileSize(fs);
	if (informationFileSize == 0)
	{
		cout << "ERROR: Connecting to server, failed calculating file size on " << fileName << endl;
		return fileInformation;
	}

	fileInfoBuffer = new uint8_t[informationFileSize];

	if (!FileUtils::readFromFile(fs, fileInfoBuffer, informationFileSize))
	{
		cout << "ERROR: Connecting to server, file " << fileName << " failed to be read." << endl;
		FileUtils::closeFile(fs);
		delete[] fileInfoBuffer;
		return fileInformation;
	}

	FileUtils::closeFile(fs);

	std::string fileInfoBufferString((const char *)fileInfoBuffer);

	boost::split(fileInformation, fileInfoBufferString, boost::is_any_of("\n"));
	
	string lastLine = fileInformation[fileInformation.size() - 1];
	// Remove last empty line
	if (lastLine.size() == 0)
	{
		fileInformation.pop_back();
	}

	delete[] fileInfoBuffer;
	return fileInformation;
}

/*
	Reads me.info and initializes the rsaWrapper of the client initializer
*/
bool ClientInitializer::getClientConnectionInfo()
{
	vector<string> clientConnectionInfo = ClientInitializer::readInformationFile(CLIENT_CONNECTION_INFO_FILE);

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

	vector<uint8_t> clientUUID = Base64Wrapper::unhex(clientConnectionInfo[1]);
	
	if (clientUUID.size() > UUID_LENGTH)
	{
		cout << "UUID in " << CLIENT_CONNECTION_INFO_FILE << " file is too long" << endl;
		return false;
	}

	std::copy(clientUUID.begin(), clientUUID.end(), this->_clientUUID);

	this->_rsaWrapper = new RSAWrapper(Base64Wrapper::decode(clientConnectionInfo[2]));

	cout << "SUCCESS: Client connection information read successfully!" << endl;

	return true;
}

/*
	Reads transfer.info and initializes the communication to the server
*/
bool ClientInitializer::getTransferInformation()
{
	vector<string> transferInformation = ClientInitializer::readInformationFile(TRANSFER_INFO_FILE);

	if (transferInformation.size() != 3)
	{
		cout << "ERROR: ClientInitializer - Connecting to server, transfer information not is in wrong pattern." << endl;
		return false;
	}

	vector<string> serverInformation;
	boost::split(serverInformation, transferInformation[0], boost::is_any_of(":"));

	if (serverInformation.size() != 2)
	{
		cout << "ERROR: ClientInitializer - Connecting to server, server connection information is in wrong pattern" << endl;
		return false;
	}

	uint16_t port;

	try
	{
		port = atoi(serverInformation[1].c_str());
	}
	catch(exception e)
	{
		cout << "ERROR: ClientInitializer - Connecting to server, given port is not a number" << e.what() << endl;
		return false;
	}

	try
	{
		// Server information is IP:PORT
		this->_clientSocketHandler = new ClientSocketHandler(serverInformation[0], port);
	}
	catch (exception e)
	{
		cout << "ERROR: ClientSocketHandler - Failed connection to server: " << e.what() << endl;
		return false;
	}

	if (!this->_clientSocketHandler->isConnected())
	{
		cout << "ERROR: ClientInitializer - Connecting to server, socket wasn't opened" << endl;
		return false;
	}

	this->_userName = transferInformation[1];
	this->_fileName = transferInformation[2];

	cout << "SUCCESS: ClientInitializer - Transfer information read successfully!" << endl;
	return true;
}
