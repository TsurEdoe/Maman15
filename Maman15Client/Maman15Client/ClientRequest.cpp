#pragma once
#include "ClientRequest.h"

/*
	Default C'tor
*/
ClientRequest::ClientRequest() : _clientId{ }, _version(CLIENT_VERSION), _code(0) {}

ClientRequest::ClientRequest(uint8_t* clientId, uint16_t code, size_t payloadSize, uint8_t* payload) :
	_clientId{ }, _version(CLIENT_VERSION), _code(0)
{
	if (clientId == NULL)
	{
		cout << "ClientRequest - Failed creating ClientRequest, bad parameters sent" << endl;
		return;
	}

	memcpy(this->_clientId, clientId, UUID_LENGTH);
	
	this->_code = code;
	this->_payload.size = payloadSize;
	this->_payload.payload = (payloadSize > 0) ? new uint8_t[payloadSize] : NULL;

	if (this->_payload.payload)
	{
		memcpy(this->_payload.payload, payload, payloadSize);
	}
}

/*
	D'tor
*/
ClientRequest::~ClientRequest() {}

/*
	Calcs the header size of the current request
*/
uint32_t ClientRequest::sizeWithoutPayload() const
{
	return (sizeof(_clientId) + sizeof(_version) + sizeof(_code) + sizeof(_payload.size));
}

/*
	Calcs the total size of the current request
*/
uint32_t ClientRequest::sizeWithPayload() const
{
	return sizeWithoutPayload() + _payload.size;
}

/*
	Serializes the response into a given buffer in order to send to the client
*/
void ClientRequest::serializeIntoBuffer(uint8_t* buffer)
{
	uint8_t* ptr = buffer;
	
	memcpy(ptr, &(_clientId), sizeof(_clientId));
	ptr += sizeof(_clientId);
	
	memcpy(ptr, &(_version), sizeof(_version));
	ptr += sizeof(_version);

	memcpy(ptr, &(_code), sizeof(_code));
	ptr += sizeof(_code);

	memcpy(ptr, &(_payload.size), sizeof(_payload.size));
	ptr += sizeof(_payload.size);

	memcpy(ptr, (_payload.payload), _payload.size);
}