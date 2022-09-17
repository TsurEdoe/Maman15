import logging
from enum import Enum

SERVER_INFO_FILE_NAME = "port.info"
DEFAULT_SERVER_PORT = 1234 
MAX_CLIENT_NAME_LENGTH = 255
CLIENT_PUBLIC_KEY_LENGTH = 160
CODE_FIELD_LENGTH = 3
FILE_LENGTH_FIELD_LENGTH = 4
MAX_FILE_PATH_LENGTH = 255
MAX_TIMES_TO_RECEIVE_FILE = 3

class ClientRequest(Enum):
    CLIENT_REGISTRATION = "101"
    CLIENT_PUBLIC_KEY = "102"
    CLIENT_ENCRYPTED_FILE = "103"
    FILE_CRC_FAILED = "104"
    FILE_CRC_SUCCESS = "105"

class ServerResponse(Enum):
    REGISTRATION_SUCCESS = "200"
    REGISTRATION_FAILED = "301"
    CLIENT_AES_KEY = "201"
    LAST_SENT_FILE_CRC = "202"

def read_server_info_from_file():
    logging.info("Reading server info from file...")
    try:
        with open(SERVER_INFO_FILE_NAME, "r") as server_info_file:
            return int(server_info_file.read(5))
    except:
        logging.warn("Failed reading server port from file, resorting to {0}".format(DEFAULT_SERVER_PORT))
        return DEFAULT_SERVER_PORT