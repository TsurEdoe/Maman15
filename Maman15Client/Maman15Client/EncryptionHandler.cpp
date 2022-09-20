#include "EncryptionHandler.h"

EncryptionHandler::EncryptionHandler(RSAWrapper* rsaWrapper, ClientSocketHandler* clientSocketHandler) :
    _rsaWrapper(rsaWrapper), _clientSocketHandler(clientSocketHandler) , _aesWrapper(NULL) { }

bool EncryptionHandler::sendPublicKeyToServer(uint8_t* clientUUID, string userName)
{
    uint8_t buffer[PACKET_SIZE];
    memset(buffer, 0, PACKET_SIZE);

    string publicKey = this->_rsaWrapper->getPublicKey();
    ClientRequest sendPublicKeyRequest;
    memcpy(sendPublicKeyRequest._clientId, clientUUID, UUID_LENGTH);
    sendPublicKeyRequest._code = ClientRequest::CLIENT_PUBLIC_KEY;
    memcpy(sendPublicKeyRequest._payload.payload, userName.c_str(), userName.size());

    sendPublicKeyRequest.serializeIntoBuffer(buffer);

    return _clientSocketHandler->send(buffer);
}


bool EncryptionHandler::receiveSharedSecret()
{
    uint8_t buffer[PACKET_SIZE];
    memset(buffer, 0, PACKET_SIZE);

    if (!_clientSocketHandler->receive(buffer))
    {
        cout << "ERROR: RegistrationHandler - Failed to receive registration response from server!" << endl;
        return NULL;
    }

    ServerResponse sharedKeyResponse(buffer, sizeof(ServerResponse::ServerResponseHeader) + UUID_LENGTH);

    if (sharedKeyResponse.header._code == ServerResponse::CLIENT_AES_KEY)
    {
        string decrypteSharedKey = _rsaWrapper->decrypt((const char*)sharedKeyResponse.payload.payload, sharedKeyResponse.payload.size);
        _aesWrapper = new AESWrapper((const unsigned char*)decrypteSharedKey.c_str(), decrypteSharedKey.size());

        cout << "Initialized encryption handler successfully" << endl;
        
        return true;
    }

    cout << "Failed initizlizing encryption handler, bad response " << sharedKeyResponse.header._code << " type received from server" << endl;

    return false;
}

bool EncryptionHandler::initializeHandler(uint8_t* clientUUID, string userName)
{
    return sendPublicKeyToServer(clientUUID, userName) && receiveSharedSecret();
}

AESWrapper* EncryptionHandler::getAESEncryptionWithServer()
{
    return _aesWrapper;
}