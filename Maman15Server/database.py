import logging
import sqlite3
import threading

SERVER_DB_FILE_NAME = "server.db"

class ServerDatabase:
    __instance = None
    @staticmethod 
    def getInstance():
        """ Static access method. """
        if ServerDatabase.__instance == None:
            ServerDatabase()
        return ServerDatabase.__instance
    
    def __init__(self):
        """ Virtually private constructor. """
        if ServerDatabase.__instance != None:
            raise Exception("This class is a singleton!")
        else:
            self.__db = sqlite3.connect(SERVER_DB_FILE_NAME)
            self.__cursor = self.__db.cursor()
            self.db_write_lock = threading.Lock()
            ServerDatabase.__instance = self
            self.__initialize_tables()
        
    def __initialize_tables(self):
        logging.info("Creating the clients' table")
        self.__cursor.execute(
            """CREATE TABLE clients (
            id integer primary key autoincrement, 
            name varchar(127), 
            public_key varchar(160),
            last_seen datetime,
            shared_key varchar(32))
            """)
        
        logging.info("Creating the files' table")
        self.__cursor.execute(
            """CREATE TABLE files (
            id integer primary key autoincrement, 
            file_name varchar(255), 
            path_name varchar(255),
            verified boolean)
            """)
    
    
    def __validate_db_input(data_to_validate):
        ''' todo '''
        pass

    def register_new_client(self, client_name):
        logging.info("Validating client name before inputting")
        if self.__validate_db_input(client_name) == False:
            return -1
        try:
            self.__cursor.execute(
                """INSERT INTO clients (
                name, last_seen)
                VALUES({0},DATETIME())
                """.format(client_name))
            client_uuid = self.__cursor.lastrowid
        except:
            logging.error("Failed registering client {0}".format(client_name))
            return -1

        logging.info("Added client {0} to databse with uuid {1}".format(client_name, client_uuid))
        return client_uuid

    def add_client_key_to_db(self, client_id, client_public_key, client_server_shared_key):
        logging.info("Validating client public key before inputting")
        self.__validate_db_input(client_public_key)
        self.__cursor.execute(
            """UPDATE clients set 
            public_key = {0}, shared_key = {1}, last_seen = DATETIME() 
            WHERE id = {2}
            """.format(client_public_key, client_server_shared_key, client_id))
        logging.info("Added client's with uuid {0} keys to the databse".format(client_id))

    def register_new_file(self, encrypted_file_name, encrypted_file_path, client_id):
        logging.info("Validating file name and path before inputting")
        if self.__validate_db_input(encrypted_file_name) == False or self.__validate_db_input(encrypted_file_path) == False:
            return -1
        try:
            self.__cursor.execute(
                """INSERT INTO file (
                file_name, file_path, verified)
                VALUES({0}, {1}, FALSE)
                """.format(encrypted_file_name, encrypted_file_path))
            file_uuid = self.__cursor.lastrowid
        except:
            logging.error("Failed registering file {0}".format(file_uuid))
            return -1
            
        logging.info("Added file {0} to databse with uuid {1}".format(encrypted_file_name, file_uuid))
        return file_uuid

    def set_file_verified(self, file_id):
        self.__cursor.execute(
            """UPDATE clients set 
            verified = TRUE
            WHERE id = {2}
            """.format(file_id))
        logging.info("Set file verified with uuid {0}".format(file_id))

    