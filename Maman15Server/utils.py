import logging
import consts


def read_server_info_from_file():
    """Reads the port number from the given server info file and returns the correct port number to run the server on.
    If no file exists, returns the default port number """
    logging.info("Reading server info from file...")
    try:
        with open(consts.SERVER_INFO_FILE_NAME, "r") as server_info_file:
            return int(server_info_file.read())
    except Exception as e:
        logging.warn(
            "Failed reading server port from file, resorting to {0}: {1}".format(consts.DEFAULT_SERVER_PORT, e))
        return consts.DEFAULT_SERVER_PORT
