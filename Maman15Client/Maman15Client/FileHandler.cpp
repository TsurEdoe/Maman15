#include "FileHandler.h"

FileHandler::FileHandler(EncryptionHandler* encryptionHandler) : _encryptionHandler(encryptionHandler) { }

bool FileHandler::sendEncryptedFile(string fileFullPath, uint8_t* clientUUID)
{
	uint32_t fileSize = validateFileToSend(fileFullPath);
	if (fileSize == 0)
	{
		return false;
	}

	uint8_t* dataToSend = readDataFromFile(fileFullPath);
	if (!this->_encryptionHandler->sendEncryptedFileData(dataToSend, fileSize, clientUUID, ClientRequest::RequestCode::CLIENT_ENCRYPTED_FILE))
	{
		cout << "Failed sending encrypted file " << fileFullPath << " to server" << endl;
		delete dataToSend;
		return false;
	}

	cout << "Successully sent encrypted file " << fileFullPath << " to server" << endl;
	delete dataToSend;
	return true;
}

uint32_t FileHandler::validateFileToSend(string fileFullPath)
{
	if (!FileUtils::doesFileExist(fileFullPath))
	{
		cout << "File " << fileFullPath << " doesn't exist." << endl;
		return 0;
	}

	fstream fileToSend;
	if (!FileUtils::fileRequestOpen(fileFullPath, fileToSend))
	{
		cout << "Failed opening file " << fileFullPath << endl;
		return 0;
	}

	uint32_t fileSize = FileUtils::calculateFileSize(fileToSend);

	if (!FileUtils::closeFile(fileToSend))
	{
		cout << "Failed closing file " << fileFullPath << endl;
		return 0;
	}

	return fileSize;
}

uint8_t* FileHandler::readDataFromFile(string fileFullPath)
{
	if (!FileUtils::doesFileExist(fileFullPath))
	{
		cout << "File " << fileFullPath << " doesn't exist." << endl;
		return NULL;
	}

	fstream fileToSend;
	if (!FileUtils::fileRequestOpen(fileFullPath, fileToSend))
	{
		cout << "Failed opening file " << fileFullPath << endl;
		return NULL;
	}

	uint32_t fileSize = FileUtils::calculateFileSize(fileToSend);

	uint8_t* dataFromFileToSend = new uint8_t[fileSize];

	if (!FileUtils::readFromFile(fileToSend, dataFromFileToSend, fileSize))
	{
		cout << "Failed reading from file " << fileFullPath << endl;
		delete dataFromFileToSend;
		return NULL;
	}

	if (!FileUtils::closeFile(fileToSend))
	{
		cout << "Failed closing file " << fileFullPath << endl;
		delete dataFromFileToSend;
		return NULL;
	}

	return dataFromFileToSend;
}

uint32_t FileHandler::calculateFileCRC(string fileFullPath)
{
	size_t fileSize = validateFileToSend(fileFullPath);
	if (fileSize == 0)
	{
		cout << "Failed calculating file " << fileFullPath << " CRC." << endl;
		return 0;
	}

	uint8_t* fileData = this->readDataFromFile(fileFullPath);
	
	boost::crc_32_type result;
	
	result.process_bytes(fileData, fileSize);

	return result.checksum();
}