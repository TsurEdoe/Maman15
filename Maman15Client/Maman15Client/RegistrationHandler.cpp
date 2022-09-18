#include "RegistrationHandler.h"

RegistrationHandler::RegistrationHandler(ClientSocketHandler* clientSocketHandler) : _clientSocketHandler {clientSocketHandler} {}

bool RegistrationHandler::registerClient(string clientUserName)
{
    uint8_t* registrationPacket = new uint8_t[];

    request->_payload.size = clientUserName.size();
	uint32_t clientNameLength = (PACKET_SIZE - request->sizeWithoutPayload());
	request->_payload.payload = new uint8_t[clientNameLength];
	memcpy_s(clientName.cstr(), request->_payload.payload, clientNameLength)
    
	request->_status = ClientRequest::RequestType.CLIENT_REGISTRATION;
	request->serializeIntoBuffer(buffer);

	if (!_clientSocketHandler.send(buffer))
    {
        cout << "ERROR: RegistrationHandler - Request sending on socket failed!" << endl;
		delete request->_payload.payload;
        request->_payload.payload = NULL;
        delete request;
		request = NULL;
		return false;
    }

    uint8_t buffer[PACKET_SIZE];
    ServerResponse* serverResponse = NULL;

    if (!_clientSocketHandler.receive(buffer))
    {
        err << "ERROR: RegistrationHandler - Failed to receive first message from server!" << endl;
        return false;
    }

    serverResponse = new ServerResponse(buffer, PACKET_SIZE);
    
    if (serverResponse->)

    delete serverResponse;

    return true;
}
