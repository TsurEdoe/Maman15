#include "ClientInitializer.h"
#include "MainClient.h"


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

    MainClient mainClient(clientSocketHandler, rsaWrapper, clientUUID, userName);

    if (!mainClient.runClient(clientInitializer.getFileFullPath()))
    {
        return -1;
    }

    return 0;
}