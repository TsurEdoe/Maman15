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
PAYLOAD_SIZE_FIELD_LENGTH = 4
CHECKSUM_FIELD_LENGTH = 4


class ResponseType(Enum):
    DEFAULT_RESPONSE_TYPE = 0
    REGISTRATION_SUCCESS = 2100
    CLIENT_AES_KEY = 2102
    LAST_SENT_FILE_CRC = 2103
    REQUEST_RECEIVED = 2104


class RequestType(Enum):
    DEFAULT_REQUEST_TYPE = 0
    CLIENT_REGISTRATION = 1100
    CLIENT_PUBLIC_KEY = 1101
    CLIENT_ENCRYPTED_FILE = 1103
    FILE_CRC_SUCCESS = 1104
    FILE_CRC_FAILED_RESENDING = 1105
    FILE_CRC_FAILED_FINISHED = 1106


class Payload:
    """Data class representing a common payload"""

    def __init__(self, payload=bytes()):
        self.payload = payload
        self.size = len(self.payload)


class ServerResponse:
    """Data class representing a response from the server"""

    def __init__(self, payload=bytes(), code=ResponseType.DEFAULT_RESPONSE_TYPE, server_version=SERVER_VERSION):
        self.server_version = server_version
        self.code = code
        self.payload = Payload(payload)

    @staticmethod
    def size_without_payload():
        return CLIENT_VERSION_FIELD_LENGTH + CODE_FIELD_LENGTH + PAYLOAD_SIZE_FIELD_LENGTH

    def pack(self):
        """ Little Endian pack the Response """
        leftover = PACKET_SIZE - self.size_without_payload()
        if self.payload.size < leftover:
            leftover = self.payload.size
        return struct.pack(f"<BHI{leftover}s", self.server_version, self.code.value, self.payload.size,
                           self.payload.payload[:leftover])


class ClientRequest:
    """Data class representing a request from the client"""

    def __init__(self, data, client_socket, read_payload=False):
        """Empty Init function"""
        self.client_id = ''
        self.client_version = 0
        self.code = RequestType.DEFAULT_REQUEST_TYPE
        self.payload = Payload()
        try:
            # Unpacking header
            self.client_id, self.client_version, self.code, self.payload.size = \
                struct.unpack("<{0}sBHI".format(CLIENT_ID_FIELD_LENGTH), data[:self.size_without_payload()])
            self.code = RequestType(self.code)
        except Exception as e:
            logging.error("Failed parsing ClientRequest with error: ", e)

        if read_payload:
            self.receive_payload(client_socket)

    @staticmethod
    def size_without_payload():
        return CLIENT_ID_FIELD_LENGTH + CLIENT_VERSION_FIELD_LENGTH + CODE_FIELD_LENGTH + PAYLOAD_SIZE_FIELD_LENGTH

    def receive_payload(self, socket_to_receive_from):
        # Helper function to recv n bytes or return None if EOF is hit
        data = bytearray()
        while len(data) < self.payload.size:
            packet = socket_to_receive_from.recv(self.payload.size - len(data))
            if not packet:
                return None
            data.extend(packet)
        self.payload = Payload(data)
