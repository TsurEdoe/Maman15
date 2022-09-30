import logging
import zlib
import os


class FileHandler:
    """Singleton class used to handle file operation on the file server"""

    @staticmethod
    def calculate_crc(data):
        """Calculated CRC checksum for given data"""
        return zlib.crc32(data)

    @staticmethod
    def save_file(file_data, file_path):
        """Saves the given file data info a local directory with the given file name and given sub-path"""
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
