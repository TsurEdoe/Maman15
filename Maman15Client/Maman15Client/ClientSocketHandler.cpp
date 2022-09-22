#include "ClientSocketHandler.h"

/*
	C'tor
*/
ClientSocketHandler::ClientSocketHandler(std::string ip, uint16_t port) : _ip(ip), _port(port)
{
	io_service io_service;
	this->_sock = new tcp::socket(io_service);
}

/*
	D'tor
*/
ClientSocketHandler::~ClientSocketHandler()
{
	this->_sock->close();
}

/*
	Connects the socket to the server
*/
bool ClientSocketHandler::connect()
{
	try
	{
		_sock->connect(tcp::endpoint(ip::address::from_string(_ip), _port));
	}
	catch (exception e)
	{
		cout << "ERROR: Failed connection to server: " << e.what() << endl;
		return false;
	}
	return true;
}

/*
   Receive (blocking) PACKET_SIZE bytes from socket
 */
bool ClientSocketHandler::receive(uint8_t(&buffer)[PACKET_SIZE])
{
	try
	{
		memset(buffer, 0, PACKET_SIZE);
		_sock->non_blocking(false);
		(void) read(_sock, boost::asio::buffer(buffer, PACKET_SIZE));
		return true;
	}
	catch(boost::system::system_error&)
	{
		return false;
	}
}


/*
   Send (blocking) PACKET_SIZE bytes to socket. Returns true if successfuly sent. false otherwise.
 */
bool ClientSocketHandler::send(const uint8_t(&buffer)[PACKET_SIZE])
{
	try
	{
		_sock->non_blocking(false);  // make sure socket is blocking.
		(void) write(_sock, boost::asio::buffer(buffer, PACKET_SIZE));
		return true;
	}
	catch (boost::system::system_error&)
	{
		return false;
	}
}