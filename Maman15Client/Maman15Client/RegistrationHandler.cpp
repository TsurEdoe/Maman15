#include "RegistrationHandler.h"

RegistrationHandler::RegistrationHandler(ClientSocketHandler* clientSocketHandler) : _clientSocketHandler {clientSocketHandler} {}

bool RegistrationHandler::registerClient(string clientUserName)
{
    ClientRequest request;

    request->_payload.size = clientUserName.size();
	uint32_t clientNameLength = (PACKET_SIZE - request->sizeWithoutPayload());
	request->_payload.payload = new uint8_t[clientNameLength];
	memcpy_s(clientName.cstr(), request->_payload.payload, clientNameLength)
    
	request->_status = ClientRequest::RequestType.CLIENT_REGISTRATION;
	request->serializeIntoBuffer(buffer);

	if (!_clientSocketHandler.send(buffer))
    {
        cout << "ERROR: Request sending on socket failed!" << endl;
		delete request;
		request = NULL;
		return false;
    }
}