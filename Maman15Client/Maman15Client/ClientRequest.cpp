#pragma once
#include "ClientRequest.h"

/*
	Default C'tor
*/
ClientRequest::ClientRequest() : _clientId({ 0 }), _version(CLIENT_VERSION), _code(0) {}

ClientRequest::ClientRequest(uint8_t* clientId, uint16_t code, size_t payloadSize, uint8_t* payload, const uint8_t version = CLIENT_VERSION) :
	_clientId({ 0 }), _version(CLIENT_VERSION), _code(0)
{
	if (clientId == NULL || version == -1 || payloadSize == 0 || payload == NULL)
	{
		cout << "Failed creating ClientRequest, bad parameters sent" << endl;
		return;
	}

	memcpy(this->_clientId, clientId, UUID_LENGTH);
	
	this->_code = code;
	this->_payload.size = payloadSize;
	this->_payload.payload = new uint8_t[payloadSize];

	memcpy(this->_payload.payload, payload, payloadSize);
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
	Serializes the response into a given buffer in order to send to the client
*/
void ClientRequest::serializeIntoBuffer(uint8_t* buffer)
{
	uint8_t* ptr = buffer;
	size_t payloadSize = (PACKET_SIZE - sizeWithoutPayload());
	if (_payload.size < payloadSize)
	{
		payloadSize = _payload.size;
	}
	
	memcpy(ptr, &(_clientId), sizeof(_clientId));
	ptr += sizeof(_clientId);
	
	memcpy(ptr, &(_version), sizeof(_version));
	ptr += sizeof(_version);

	memcpy(ptr, &(_code), sizeof(_code));
	ptr += sizeof(_code);

	memcpy(ptr, &(_payload.size), sizeof(_payload.size));
	ptr += sizeof(_payload.size);

	memcpy(ptr, (_payload.payload), payloadSize);
}