#pragma once

#include <string>
#include <base64.h>
#include <boost/algorithm/hex.hpp>

/*
	Wrapper to encode and decode data using base64
*/
class Base64Wrapper
{
public:
	static std::string encode(const std::string& str);
	static std::string decode(const std::string& str);
	static std::string hex(const std::vector<uint8_t>& v);
	static std::vector<uint8_t> unhex(const std::string& hex);
};
