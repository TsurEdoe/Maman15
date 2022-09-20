#include "ClientSocketHandler.h"
#include "AESWrapper.h"

/*
        3. send RSA public key
        4. receive shared key and decrypt using private key
        5. read file data (file name in transfer.info)
        6. encrypt file data and send to server
        7. receive CRC and compare with local calculation
            a. if not the same go back to stage 5 (max 3 tries)
    */
class FileHandler
{
private:
    EncryptionHandler* _encryptionHandler;
    

public:
    FileHandler(EncryptionHandler* encryptionHandler);
    bool sendEncryptedFile(string fileFullPath);
}