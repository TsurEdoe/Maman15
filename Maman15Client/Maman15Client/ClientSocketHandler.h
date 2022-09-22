#pragma once
#include <iostream>
#include <exception>
#include <boost/asio.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

using namespace std;
using namespace boost::asio;
using ip::tcp;

#define PACKET_SIZE 1024

/*
	Handles socker IO operations.
*/
class ClientSocketHandler
{
private:
	tcp::socket* _sock;
	string _ip;
	uint16_t _port;
public:
	ClientSocketHandler(std::string ip, uint16_t port);
	~ClientSocketHandler();
	bool connect();
	bool receive(uint8_t (&buffer)[PACKET_SIZE]);
	bool send(const uint8_t(&buffer)[PACKET_SIZE]);
};

