from Crypto.Cipher import AES
from Crypto.Random import get_random_bytes
import base64

pad = lambda s: s + (AES.block_size - len(s) % AES.block_size) * chr(AES.block_size - len(s) % AES.block_size)
unpad = lambda s: s[:-ord(s[len(s) - 1:])]

class CryptoHandler:
    __instance = None
    @staticmethod 
    def getInstance():
        """ Static access method. """
        if CryptoHandler.__instance == None:
            CryptoHandler()
        return CryptoHandler.__instance
    
    def __init__(self):
        """ Virtually private constructor. """
        if CryptoHandler.__instance != None:
            raise Exception("This class is a singleton!")
        else:
            CryptoHandler.__instance = self
    
    def get_aes_key(self):
        return get_random_bytes(32)

    def encrypt_data(self, plain_data, encryption_key):
        plain_data = pad(plain_data)
        cipher = AES.new(encryption_key, AES.MODE_CBC, 0)
        return base64.b64encode(cipher.encrypt(plain_data))
        
    def decrypt_data(self, cipher_data, decryption_key):
        cipher_data = base64.b64decode(cipher_data)
        cipher = AES.new(decryption_key, AES.MODE_CBC, 0)
        return unpad(cipher.decrypt(cipher_data[16:]))