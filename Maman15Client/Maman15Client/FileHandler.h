#include "ClientSocketHandler.h"
#include "EncryptionHandler.h"
#include "FileUtils.h"

/*
        6. encrypt file data and send to server
        7. receive CRC and compare with local calculation
            a. if not the same go back to stage 5 (max 3 tries)
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
}