import logging

SERVER_INFO_FILE_NAME = "port.info"
DEFAULT_SERVER_PORT = 1234 

def read_server_info_from_file():
    logging.info("Reading server info from file...")
    try:

        with open(SERVER_INFO_FILE_NAME, "r") as server_info_file:
            return int(server_info_file.read(5))
    except:
        logging.warn("Failed reading server port from file, resorting to {0}".format(DEFAULT_SERVER_PORT))
        return DEFAULT_SERVER_PORT