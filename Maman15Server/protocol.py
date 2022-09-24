import logging
import struct
from enum import Enum

PACKET_SIZE = 1024
SERVER_VERSION = 3
CLIENT_PUBLIC_KEY_FIELD_LENGTH = 160
FILE_LENGTH_FIELD_LENGTH = 4
CLIENT_ID_FIELD_LENGTH = 16
FILE_NAME_LENGTH_FIELD_LENGTH = 255
CLIENT_NAME_LENGTH_FIELD_LENGTH = 255
CLIENT_VERSION_FIELD_LENGTH = 1
CODE_FIELD_LENGTH = 2
PAYLOAD_SIZE_FIELD_LENGTH = 2

class Payload:
    """Data class representing a common payload"""
    def __init__(self, payload):
        self.payload = bytes(payload)
        self.size = len(self.payload)

class ServerResponse:
    """Data class representing a response from the server"""
    def __init__(self, payload = "", server_version = SERVER_VERSION, code = 0):
        self.server_version = server_version
        self.code = code
        self.payload = Payload(payload)

    def size_without_payload(self):
        return CLIENT_VERSION_FIELD_LENGTH + CODE_FIELD_LENGTH + PAYLOAD_SIZE_FIELD_LENGTH

    def pack(self):
        """ Little Endian pack the Response """
        leftover = PACKET_SIZE - self.size_without_payload()
        if self.payload.size < leftover:
            leftover = self.payload.size
        return struct.pack(f"<BBI{leftover}s",self.server_version, \
            self.code, self.payload.size, self.payload.payload[:leftover])

    class ResponseType(Enum):
        REGISTRATION_SUCCESS = 2100,
        CLIENT_AES_KEY = 2102,
        LAST_SENT_FILE_CRC = 2103,
        REQUEST_RECEIVED = 2104

class ClientRequest:
    """Data class representing a request from the client"""
    def __init__(self, data):
        """Empty Init function"""
        self.client_id = ''
        self.client_version = 0
        self.code = 0
        self.payload = Payload()
        try:
            # Unpacking header
            self.client_id, self.client_version, self.code = \
                struct.unpack("<{0}sBB".format(CLIENT_ID_FIELD_LENGTH), data[:self.size_without_payload()])
            offset = self.size_without_payload()
            
            # Unpacking payload size
            self.payload.size = struct.unpack("<I", data[offset : offset + PAYLOAD_SIZE_FIELD_LENGTH])
            offset = offset + PAYLOAD_SIZE_FIELD_LENGTH
            
            # Unpacking payload
            leftover = PACKET_SIZE - offset
            if self.payload.size < leftover:
                leftover = self.payload.size
            self.payload.payload, = struct.unpack("<{0}s".format(leftover), data[offset : offset + leftover])
        except Exception as e:
            logging.error("Failed parsing ClientRequest with error: ", e)

    def size_without_payload(self):
        return CLIENT_ID_FIELD_LENGTH + CLIENT_VERSION_FIELD_LENGTH + CODE_FIELD_LENGTH + PAYLOAD_SIZE_FIELD_LENGTH

    class RequestType(Enum):
        CLIENT_REGISTRATION = 1100
        CLIENT_PUBLIC_KEY = 1101
        CLIENT_ENCRYPTED_FILE = 1103
        FILE_CRC_SUCCESS = 1104
        FILE_CRC_FAILED_RESENDING = 1105
        FILE_CRC_FAILED_FINISHED = 1106