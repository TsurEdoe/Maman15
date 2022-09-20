#pragma once
#include <string>
#include <iostream>
#include "CommonPayload.h"

using namespace std;

#define CLIENT_VERSION 3
#define PACKET_SIZE 1024
#define UUID_LENGTH 16

/*
	A data struct to represent the request to the server
*/
struct ClientRequest
{
	enum RequestCode
	{
		CLIENT_REGISTRATION = 1100,
		CLIENT_PUBLIC_KEY = 1101,
		CLIENT_ENCRYPTED_FILE = 1103,
		FILE_CRC_SUCCESS = 1104,
		FILE_CRC_FAILED_RESENDING = 1105,
		FILE_CRC_FAILED_FINISHED = 1106
	};

	uint8_t _clientId[UUID_LENGTH];
	const uint8_t _version;
	uint16_t _code;
	CommonPayload _payload;

	ClientRequest();
	ClientRequest(uint8_t* clientId, uint16_t code, size_t payloadSize, uint8_t* payload, const uint8_t version = CLIENT_VERSION);
	~ClientRequest();
	uint32_t sizeWithoutPayload() const;
	void serializeIntoBuffer(uint8_t* buffer);
};