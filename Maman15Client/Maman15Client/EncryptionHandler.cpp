#include "EncryptionHandler.h"

EncryptionHandler::EncryptionHandler(RSAWrapper* rsaWrapper, ClientSocketHandler* clientSocketHandler) :
    _rsaWrapper(rsaWrapper), _clientSocketHandler(clientSocketHandler) { }

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

    if (sharedKeyResponse.header._code == ServerResponse:::CLIENT_AES_KEY)
    {
        memcpy((void*)clientUUIDBuffer, (void*)registrationRespone.payload.payload, (size_t)UUID_LENGTH);
        cout << "Registration succefull, got uuid " << clientUUIDBuffer << " from server" << endl;

        return false;
    }

    cout << "Failed registraion with server: Got bad response type (" << registrationRespone.header._code << ")" << endl;

    return false;
}

bool EncryptionHandler::getSharedSecret()
{

}

AESWrapper* EncryptionHandler::getAESEncryptionWithServer()
{
    
}