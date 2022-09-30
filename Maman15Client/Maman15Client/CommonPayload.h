#pragma once
#include <cstdint>

#define UUID_LENGTH 16

/*
	Common for Request & Response.
*/
struct CommonPayload
{
	uint32_t size;
	uint8_t* payload; 
	CommonPayload();
	~CommonPayload();
};