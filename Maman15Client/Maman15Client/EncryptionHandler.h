#pragma once
#include "RSAWrapper.h"
#include "ClientSocketHandler.h"
#include "AESWrapper.h"
#include "ClientRequest.h"
#include "ServerResponse.h"

#define ENCRYPTED_SHARED_KEY_LENGTH 128
#define USER_NAME_SIZE 255

/*
    Handles all encryption needs for client operations:
    1. Handles key exchanging with server
    2. Sends encrypted data to the server
*/
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
    string encryptedFileData(uint8_t* dataToEncrypt, uint32_t dataToEncryptLengthe);
    
    static void padStringWithZeroes(string& strToPad, uint32_t wantedSize);
};