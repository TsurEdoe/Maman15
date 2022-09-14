import logging
import utils
from file_server import FileServer

def main():
    server_port = utils.read_server_info_from_file()
    
    logging.info("Starting server on port: {0}".format(server_port))
    try:
        file_server = FileServer(server_port)
        file_server.start()
    except Exception:
        logging.error("Failed running server with exception, closing server and exising")



if __name__ == '__main__':
    main()
