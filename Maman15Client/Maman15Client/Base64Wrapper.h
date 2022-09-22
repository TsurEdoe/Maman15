#pragma once

#include <string>
#include <base64.h>

/*
	Wrapper to encode and decode data using base64
*/
class Base64Wrapper
{
public:
	static std::string encode(const std::string& str);
	static std::string decode(const std::string& str);
};
