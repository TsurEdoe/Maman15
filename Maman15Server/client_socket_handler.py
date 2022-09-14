import logging
import socket

"""
    Class used for handling incomig sockets and passing the requests to the logical handlers 
"""
class ClientSocketHandler:
    def __init__(self, client_handler, port, host = '127.0.0.1'):
        self.port = port
        self.host = host
        self.client_handler = client_handler
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        
    def bind_and_listen_for_clients(self):
        try:
            self.server_socket.bind((self.host, self.port))
            logging.info("Socket on {0}:{1} binded!".format(self.host, self.port))
            
            self.server_socket.listen(5)
            logging.info("Socket listening for new client connections...")

            while True:
                c, addr = self.server_socket.accept()  
                logging.info('Connected to :', addr[0], ':', addr[1])
                self.client_handler.handle_new_client(c)
        except Exception:
            logging.error("Failed accepting connectins from client, closing socket...")
            self.server_socket.close()