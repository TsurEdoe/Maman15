#include <boost/asio.hpp>
#include "ClientInitializer.h"
#include "EncryptionHandler.h"

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

    if (clientSocketHandler == NULL || rsaWrapper == NULL || clientUUID == NULL)
    {
        return -1;
    }

    EncryptionHandler* encryptionHandler = new EncryptionHandler(rsaWrapper, clientSocketHandler, clientUUID);

    if (!encryptionHandler.getSharedSecret())
    {
        delete encryptionHandler;
        return -1;
    }

    FileHandler fileHandler(encryptionHandler);

    if (!fileHandler.sendEncryptedFile(clientInitializer.getFileFullPath()))
    {
        delete encryptionHandler;
        return -1;
    }

    return 0;
}