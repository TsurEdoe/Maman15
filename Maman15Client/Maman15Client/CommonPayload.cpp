#include "CommonPayload.h"

/*
	C'tor
*/
CommonPayload::CommonPayload() : size(0), payload(NULL) {}

/*
	D'tor
*/
CommonPayload::~CommonPayload()
{
	if (payload != NULL)
	{
		delete[] payload;
		payload = NULL;
	}
}
