#pragma once

#include <osrng.h>
#include <rsa.h>

#include <string>

#define BITS 1024
#define KEYSIZE  160

class RSAWrapper
{
private:
	CryptoPP::AutoSeededRandomPool _rng;
	CryptoPP::RSA::PrivateKey _privateKey;
	CryptoPP::RSA::PublicKey _publicKey;

public:
	RSAWrapper();
	~RSAWrapper();

	std::string getPrivateKey() const;
	char* getPrivateKey(char* keyout, unsigned int length) const;

	std::string getPublicKey() const;
	char* getPublicKey(char* keyout, unsigned int length) const;

	std::string decrypt(const std::string& cipher);
	std::string decrypt(const char* cipher, unsigned int length);

	std::string encrypt(const std::string& plain);
	std::string encrypt(const char* plain, unsigned int length);
};
