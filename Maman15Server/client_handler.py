import threading, os

import protocol
from crypto_handler import CryptoHandler
from uuid import UUID
from database import ServerDatabase
from file_handler import FileHandler
from protocol import *
from utils import *


class ClientHandler(threading.Thread):
    def __init__(self, client_socket, client_address):
        self.__database = ServerDatabase.getInstance()
        self.__client_socket = client_socket
        self.__client_address = client_address
        self.__client_id = -1
        self.__client_name = ""
        self.__client_public_key = ""
        self.__client_server_shared_key = CryptoHandler.get_aes_key()

    def run(self):
        if not self.__handle_client():
            self.stop()

    def stop(self):
        logging.info(
            "Stopping client handler thread for client {0} and closing client socket".format(self.__client_id))
        self.__client_socket.close()
        return

    def __handle_client(self):
        try:
            if not self.__handle_first_message():
                return False
            times_received_file = 0
            while times_received_file < consts.MAX_TIMES_TO_RECEIVE_FILE:
                try:
                    if not self.__receive_encrypted_file():
                        times_received_file = times_received_file + 1
                        continue
                    else:
                        logging.info("Successfully received file from client {0}, closing connection {1}".format(
                            self.__client_id, self.__client_address))
                        return True
                except Exception as e:
                    logging.error("Failed receiving file in try number {0}: {1}".format(times_received_file, e))
                    continue
            logging.error("Failed receiving file after max amount of times. closing connection with client {0}".format(
                self.__client_address))
            return False
        except Exception as e:
            logging.error(
                "Failed handling client {0} requests: {1}. Closing connection {2}.".format(self.__client_id, e,
                                                                                           self.__client_address))
            return False

    def __handle_first_message(self):
        first_message_request_data = self.__client_socket.recv(ClientRequest.size_without_payload())
        first_message_request = ClientRequest(first_message_request_data, self.__client_socket)

        if first_message_request.code == RequestType.CLIENT_REGISTRATION:
            logging.info("Got registration request, registering client {0}".format(first_message_request.client_id))
            if not self.__register_client(first_message_request):
                return False
            logging.info("Continuing to send shared client server key to {0}".format(first_message_request.client_id))
            client_key_request_data = self.__client_socket.recv(ClientRequest.size_without_payload())
            client_key_request = ClientRequest(client_key_request_data, self.__client_socket)
            return self.__send_aes_key_to_client(client_key_request)

        if first_message_request.code == RequestType.CLIENT_PUBLIC_KEY:
            logging.info("Got shared key request, sending shared key to client".format(first_message_request.client_id))
            self.__client_id = UUID(bytes=first_message_request.client_id)
            return self.__send_aes_key_to_client(first_message_request)
        else:
            logging.error("Got unexpected message request from client, failing handler")
            return False

    def __register_client(self, registration_request):
        registration_request.receive_payload(self.__client_socket)

        if registration_request.code != RequestType.CLIENT_REGISTRATION:
            logging.error("Expected registration, got {0} instead".format(registration_request.code))
            return False

        logging.info("Registering new client.")

        self.__client_name = registration_request.payload.payload[:CLIENT_NAME_LENGTH_FIELD_LENGTH].decode("UTF-8")
        logging.info("New client name: {0}".format(self.__client_name))

        client_id = self.__database.register_new_client(self.__client_name)
        if client_id == -1:
            logging.error("Failed registering new client")
            return False
        logging.info("Success, new client registered, returning REGISTRATION_SUCCESS message")
        self.__client_id = UUID(bytes=client_id.bytes)
        registration_succeeded_response = ServerResponse(self.__client_id.bytes, ResponseType.REGISTRATION_SUCCESS)
        self.__client_socket.send(registration_succeeded_response.pack())
        return True

    def __send_aes_key_to_client(self, client_key_request):
        client_key_request.receive_payload(self.__client_socket)

        if client_key_request.code != RequestType.CLIENT_PUBLIC_KEY:
            logging.error("Expected client public key, got {0} instead".format(client_key_request.code))
            return False

        logging.info("Sending client {0} shared AES key".format(self.__client_id))

        self.__client_public_key = client_key_request.payload.payload[CLIENT_NAME_LENGTH_FIELD_LENGTH:
                                                                      CLIENT_NAME_LENGTH_FIELD_LENGTH
                                                                      + CLIENT_PUBLIC_KEY_FIELD_LENGTH]
        self.__database.add_client_key_to_db(self.__client_id, self.__client_public_key,
                                             self.__client_server_shared_key)
        encrypted_shared_key = CryptoHandler.encrypt_data_asymmetric(self.__client_server_shared_key,
                                                                     self.__client_public_key)

        shared_key_response = ServerResponse(self.__client_id.bytes + encrypted_shared_key, ResponseType.CLIENT_AES_KEY)
        self.__client_socket.send(shared_key_response.pack())
        return True

    def __receive_encrypted_file(self):
        encrypted_file_request_data = self.__client_socket.recv(ClientRequest.size_without_payload())
        encrypted_file_request = ClientRequest(encrypted_file_request_data, self.__client_socket, True)

        if encrypted_file_request.code != RequestType.CLIENT_ENCRYPTED_FILE:
            logging.error("Expected client encrypted file, got {0} instead".format(encrypted_file_request.code))
            return False

        logging.info("Receiving from client {0} encrypted file".format(self.__client_id))
        encrypted_file_size = struct.unpack("<L", encrypted_file_request.payload.payload[
                                                  CLIENT_ID_FIELD_LENGTH:
                                                  CLIENT_ID_FIELD_LENGTH + FILE_LENGTH_FIELD_LENGTH])[0]

        encrypted_file_name = encrypted_file_request.payload.payload[
                              CLIENT_ID_FIELD_LENGTH + FILE_LENGTH_FIELD_LENGTH: \
                              CLIENT_ID_FIELD_LENGTH + FILE_LENGTH_FIELD_LENGTH + \
                              FILE_NAME_LENGTH_FIELD_LENGTH].decode("UTF-8").rstrip('\x00')

        encrypted_file_path = os.path.join(consts.SERVER_FILES_ROOT_DIRECTORY, encrypted_file_name)
        logging.info("Received file name {0}".format(encrypted_file_name))

        file_id = self.__database.register_new_file(encrypted_file_name, encrypted_file_path, self.__client_id)
        if file_id == -1:
            return False

        encrypted_file_data = encrypted_file_request.payload.payload[
                              CLIENT_ID_FIELD_LENGTH + FILE_LENGTH_FIELD_LENGTH + FILE_NAME_LENGTH_FIELD_LENGTH:
                              CLIENT_ID_FIELD_LENGTH + FILE_LENGTH_FIELD_LENGTH + FILE_NAME_LENGTH_FIELD_LENGTH + encrypted_file_size]

        decrypted_file_data = CryptoHandler.decrypt_data_symmetric(encrypted_file_data, self.__client_server_shared_key)

        return self.__handle_file_from_client(decrypted_file_data, encrypted_file_name, encrypted_file_path, file_id,
                                              encrypted_file_size)

    def __handle_file_from_client(self, file_data, file_name, file_path, file_id, encrypted_file_size):
        logging.info("Calculating file CRC for file {0}".format(file_name))
        file_crc = FileHandler.calculate_crc(file_data)

        crc_response_payload = struct.pack(
            "<{0}sI{1}sI".format(protocol.CLIENT_ID_FIELD_LENGTH, protocol.FILE_NAME_LENGTH_FIELD_LENGTH),
                    self.__client_id.bytes, encrypted_file_size, bytes(file_name, "UTF-8"), file_crc)

        crc_response = ServerResponse(crc_response_payload, ResponseType.LAST_SENT_FILE_CRC)
        self.__client_socket.send(crc_response.pack())

        logging.info("Waiting for client CRC confirmation for file {0}".format(file_name))
        crc_client_confirmation_request_data = self.__client_socket.recv(ClientRequest.size_without_payload())
        crc_client_confirmation_request = ClientRequest(crc_client_confirmation_request_data,
                                                        self.__client_socket, True)

        if crc_client_confirmation_request.code == RequestType.FILE_CRC_FAILED_RESENDING:
            logging.warn(
                "Failed CRC verification with client for file {0}, restarting file receive process".format(file_name))
            return False
        elif crc_client_confirmation_request.code == RequestType.FILE_CRC_SUCCESS:
            logging.info(
                "Succeeded CRC verification with client for file {0}, writing file to disk".format(file_name))
            if not FileHandler.save_file(file_data, file_path):
                return False

            logging.info("Setting file verified in db for file {0}".format(file_name))
            self.__database.set_file_verified(file_id, self.__client_id)

            logging.info("Sending request received to client {0}".format(self.__client_id))
            final_response = ServerResponse(code=ResponseType.REQUEST_RECEIVED)
            self.__client_socket.send(final_response.pack())
            return True
        elif crc_client_confirmation_request.code == RequestType.FILE_CRC_FAILED_FINISHED:
            logging.warn(
                "Failed CRC verification with client for file {0}, finishing file receive process".format(file_name))
            return False
        else:
            logging.error("Expected file crc result, got {0} instead".format(crc_client_confirmation_request.code))
            return False
