#include "ClientSocketHandler.h"
#include "EncryptionHandler.h"
#include "FileUtils.h"
#include <boost/crc.hpp>

#define CHECKSUM_SIZE 4

/*
    Handles file operations for client:
    1. Sends an encrypted file using the encryption handler
    2. Calculates file CRC
*/
class FileHandler
{
private:
    EncryptionHandler* _encryptionHandler;

    uint8_t* readDataFromFile(string fileFullPath);
    uint32_t validateFileToSend(string fileFullPath);

public:
    FileHandler(EncryptionHandler* encryptionHandler);
    bool sendEncryptedFile(string fileFullPath, uint8_t* clientUUID);
    uint32_t calculateFileCRC(string fileFullPath);
};