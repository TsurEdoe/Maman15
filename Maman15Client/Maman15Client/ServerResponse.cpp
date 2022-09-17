#pragma once
#include <string>
#include "ServerResponse.h"
#include <stdexcept>

/*
	C'tor
*/
ServerResponse::ServerResponse() : nameLen(0), name(NULL) {}

/*
	D'tor
*/
ServerResponse::~ServerResponse()
{
	if (name != NULL) 
	{
		delete[] name;
		name = NULL;
	}
}

/*
   Deserialize raw data into response.
 */
ServerResponse::ServerResponse(const uint8_t* const buffer, const uint32_t size)
{
	uint32_t bytesRead = 0;
	const uint8_t* ptr = buffer;
	name = NULL;
	nameLen = 0;

	// Fill minimal header
	memcpy(&(this->header), ptr, sizeof(ServerResponse::ServerResponseHeader));
	bytesRead += sizeof(ServerResponse::ServerResponseHeader);
	ptr += sizeof(ServerResponse::ServerResponseHeader);
	if (bytesRead + sizeof(uint16_t) > size)
	{
		return;  // return the response with minimal header.
	}

	// Copy name length
	memcpy(&(this->nameLen), ptr, sizeof(uint16_t));
	bytesRead += sizeof(uint16_t);
	ptr += sizeof(uint16_t);
	if ((this->nameLen == 0) || ((bytesRead + this->nameLen) > size))
	{
		return; // name length invalid.
	}  

	this->name = new uint8_t[this->nameLen + 1];
	memcpy(this->name, ptr, this->nameLen);
	this->name[this->nameLen] = '\0';
	bytesRead += this->nameLen;
	ptr += this->nameLen;

	if (bytesRead + sizeof(uint32_t) > size)
	{
		return;
	}

	// copy payload size
	memcpy(&(this->payload.size), ptr, sizeof(uint32_t));
	bytesRead += sizeof(uint32_t);
	ptr += sizeof(uint32_t);
	if (this->payload.size == 0)
	{
		return;  // payload length invalid.
	}
	
	// copy payload until size limit.
	uint32_t leftover = size - bytesRead;
	if (this->payload.size < leftover)
	{
		leftover = this->payload.size;
	}

	this->payload.payload = new uint8_t[leftover];
	memcpy(this->payload.payload, ptr, leftover);
}

/*
	Calcs the header size of the current response
*/
uint32_t ServerResponse::sizeWithoutPayload() const
{
	return (sizeof(header) + sizeof(nameLen) + nameLen + sizeof(payload.size));
}