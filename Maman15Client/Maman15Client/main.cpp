#include <boost/asio.hpp>
#include "ClientInitializer.h"
#include "EncryptionHandler.h"
#include "FileHandler.h"

#define MAX_RETRIES_FILE_RESEND 3

int main(int argc, char* argv[])
{
    ClientInitializer clientInitializer;
    if (!clientInitializer.initializeClient())
    {
        return -1;
    }
    
    ClientSocketHandler* clientSocketHandler = clientInitializer.getClientSocketHandler();
    RSAWrapper* rsaWrapper = clientInitializer.getRSAWrapper();
    uint8_t* clientUUID = clientInitializer.getClientUUID();
    string userName = clientInitializer.getUserName();

    if (clientSocketHandler == NULL || rsaWrapper == NULL || clientUUID == NULL || userName == "")
    {
        return -1;
    }

    EncryptionHandler* encryptionHandler = new EncryptionHandler(rsaWrapper, clientSocketHandler);

    if (!encryptionHandler->initializeHandler(clientUUID, userName))
    {
        delete encryptionHandler;
        return -1;
    }

    FileHandler fileHandler(encryptionHandler);

    for (size_t i = 0; i < MAX_RETRIES_FILE_RESEND; i++)
    {
        if (!fileHandler.sendEncryptedFile(clientInitializer.getFileFullPath(), clientUUID))
        {
            delete encryptionHandler;
            return -1;
        }
    }

    return 0;
}