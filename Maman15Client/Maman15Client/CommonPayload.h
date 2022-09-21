#pragma once
#include <cstdint>

#define UUID_LENGTH 16

struct CommonPayload  // Common for Request & Response.
{
	size_t size;
	uint8_t* payload; 
	CommonPayload();
	~CommonPayload();
};