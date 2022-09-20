#pragma once
#include "RSAWrapper.h"
#include "ClientSocketHandler.h"
#include "AESWrapper.h"
#include "ClientRequest.h"
#include "ServerResponse.h"

class EncryptionHandler
{
private:
    RSAWrapper* _rsaWrapper;
    ClientSocketHandler* _clientSocketHandler;
    AESWrapper* _aesWrapper;

    bool sendPublicKeyToServer(uint8_t* clientUUID, string userName);
    bool receiveSharedSecret();
public:
    EncryptionHandler(RSAWrapper* rsaWrapper, ClientSocketHandler* clientSocketHandler);
    bool initializeHandler(uint8_t* clientUUID, string userName);

    AESWrapper* getAESEncryptionWithServer();
}