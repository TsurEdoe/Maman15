#pragma once
#include <osrng.h>
#include <rsa.h>
#include <string>

class RSAPublicWrapper
{
public:
	static const unsigned int KEYSIZE = 160;
	static const unsigned int BITS = 1024;

private:
	CryptoPP::AutoSeededRandomPool _rng;
	CryptoPP::RSA::PublicKey _publicKey;

	RSAPublicWrapper(const RSAPublicWrapper& rsapublic);
public:

	RSAPublicWrapper(const std::string& key);
	~RSAPublicWrapper();

	std::string getPublicKey() const;

	std::string encrypt(const std::string& plain);
};

class RSAPrivateWrapper
{
public:
	static const unsigned int BITS = 1024;

private:
	CryptoPP::AutoSeededRandomPool _rng;
	CryptoPP::RSA::PrivateKey _privateKey;

	RSAPrivateWrapper(const RSAPrivateWrapper& rsaprivate);
public:
	RSAPrivateWrapper();
	RSAPrivateWrapper(const std::string& key);
	~RSAPrivateWrapper();

	std::string getPrivateKey() const;

	std::string getPublicKey() const;

	std::string decrypt(const char* cipher, unsigned int length);
};

class RSAWrapper
{
private:
	RSAPrivateWrapper* _privateWrapper;
	RSAPublicWrapper* _publicWrapper;
public:
	RSAWrapper();
	RSAWrapper(const std::string& key);
	~RSAWrapper();

	std::string getPublicKey() const;
	
	std::string getPrivateKey() const;

	std::string encrypt(const std::string& plain);

	std::string decrypt(const char* cipher, unsigned int length);
};