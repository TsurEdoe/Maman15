import logging
import utils
from client_connection_handler import ClientConnectionHandler

def main():
    server_port = utils.read_server_info_from_file()
    
    logging.info("Starting server on port: {0}".format(server_port))
    try:
        client_connection_handler = ClientConnectionHandler(server_port, "127.0.0.1")
        client_connection_handler.bind_and_listen_for_clients()
    except Exception:
        logging.error("Failed running server with exception, closing server and exising")
        client_connection_handler.stop()


if __name__ == "__main__":
    main()
