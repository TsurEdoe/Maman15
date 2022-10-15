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
#define MAX_NUMBER_OF_SEND_TRIES 3

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
	bool receive(uint8_t (&buffer)[PACKET_SIZE], size_t bytesToReceive);
	bool send(const uint8_t* buffer, size_t bytesToSend);
};

