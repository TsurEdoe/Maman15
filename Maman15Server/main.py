import logging
import utils
from client_connection_handler import ClientConnectionHandler


def main():
    logging.basicConfig(encoding='utf-8', level=logging.INFO)
    server_port = utils.read_server_info_from_file()
    logging.info("Starting server on port: {0}".format(server_port))
    client_connection_handler = ClientConnectionHandler(server_port, "127.0.0.1")

    try:
        client_connection_handler.bind_and_listen_for_clients()
    except Exception as e:
        logging.error("Failed running server with exception, closing server and existing: {0}".format(e))
        client_connection_handler.stop()


if __name__ == "__main__":
    main()
