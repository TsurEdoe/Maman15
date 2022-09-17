import logging
from enum import Enum
import consts

"""
    Enum holding all of the different client requests
"""
class ClientRequest(Enum):
    CLIENT_REGISTRATION = "101"
    CLIENT_PUBLIC_KEY = "102"
    CLIENT_ENCRYPTED_FILE = "103"
    FILE_CRC_FAILED = "104"
    FILE_CRC_SUCCESS = "105"

"""
    Enum holding all of the different server responses
"""
class ServerResponse(Enum):
    REGISTRATION_SUCCESS = "200"
    REGISTRATION_FAILED = "301"
    CLIENT_AES_KEY = "201"
    LAST_SENT_FILE_CRC = "202"

"""
    Reads the port number from the given server info file and returns the correct port number to run the server on.
    If no file exists, returns the default port number
"""
def read_server_info_from_file():
    logging.info("Reading server info from file...")
    try:
        with open(consts.SERVER_INFO_FILE_NAME, "r") as server_info_file:
            return int(server_info_file.read(5))
    except:
        logging.warn("Failed reading server port from file, resorting to {0}".format(consts.DEFAULT_SERVER_PORT))
        return consts.DEFAULT_SERVER_PORT