#pragma once
#include <boost/asio.hpp>
#include "ClientInitializer.h"
#include "EncryptionHandler.h"
#include "FileHandler.h"
#include "MainClient.h"

#define MAX_RETRIES_FILE_RESEND 3

/*
    Main client class to run the client operation
*/
class MainClient
{
private:
    ClientSocketHandler* _clientSocketHandler;
    RSAWrapper* _rsaWrapper;
    uint8_t* _clientUUID;
    string _userName;
    EncryptionHandler* _encryptionHandler;
    FileHandler* _fileHandler;
    bool sendFileToServer(string fileName);
    uint32_t getServerCalculatedCRC();
    bool handleCRCValidation(uint32_t serverCalculatedCRC, uint32_t clientCalculatedCRC, string fileName);

public:
    MainClient(ClientSocketHandler* clientSocketHandler, RSAWrapper* rsaWrapper, uint8_t* clientUUID, string userName);
    ~MainClient();
    bool runClient(string fileName);
};