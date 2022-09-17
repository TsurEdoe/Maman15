#pragma once
#include <iostream>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <exception>

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
public:
	ClientSocketHandler(std::string ip, uint8_t port);
	~ClientSocketHandler();
	bool receive(uint8_t (&buffer)[PACKET_SIZE]);
	bool send(const uint8_t(&buffer)[PACKET_SIZE]);
};

