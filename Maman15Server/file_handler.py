import logging
import zlib
import os
from consts import SERVER_FILES_ROOT_DIRECTORY

"""
    Singelton class used to handle file operation on the file server
"""
class FileHandler:
    __instance = None
    @staticmethod 
    def getInstance():
        """ Static access method. """
        if FileHandler.__instance == None:
            FileHandler()
        return FileHandler.__instance
    
    def __init__(self):
        """ Virtually private constructor. """
        if FileHandler.__instance != None:
            raise Exception("This class is a singleton!")
        else:       
            FileHandler.__instance = self

    """
        Calculated CRC checksum for given data
    """
    def calculate_CRC(data):
        return zlib.crc32(data)

    """
        Saves the given file data info a local directory with the given file name and given subpath
    """
    def save_file(self, file_data, file_name, file_path):
        try:
            with open(os.path.join(SERVER_FILES_ROOT_DIRECTORY, file_path, file_name), "w") as new_file:
                new_file.write(file_data)
        except:
            logging.error("Failed saving file {0} to disk.".format(file_name))
            return False
        return True