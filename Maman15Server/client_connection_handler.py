import logging
import socket
from client_handler import ClientHandler


class ClientConnectionHandler:
    """Class used for handling incoming sockets and passing the requests to the logical handlers"""
    def __init__(self, port, host="127.0.0.1"):
        self.__client_connection_map = {}
        self.__port = port
        self.__host = host
        self.__server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    def bind_and_listen_for_clients(self):
        """Binds server socket and listens to new client connections"""
        try:
            self.__server_socket.bind((self.__host, self.__port))
            logging.info("Socket on {0}:{1} binded!".format(self.__host, self.__port))

            self.__server_socket.listen(5)
            logging.info("Socket listening for new client connections...")
            client_address = None

            while True:
                try:
                    client_socket, client_address = self.__server_socket.accept()
                    logging.info("Connected to {0}:{1}".format(client_address[0], client_address))
                    self.handle_new_client(client_socket, client_address)
                except Exception as exception:
                    logging.error("Failed handling client {0}: {1}".format(client_address, exception))
                    continue
        except Exception as exception:
            logging.error("Failed accepting connections: {0}. \nClosing all sockets...".format(exception))
            self.stop()

    def stop(self):
        """Stops the server by closing the main socket and closing all client handler threads"""
        self.__server_socket.close()
        for client_handler_thread in self.__client_connection_map.values():
            client_handler_thread.stop()

    def handle_new_client(self, client_socket, client_address):
        """Handles a new client connection by starting the new client handler thread"""
        client_handler = ClientHandler(client_socket, client_address)
        self.__client_connection_map[client_address] = client_handler
        client_handler.start()
