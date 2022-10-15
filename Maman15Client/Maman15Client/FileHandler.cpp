#include "FileHandler.h"

/*
	C'tor
*/
FileHandler::FileHandler(EncryptionHandler* encryptionHandler, ClientSocketHandler* clientSocketHandler) : 
	_encryptionHandler(encryptionHandler), _clientSocketHandler(clientSocketHandler) { }

/*
	Sends an encrypted file message to the server
*/
bool FileHandler::sendEncryptedFile(string fileName, uint8_t* clientUUID)
{
	string paddedFileName = fileName;
	EncryptionHandler::padStringWithZeroes(paddedFileName, FILE_NAME_FIELD_SIZE);

	uint32_t fileSize = validateFileToSend(fileName);
	if (fileSize == 0)
	{
		return false;
	}

	uint8_t* dataFromFile = readDataFromFile(fileName);
	if (dataFromFile == NULL)
	{
		cout << "FileHandler - Failed sending encrypted file " << fileName << " to server: Failed reading file content" << endl;
		return false;
	}

	string encryptedFileData = _encryptionHandler->encryptedFileData(dataFromFile, fileSize);
	fileSize = encryptedFileData.size();

	delete dataFromFile;
	dataFromFile = NULL;
	
	if (encryptedFileData.empty())
	{
		cout << "FileHandler - Failed sending encrypted file " << fileName << " to server: Failed encrypting file" << endl;
		return false;
	}

	delete dataFromFile;
	dataFromFile = NULL;

	size_t dataToSendPlainSize = UUID_LENGTH + FILE_CONTENT_FIELD_SIZE + FILE_NAME_FIELD_SIZE;

	uint8_t* payload = new uint8_t[dataToSendPlainSize + fileSize];

	memcpy(payload, clientUUID, UUID_LENGTH);
	memcpy(payload + UUID_LENGTH, &fileSize, sizeof(uint32_t));
	memcpy(payload + UUID_LENGTH + FILE_CONTENT_FIELD_SIZE, paddedFileName.c_str(), FILE_NAME_FIELD_SIZE);
	memcpy(payload + UUID_LENGTH + FILE_CONTENT_FIELD_SIZE + FILE_NAME_FIELD_SIZE, encryptedFileData.c_str(), fileSize);

	ClientRequest sendEncryptedDataToServer(clientUUID, ClientRequest::CLIENT_ENCRYPTED_FILE, dataToSendPlainSize + fileSize, payload);
	
	uint8_t* buffer = new uint8_t[sendEncryptedDataToServer.sizeWithPayload()];
	
	sendEncryptedDataToServer.serializeIntoBuffer(buffer);

	bool sendResult = _clientSocketHandler->send(buffer, sendEncryptedDataToServer.sizeWithPayload());

	cout << "FileHandler - " << (sendResult ? "Successully sent encrypted file " : "ERROR: Failed sending encrypted file ") << fileName << " to server" << endl;

	delete[] payload;
	payload = NULL;
	delete[] buffer;
	buffer = NULL;
	
	return true;
}

/*
	Validates the file that will be sent. Returns the file size if valid, 0 otherwise
*/
uint32_t FileHandler::validateFileToSend(string fileName)
{
	if (!FileUtils::doesFileExist(fileName))
	{
		cout << "FileHandler - File " << fileName << " doesn't exist." << endl;
		return 0;
	}

	fstream fileToSend;
	if (!FileUtils::fileRequestOpen(fileName, fileToSend))
	{
		cout << "FileHandler - ERROR: Failed opening file " << fileName << endl;
		return 0;
	}

	uint32_t fileSize = FileUtils::calculateFileSize(fileToSend);

	if (!FileUtils::closeFile(fileToSend))
	{
		cout << "FileHandler - ERROR: Failed closing file " << fileName << endl;
		return 0;
	}

	return fileSize;
}

/*
	Reads content from a file
*/
uint8_t* FileHandler::readDataFromFile(string fileName)
{
	if (!FileUtils::doesFileExist(fileName))
	{
		cout << "FileHandler - ERROR: File " << fileName << " doesn't exist." << endl;
		return NULL;
	}

	fstream fileToSend;
	if (!FileUtils::fileRequestOpen(fileName, fileToSend))
	{
		cout << "FileHandler - ERROR: Failed opening file " << fileName << endl;
		return NULL;
	}

	uint32_t fileSize = FileUtils::calculateFileSize(fileToSend);

	uint8_t* dataFromFileToSend = new uint8_t[fileSize];

	if (!FileUtils::readFromFile(fileToSend, dataFromFileToSend, fileSize))
	{
		cout << "FileHandler - ERROR: Failed reading from file " << fileName << endl;
		delete[] dataFromFileToSend;
		return NULL;
	}

	if (!FileUtils::closeFile(fileToSend))
	{
		cout << "FileHandler - ERROR: Failed closing file " << fileName << endl;
		delete[] dataFromFileToSend;
		return NULL;
	}

	return dataFromFileToSend;
}

/*
	Calculated checksum (CRC) of a given file content
*/
uint32_t FileHandler::calculateFileCRC(string fileName)
{
	cout << "FileHandler - Calculating CRC on " << fileName << endl;
	size_t fileSize = validateFileToSend(fileName);
	if (fileSize == 0)
	{
		cout << "FileHandler - ERROR: Failed calculating file " << fileName << " CRC." << endl;
		return 0;
	}

	uint8_t* fileData = this->readDataFromFile(fileName);
	
	boost::crc_32_type result;
	
	result.process_bytes(fileData, fileSize);

	uint32_t fileCRC = result.checksum();

	cout << "FileHandler- " << fileName << "'s CRC is " << fileCRC << endl;
	return fileCRC;
}