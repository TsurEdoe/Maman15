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
        self.__cursor.execute('''CREATE TABLE clients (
            id integer primary key autoincrement, 
            name varchar(127), 
            public_key varchar(160),
            last_seen datetime,
            shared_key varchar(32))
            ''')
        
        logging.info("Creating the files' table")
        self.__cursor.execute('''CREATE TABLE files (
            id integer primary key autoincrement, 
            file_name varchar(255), 
            path_name varchar(255),
            verified boolean)
            ''')
    
    
    def register_new_client(self):