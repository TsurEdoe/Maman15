#include "ClientSocketHandler.h"
#include "EncryptionHandler.h"
#include "FileUtils.h"
#include <boost/crc.hpp>

#define CHECKSUM_FIELD_SIZE 4
#define FILE_CONTENT_FIELD_SIZE 4
#define FILE_NAME_FIELD_SIZE 255

/*
    Handles file operations for client:
    1. Sends an encrypted file using the encryption handler
    2. Calculates file CRC
*/
class FileHandler
{
private:
    EncryptionHandler* _encryptionHandler;
    ClientSocketHandler* _clientSocketHandler;

    uint8_t* readDataFromFile(string fileName);
    uint32_t validateFileToSend(string fileName);

public:
    FileHandler(EncryptionHandler* encryptionHandler, ClientSocketHandler* clientSocketHandler);
    bool sendEncryptedFile(string fileName, uint8_t* clientUUID);
    uint32_t calculateFileCRC(string fileName);
};