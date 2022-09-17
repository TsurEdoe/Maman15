#pragma once
#include <cstdint>

struct CommonPayload  // Common for Request & Response.
{
	size_t size;
	uint8_t* payload; 
	CommonPayload();
	~CommonPayload();
};