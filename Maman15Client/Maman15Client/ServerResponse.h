#pragma once
#include "CommonPayload.h"

/*
	A data struct to represent the request from the client
*/
struct ServerResponse
{
#pragma pack(push, 1)      // ServerResponseHeader is copied to buffer to send on socket. Hence, should be aligned to 1.
	struct ServerResponseHeader
	{
		uint8_t _version;
		uint16_t _code;
		ServerResponseHeader() : _version(0), _code(0) {}
	};
#pragma pack(pop)

	enum ResponseType
	{
		REGISTRATION_SUCCESS = 2100,
		CLIENT_AES_KEY = 2102,
		LAST_SENT_FILE_CRC = 2103,
		REQUEST_RECEIVED = 2104
	};

	ServerResponseHeader header;
	CommonPayload payload;
	ServerResponse();
	ServerResponse(const uint8_t* const buffer, const uint32_t size);
	~ServerResponse();
	uint32_t sizeWithoutPayload() const;
};
