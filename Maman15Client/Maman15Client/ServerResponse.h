#pragma once
#include "CommonPayload.h"

/*
	A data struct to represent the request to the server
*/
struct ServerResponse
{
#pragma pack(push, 1)      // ServerResponseHeader is copied to buffer to send on socket. Hence, should be aligned to 1.
	struct ServerResponseHeader
	{
		uint32_t userId;
		uint8_t  version;
		uint8_t  op;
		ServerResponseHeader() : userId(0), version(0), op(0) {}
	};
#pragma pack(pop)

	enum ResponseStatus
	{
		REGISTRATION_SUCCESS = 200,
		REGISTRATION_FAILED = 301,
		CLIENT_AES_KEY = 201,
		LAST_SENT_FILE_CRC = 202
	};

	ServerResponseHeader header;
	uint16_t nameLen;
	uint8_t* name;
	CommonPayload payload;
	ServerResponse();
	ServerResponse(const uint8_t* const buffer, const uint32_t size);
	~ServerResponse();
	uint32_t sizeWithoutPayload() const;
};
