import logging
import socket
from client_handler import ClientHandler
"""
    Class used for handling incomig sockets and passing the requests to the logical handlers 
"""
class ClientConnectionHandler:
    def __init__(self, port, host = "127.0.0.1"):
        self.__client_connection_map = {}
        self.__port = port
        self.__host = host
        self.__server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        
    def bind_and_listen_for_clients(self):
        try:
            self.__server_socket.bind((self.__host, self.__port))
            logging.info("Socket on {0}:{1} binded!".format(self.__host, self.__port))
            
            self.__server_socket.listen(5)
            logging.info("Socket listening for new client connections...")

            while True:
                client_socket, client_address = self.__server_socket.accept()  
                logging.info("Connected to {0}:{1}".format(client_address[0],client_address))
                self.handle_new_client(client_socket, client_address)
        except Exception as exception:
            logging.error("Failed accepting connections: {0}. \nClosing all sockets...".format(exception))
            self.stop()
    
    def stop(self):
        self.__server_socket.close()
        for client_handler_thread in self.__client_connection_map.values():
            client_handler_thread.stop()
    
    def handle_new_client(self, client_socket, client_address):
        client_handler = ClientHandler(client_socket, client_address)
        client_handler.run()
        self.__client_connection_map[client_address] = client_handler