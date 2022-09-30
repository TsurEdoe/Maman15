#include "Base64Wrapper.h"

/*
	Encodes the argument given and returns the encoded base64 string
*/
std::string Base64Wrapper::encode(const std::string& str)
{
	std::string encoded;
	CryptoPP::StringSource ss(str, true,
		new CryptoPP::Base64Encoder(
			new CryptoPP::StringSink(encoded), false
		) // Base64Encoder
	); // StringSource

	return encoded;
}

/*
	Decodes the argument given and returns the decoded plain string
*/
std::string Base64Wrapper::decode(const std::string& str)
{
	std::string decoded;
	CryptoPP::StringSource ss(str, true,
		new CryptoPP::Base64Decoder(
			new CryptoPP::StringSink(decoded)
		) // Base64Decoder
	); // StringSource

	return decoded;
}

std::string Base64Wrapper::hex(const std::vector<uint8_t>& v)
{
	std::string to;
	boost::algorithm::hex(v.begin(), v.end(), std::back_inserter(to));
	return to;
}

std::vector<uint8_t> Base64Wrapper::unhex(const std::string& hex)
{
	std::vector<uint8_t> bytes;
	boost::algorithm::unhex(hex, std::back_inserter(bytes));
	return bytes;
}
