#pragma once
#include <string>
#include "ClientRequest.h"

/*
	Default C'tor
*/
ClientRequest::ClientRequest() : _version(SERVER_VERSION), _status(0), _nameLen(0), _name(NULL) {}

/*
	D'tor
*/
ClientRequest::~ClientRequest()
{
	if (_name != NULL)
	{
		delete[] _name;
		_name = NULL;
	}
}

/*
	Calcs the header size of the current request
*/
uint32_t ClientRequest::sizeWithoutPayload() const
{
	return (sizeof(_version) + sizeof(_status) + sizeof(_nameLen) + _name + sizeof(_payload.size)); 
}

/*
	Serializes the request into a given buffer in order to send to the client
*/
void ClientRequest::serializeIntoBuffer(uint8_t* buffer)
{
	uint8_t* ptr = buffer;
	size_t payloadSize = (PACKET_SIZE - sizeWithoutPayload());
	if (_payload.size < payloadSize)
	{
		payloadSize = _payload.size;
	}

	memcpy(ptr, &(_version), sizeof(_version));
	ptr += sizeof(_version);
	memcpy(ptr, &(_status), sizeof(_status));
	ptr += sizeof(_status);
	memcpy(ptr, &(_nameLen), sizeof(_nameLen));
	ptr += sizeof(_nameLen);
	memcpy(ptr, (_name), _nameLen);
	ptr += _nameLen;
	memcpy(ptr, &(_payload.size), sizeof(_payload.size));
	ptr += sizeof(_payload.size);
	memcpy(ptr, (_payload.payload), payloadSize);
}