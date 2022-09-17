import threading, logging
from crypto_handler import CryptoHandler
from database import ServerDatabase
from file_handler import FileHandler
import consts
from utils import *

class ClientHandler(threading.Thread):
    def __init__(self, client_socket, client_address):
        self.__database = ServerDatabase.getInstance()
        self.__crypto_handler = CryptoHandler.getInstance()
        self.__file_handler = FileHandler.getInstance()
        self.__client_socket = client_socket
        self.__client_address = client_address
        self.__client_id = -1
        self.__client_name = ""
        self.__client_public_key = ""
        self.__client_server_shared_key = self.__crypto_handler.get_aes_key()

    def run(self):
        self.__handle_client()

    def stop(self):
        logging.info("Stopping client handler thread for client {0} and closing client socket".format(self.__client_name))
        self.__client_socket.close()
        self._stop_event.set()

    def __handle_client(self):
        try:
            self.__register_client()
            self.__send_aes_key_to_client()
            times_received_file = 0
            while times_received_file < MAX_TIMES_TO_RECEIVE_FILE:
                try:
                    if self.__receive_encrypted_file() == False:
                        times_received_file = times_received_file + 1
                        continue
                    else:
                        logging.info("Successfully received file from client {0}, closing connection {1}".format(self.__client_name, self.__client_address))
                        return
                except:
                    continue
            logging.error("Failed receiving file after max amout of times. closing connection with client {0}".format(self.__client_address))
        except:
            logging.error("Failed handling client {0} requests. Closing connection {1}".format(self.__client_name, self.__client_address))
        finally:
            self.__client_socket.close()

    def __register_client(self):
        request_code = self.__client_socket.receive(consts.CODE_FIELD_LENGTH)
        if request_code != ClientRequest.CLIENT_REGISTRATION:
            logging.error("Expected registration, got {0} instead".format(request_code))
            return False
        
        logging.info("Registering new client.")
        
        name_length = self.__client_socket.receive(1)
        if name_length > consts.MAX_CLIENT_NAME_LENGTH:
            logging.error("Name length longer than allowed.")
            return False 
        
        self.__client_name = self.__client_socket.receive(name_length)
        logging.info("New client name: {0}".format(self.__client_name))
        
        client_id = self.__database.register_new_client(self.__client_name)
        if client_id == -1:
            logging.error("Failed registering new client, returning REGISTRATION_FAILED message")
            self.__client_socket.send(ServerResponse.REGISTRATION_FAILED)
            return 
        logging.error("Success, new client registered, returning REGISTRATION_FAILED message")
        self.__client_socket.send(ServerResponse.REGISTRATION_SUCCESS)
        self.__client_id = client_id

    def __send_aes_key_to_client(self):
        request_code = self.__client_socket.receive(consts.CODE_FIELD_LENGTH)
        if request_code != ClientRequest.CLIENT_PUBLIC_KEY:
            logging.error("Expected client public key, got {0} instead".format(request_code))
            return False

        logging.info("Sending client {0} shared AES key".format(self.__client_name))

        self.__client_public_key = self.__client_socket.receive(consts.CLIENT_PUBLIC_KEY_LENGTH)
        self.__database.add_client_key_to_db(self.__client_id, self.__client_public_key, self.__client_server_shared_key)
        encrypted_shared_key = self.__crypto_handler.encrypt_data(self.__client_server_shared_key, self.__client_public_key)
        self.__client_socket.send(ServerResponse.CLIENT_AES_KEY)
        self.__client_socket.send(encrypted_shared_key)

    def __receive_encrypted_file(self):
        request_code = self.__client_socket.receive(consts.CODE_FIELD_LENGTH)
        if request_code != ClientRequest.CLIENT_ENCRYPTED_FILE:
            logging.error("Expected client public key, got {0} instead".format(request_code))
            raise

        logging.info("Receiving from client {0} encrypted file".format(self.__client_name))
        encrypted_file_path_size = self.__client_socket.receive(1)
        if encrypted_file_path_size > consts.MAX_FILE_PATH_LENGTH:
            logging.error("File path length longer than allowed.")
            raise

        encrypted_file_path = self.__client_socket.receive(encrypted_file_path_size)
        encrypted_file_path, encrypted_file_name = encrypted_file_path.split("/")[:-1], encrypted_file_path.split("/")[-1] 
        logging.info("Received file with path {0} and name {1}".format(encrypted_file_path, encrypted_file_name))

        file_id = self.__database.register_new_file(encrypted_file_name, encrypted_file_path, self.__client_id)
        if file_id == -1:
            raise

        encrypted_file_size = self.__client_socket.receive(consts.FILE_LENGTH_FIELD_LENGTH)
        encrypted_file_data = self.__client_socket.receive(encrypted_file_size)
        decrypted_file_data = self.__crypto_handler.decrypt_data(self.__client_server_shared_key, encrypted_file_data)
        return self.__handle_file_from_client(decrypted_file_data, encrypted_file_name, encrypted_file_path, file_id)

    def __handle_file_from_client(self, file_data, file_name, file_path, file_id):
        logging.info("Calculating file CRC for file {0}".format(file_name))
        file_CRC = self.__file_handler.calculate_CRC(file_data)
        self.__client_socket.send(ServerResponse.LAST_SENT_FILE_CRC)
        self.__client_socket.send(file_CRC)

        logging.info("Waiting for client CRC confirmation for file {0}".format(file_name))
        request_code = self.__client_socket.receive(consts.CODE_FIELD_LENGTH)
        if request_code == ClientRequest.FILE_CRC_FAILED:
            logging.warn("Failed CRC verification with client for file {0}, restarting file receive process".format(file_name))
            return False
        elif request_code == ClientRequest.FILE_CRC_SUCCESS:
            logging.info("Successeded CRC verification with client for file {0}, writing file to disk".format(file_name))
            if self.__file_handler.save_file(file_data, file_name, file_path) == False:
                return False
            logging.info("Setting file verified in db for file {0}".format(file_name))
            self.__database.set_file_verified(file_id, self.__client_id)
            return True
        else:
            logging.error("Expected file crc result, got {0} instead".format(request_code))
            return False