#include "RSAWrapper.h"


RSAWrapper::RSAWrapper()
{
	this->_privateWrapper = new RSAPrivateWrapper();
	this->_publicWrapper = new RSAPublicWrapper(this->_privateWrapper->getPublicKey());
}

RSAWrapper::RSAWrapper(const std::string& key)
{
	this->_privateWrapper = new RSAPrivateWrapper(key);
	this->_publicWrapper = new RSAPublicWrapper(this->_privateWrapper->getPublicKey());
}

RSAWrapper::~RSAWrapper()
{
	delete _privateWrapper;
	_privateWrapper = NULL;

	delete _publicWrapper;
	_publicWrapper = NULL;
}

std::string RSAWrapper::getPublicKey() const
{
	return _publicWrapper->getPublicKey();
}

std::string RSAWrapper::getPrivateKey() const
{
	return _privateWrapper->getPrivateKey();
}

std::string RSAWrapper::encrypt(const std::string& plain)
{
	return _publicWrapper->encrypt(plain);
}

std::string RSAWrapper::decrypt(const char* cipher, unsigned int length)
{
	return _privateWrapper->decrypt(cipher, length);
}

RSAPublicWrapper::RSAPublicWrapper(const std::string& key)
{
	CryptoPP::StringSource ss(key, true);
	_publicKey.Load(ss);
}

RSAPublicWrapper::~RSAPublicWrapper()
{
}

std::string RSAPublicWrapper::getPublicKey() const
{
	std::string key;
	CryptoPP::StringSink ss(key);
	_publicKey.Save(ss);
	return key;
}

std::string RSAPublicWrapper::encrypt(const std::string& plain)
{
	std::string cipher;
	CryptoPP::RSAES_OAEP_SHA_Encryptor e(_publicKey);
	CryptoPP::StringSource ss(plain, true, new CryptoPP::PK_EncryptorFilter(_rng, e, new CryptoPP::StringSink(cipher)));
	return cipher;
}

RSAPrivateWrapper::RSAPrivateWrapper()
{
	_privateKey.Initialize(_rng, BITS);
}

RSAPrivateWrapper::RSAPrivateWrapper(const std::string& key)
{
	CryptoPP::StringSource ss(key, true);
	_privateKey.Load(ss);
}

RSAPrivateWrapper::~RSAPrivateWrapper()
{
}

std::string RSAPrivateWrapper::getPrivateKey() const
{
	std::string key;
	CryptoPP::StringSink ss(key);
	_privateKey.Save(ss);
	return key;
}

std::string RSAPrivateWrapper::getPublicKey() const
{
	CryptoPP::RSAFunction publicKey(_privateKey);
	std::string key;
	CryptoPP::StringSink ss(key);
	publicKey.Save(ss);
	return key;
}

std::string RSAPrivateWrapper::decrypt(const char* cipher, unsigned int length)
{
	std::string decrypted;
	CryptoPP::RSAES_OAEP_SHA_Decryptor d(_privateKey);
	CryptoPP::StringSource ss_cipher(reinterpret_cast<const CryptoPP::byte*>(cipher), length, true, new CryptoPP::PK_DecryptorFilter(_rng, d, new CryptoPP::StringSink(decrypted)));
	return decrypted;
}
