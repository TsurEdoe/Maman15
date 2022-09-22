import logging, sqlite3, threading, uuid 
from consts import SERVER_DB_FILE_NAME

"""
    Singleton class used for handling all DB operations (create, insert, update)
"""
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
            sqlite3.register_adapter(uuid.UUID, lambda u: u.bytes_le)
        
    """
        Initializes the needed table for the server operation (clients and file table)
    """
    def __initialize_tables(self):
        logging.info("Creating the clients' table")
        self.db_write_lock.acquire()
        self.__cursor.execute(
            """CREATE TABLE clients (
            id varchar(127) primary key, 
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
            FOREIGN KEY(client_id) REFERENCES clients(id)
            verified boolean)
            """)
        self.db_write_lock.release()

    """
        Updated last seen 

    """
    def __update_last_seen(self, client_id):
        self.__cursor.execute(
            """UPDATE clients set 
            last_seen = DATETIME() 
            WHERE id = ?
            """, [client_id])
        self.db_write_lock.release()
        logging.info("Set client's last seen field (uuid={0}) ".format(client_id))

    """
        Registers a new client into the DB (clients table). Adds only the name.
        Also updates last seen field for appropriate client
    """
    def register_new_client(self, client_name):
        try:
            self.db_write_lock.acquire()
            self.__cursor.execute(
                """INSERT INTO clients (
                id, name)
                VALUES(?, ?)
                """, [uuid.uuid4(), client_name])
            client_uuid = self.__cursor.lastrowid
        except:
            logging.error("Failed registering client {0}".format(client_name))
            return -1

        logging.info("Added client {0} to databse with uuid {1}".format(client_name, client_uuid))
        self.__update_last_seen(client_uuid)
        self.db_write_lock.release()
        return client_uuid

    """
        Adds the client public key and the client server shared aes key to the appropriate entry in the clients table.
        Also updates last seen field for appropriate client
    """
    def add_client_key_to_db(self, client_id, client_public_key, client_server_shared_key):
        self.db_write_lock.acquire()
        self.__cursor.execute(
            """UPDATE clients set 
            public_key = ?, shared_key = ?
            WHERE id = ?
            """, [client_public_key, client_server_shared_key, client_id])
        self.__update_last_seen(client_id)
        self.db_write_lock.release()
        logging.info("Added client's with uuid {0} keys to the databse".format(client_id))

    """
        Registers a new file into the DB (files table). Adds only the name, path, and client id
        Also updates last seen field for appropriate client
    """
    def register_new_file(self, encrypted_file_name, encrypted_file_path, client_id):
        try:
            self.db_write_lock.acquire()
            self.__cursor.execute(
                """INSERT INTO file (
                file_name, file_path, client_id, verified)
                VALUES(?, ?, ?, FALSE)
                """, [encrypted_file_name, encrypted_file_path, client_id])
            file_uuid = self.__cursor.lastrowid
        except:
            logging.error("Failed registering file {0}".format(file_uuid))
            return -1
        
        self.__update_last_seen(client_id)
        self.db_write_lock.release()
        logging.info("Added file {0} to databse with uuid {1}".format(encrypted_file_name, file_uuid))
        return file_uuid

    """
        Sets the verified flag on the appropriate file.
        Also updates last seen field for appropriate client
    """
    def set_file_verified(self, file_id, client_id):
        self.db_write_lock.acquire()
        self.__cursor.execute(
            """UPDATE files set 
            verified = TRUE
            WHERE id = ?
            """, [file_id])
        self.__update_last_seen(client_id)
        self.db_write_lock.release()
        logging.info("Set file verified with uuid {0}".format(file_id))

    