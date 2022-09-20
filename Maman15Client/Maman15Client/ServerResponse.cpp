#pragma once
#include <string>
#include "ServerResponse.h"
#include <stdexcept>

/*
	C'tor
*/
ServerResponse::ServerResponse() {}

/*
	D'tor
*/
ServerResponse::~ServerResponse() {}

/*
   Deserialize raw data into response.
 */
ServerResponse::ServerResponse(const uint8_t* const buffer, const uint32_t size)
{
	uint32_t bytesRead = 0;
	const uint8_t* ptr = buffer;

	// Fill minimal header
	memcpy(&(this->header), ptr, sizeof(ServerResponse::ServerResponseHeader));
	bytesRead += sizeof(ServerResponse::ServerResponseHeader);
	ptr += sizeof(ServerResponse::ServerResponseHeader);
	if (bytesRead + sizeof(uint16_t) > size)
	{
		return;  // return the response with minimal header.
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
	return (sizeof(header) + sizeof(payload.size));
}