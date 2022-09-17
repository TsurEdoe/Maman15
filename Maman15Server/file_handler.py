import logging
import zlib
import os

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

    def calculate_CRC(data):
        return zlib.crc32(data)

    def save_file(self, file_data, file_name, file_path):
        try:
            with open(os.path.join(".", file_path, file_name), "w") as new_file:
                new_file.write(file_data)
        except:
            logging.error("Failed saving file {0} to disk.".format(file_name))
            return False
        return True