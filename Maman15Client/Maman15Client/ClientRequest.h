#pragma once
#include "CommonPayload.h"

#define CLIENT_VERSION 3
#define PACKET_SIZE 1024

/*
	A data struct to represent the request to the server
*/
struct ClientRequest
{
	enum RequestType
	{
		CLIENT_REGISTRATION = 101,
		CLIENT_PUBLIC_KEY = 102,
		CLIENT_ENCRYPTED_FILE = 103,
		FILE_CRC_FAILED = 104,
		FILE_CRC_SUCCESS = 105
	};

	const uint8_t _version;
	uint16_t _status;
	uint16_t _nameLen;
	uint8_t* _name;
	CommonPayload _payload;

	ClientRequest();
	~ClientRequest();
	uint32_t sizeWithoutPayload() const;
	void serializeIntoBuffer(uint8_t* buffer);
};