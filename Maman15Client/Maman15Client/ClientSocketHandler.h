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
	io_service _ioService;
	tcp::socket* _sock;
public:
	ClientSocketHandler(std::string ip, uint16_t port);
	~ClientSocketHandler();
	bool isConnected();
	bool receive(uint8_t (&buffer)[PACKET_SIZE]);
	bool send(const uint8_t(&buffer)[PACKET_SIZE]);
};

