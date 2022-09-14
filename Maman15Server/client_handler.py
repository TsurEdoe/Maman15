from database import ServerDataBase

class ClientHandler:
    def __init__(self):
        self.database = ServerDatabase.getInstance()