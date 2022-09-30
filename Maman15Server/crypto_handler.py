from Crypto.Random import get_random_bytes
from Crypto.Cipher import AES, PKCS1_OAEP
from Crypto.PublicKey import RSA
from Crypto.Util.Padding import pad, unpad

AES_KEY_LENGTH = 16

class CryptoHandler:
    @staticmethod
    def get_aes_key():
        return get_random_bytes(AES_KEY_LENGTH)

    @staticmethod
    def encrypt_data_asymmetric(plain_data, encryption_key):
        imported_encryption_key = RSA.importKey(encryption_key)
        cipher_rsa = PKCS1_OAEP.new(imported_encryption_key)
        return cipher_rsa.encrypt(plain_data)

    @staticmethod
    def encrypt_data_symmetric(plain_data, encryption_key):
        plain_data = pad(plain_data)
        cipher_aes = AES.new(encryption_key, AES.MODE_CBC, bytes(16))
        return cipher_aes.encrypt(plain_data)

    @staticmethod
    def decrypt_data_symmetric(cipher_data, decryption_key):
        cipher_aes = AES.new(decryption_key, AES.MODE_CBC, bytes(16))
        return unpad(cipher_aes.decrypt(cipher_data), AES.block_size)
