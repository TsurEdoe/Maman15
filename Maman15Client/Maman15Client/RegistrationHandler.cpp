#include "RegistrationHandler.h"

/*
    C'tor
*/
RegistrationHandler::RegistrationHandler(ClientSocketHandler* clientSocketHandler) : _clientSocketHandler{ clientSocketHandler } {}

/*
    Registers a client with the server by sending a registration request
*/
bool RegistrationHandler::registerClient(string clientUserName, uint8_t* clientUUIDBuffer)
{
    uint8_t buffer[PACKET_SIZE];
    memset(buffer, 0, PACKET_SIZE);

    ClientRequest registrationRequest(clientUUIDBuffer, ClientRequest::CLIENT_REGISTRATION, clientUserName.size(), (uint8_t*)clientUserName.c_str());
    registrationRequest.serializeIntoBuffer(buffer);
    cout << "RegistrationHandler - Sending registration request to server" << endl;

    if (!_clientSocketHandler->send(buffer, registrationRequest.sizeWithPayload()))
    {
        cout << "ERROR: RegistrationHandler - Request sending on socket failed!" << endl;
        return false;
    }

    memset(buffer, 0, PACKET_SIZE);
    cout << "RegistrationHandler - Receiving registration response from server" << endl;
    if (!_clientSocketHandler->receive(buffer, sizeof(ServerResponse::ServerResponseHeader) + sizeof(uint32_t) + UUID_LENGTH))
    {
        cout << "ERROR: RegistrationHandler - Failed to receive registration response from server!" << endl;
        return false;
    }

    ServerResponse registrationRespone(buffer, sizeof(ServerResponse::ServerResponseHeader) + sizeof(uint32_t) + UUID_LENGTH);

    if (registrationRespone.header._code == ServerResponse::REGISTRATION_SUCCESS)
    {
        memcpy((void*)clientUUIDBuffer, (void*)registrationRespone.payload.payload, (size_t)UUID_LENGTH);
        cout << "RegistrationHandler - Registration succefull, got uuid " << clientUUIDBuffer << " from server" << endl;
        
        return true;
    }

    cout << "ERROR: RegistrationHandler - Failed registraion with server: Got bad response type (" << registrationRespone.header._code << ")" << endl;

    return false;
}
