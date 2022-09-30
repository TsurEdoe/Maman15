import logging
import zlib
import os
from consts import SERVER_FILES_ROOT_DIRECTORY

"""
    Singelton class used to handle file operation on the file server
"""


class FileHandler:
    """
        Calculated CRC checksum for given data
    """

    @staticmethod
    def calculate_crc(data):
        return zlib.crc32(data)

    """
        Saves the given file data info a local directory with the given file name and given subpath
    """

    @staticmethod
    def save_file(file_data, file_path):
        try:
            try:
                os.mkdir(os.path.dirname(file_path))
            except FileExistsError as e:
                pass
            with open(file_path, "wb") as new_file:
                new_file.write(file_data)
        except Exception as e:
            logging.error("Failed saving file {0} to disk: {1}".format(file_path, e))
            return False

        return True
