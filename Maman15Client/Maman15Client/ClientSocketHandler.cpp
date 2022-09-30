#include "ClientSocketHandler.h"

/*
	C'tor
*/
ClientSocketHandler::ClientSocketHandler(std::string ip, uint16_t port)
{
	_sock = new tcp::socket(_ioService);
	_sock->connect(tcp::endpoint(ip::address::from_string(ip), port));
}

/*
	D'tor
*/
ClientSocketHandler::~ClientSocketHandler()
{
	_sock->close();
}

/*
	Returns the socket conncection to the server status 
*/
bool ClientSocketHandler::isConnected()
{
	return _sock->is_open();
}

/*
   Receive (blocking) PACKET_SIZE bytes from socket
 */
bool ClientSocketHandler::receive(uint8_t(&buffer)[PACKET_SIZE], size_t bytesToReceive)
{
	try
	{
		memset(buffer, 0, PACKET_SIZE);
		_sock->non_blocking(false);
		(void) read(_sock, boost::asio::buffer(buffer, bytesToReceive));
		return true;
	}
	catch(boost::system::system_error& e)
	{
		cout << "ERROR: ClientSocketHandler - Failed receiving data: " << e.what() << endl;
		return false;
	}
}


/*
   Send (blocking) PACKET_SIZE bytes to socket. Returns true if successfuly sent. false otherwise.
 */
bool ClientSocketHandler::send(const uint8_t(&buffer)[PACKET_SIZE], size_t bytesToSend)
{
	try
	{
		_sock->non_blocking(false);  // make sure socket is blocking.
		(void) write(_sock, boost::asio::buffer(buffer, bytesToSend));
		return true;
	}
	catch (boost::system::system_error& e)
	{
		cout << "ERROR: ClientSocketHandler - Failed sending data: " << e.what() << endl;
		return false;
	}
}