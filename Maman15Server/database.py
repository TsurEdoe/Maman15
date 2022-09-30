import logging
import sqlite3
import threading
import uuid

from consts import SERVER_DB_FILE_NAME


class ServerDatabase:
    """Singleton class used for handling all DB operations (create, insert, update)"""
    __instance = None

    @staticmethod
    def get_instance():
        """ Static access method. """
        if ServerDatabase.__instance is None:
            ServerDatabase()
        return ServerDatabase.__instance

    def __init__(self):
        """ Virtually private constructor. """
        if ServerDatabase.__instance is not None:
            raise Exception("This class is a singleton!")
        else:
            self.__db = sqlite3.connect(SERVER_DB_FILE_NAME, check_same_thread=False)
            self.__cursor = self.__db.cursor()
            self.db_write_lock = threading.Lock()
            ServerDatabase.__instance = self
            self.__initialize_tables()
            sqlite3.register_adapter(uuid.UUID, lambda u: u.bytes_le)

    def __initialize_tables(self):
        """Initializes the needed table for the server operation (clients and file table)"""
        logging.info("Creating the clients' table")
        with self.db_write_lock:
            try:
                self.__cursor.execute(
                    """CREATE TABLE clients (
                    id varchar(16) primary key, 
                    name varchar(127), 
                    public_key varchar(160),
                    last_seen datetime,
                    shared_key varchar(32))
                    """)
            except sqlite3.OperationalError as e:
                logging.warn("Failed initializing table clients: {0}".format(e))

            logging.info("Creating the files' table")
            try:
                self.__cursor.execute(
                    """CREATE TABLE files (
                    file_name varchar(255), 
                    file_path varchar(255),
                    client_id integer,
                    verified boolean,
                    FOREIGN KEY(client_id) REFERENCES clients(id))
                    """)
            except sqlite3.OperationalError as e:
                logging.warn("Failed initializing table files: {0}".format(e))

    """
        Updated last seen 
    """

    def __update_last_seen(self, client_id):
        with self.db_write_lock:
            self.__cursor.execute(
                """UPDATE clients set 
                last_seen = DATETIME() 
                WHERE id = ?
                """, [client_id])
        logging.info("Set client's last seen field (uuid={0}) ".format(client_id))

    """
        Registers a new client into the DB (clients table). Adds only the name.
        Also updates last seen field for appropriate client
    """

    def register_new_client(self, client_name):
        try:
            client_uuid = uuid.uuid4()
            with self.db_write_lock:
                self.__cursor.execute(
                    """INSERT INTO clients (
                    id, name)
                    VALUES(?, ?)
                    """, [client_uuid, client_name])

        except Exception as e:
            logging.error("Failed registering client {0}: {1}".format(client_name, e))
            return -1

        logging.info("Added client {0} to databse with uuid {1}".format(client_name, client_uuid))
        self.__update_last_seen(client_uuid)
        return client_uuid

    def add_client_key_to_db(self, client_id, client_public_key, client_server_shared_key):
        """Adds the client public key and the client server shared aes key to the appropriate entry in the clients table.
                Also updates last seen field for appropriate client"""
        with self.db_write_lock:
            self.__cursor.execute(
                """UPDATE clients set 
                public_key = ?, shared_key = ?
                WHERE id = ?
                """, [client_public_key, client_server_shared_key, client_id])
        self.__update_last_seen(client_id)
        logging.info("Added client's with uuid {0} keys to the databse".format(client_id))

    def register_new_file(self, encrypted_file_name, encrypted_file_path, client_id):
        """Registers a new file into the DB (files table). Adds only the name, path, and client id.
           Also updates last seen field for appropriate client"""
        try:
            with self.db_write_lock:
                self.__cursor.execute(
                    """INSERT INTO files (
                    file_name, file_path, client_id, verified)
                    VALUES(?, ?, ?, FALSE)
                    """, [encrypted_file_name, encrypted_file_path, client_id])
        except Exception as e:
            logging.error("Failed registering file {0}: {1}".format(encrypted_file_name, e))
            return False

        self.__update_last_seen(client_id)
        logging.info("Added file {0} to database with client_id {1}".format(encrypted_file_name, client_id))
        return True

    def set_file_verified(self, file_name, client_id):
        """Sets the verified flag on the appropriate file. Also updates last seen field for appropriate client"""
        with self.db_write_lock:
            self.__cursor.execute(
                """UPDATE files set 
                verified = TRUE
                WHERE (client_id = ? and file_name = ?)
                """, [client_id, file_name])

        self.__update_last_seen(client_id)
        logging.info("Set file verified for {0} with client_id {1}".format(file_name, client_id))
