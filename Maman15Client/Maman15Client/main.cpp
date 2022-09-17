#include <boost/asio.hpp>
#include "ClientInitializer.h"
#include "CryptoUtils.h"

int main(int argc, char* argv[])
{
    /*
        0. read transfer.info
            0a. connect to server
        1. check me.info
            a. if not exist - register and create me.info
        2. generate RSA key pair
        3. send RSA public key
        4. receive shared key and decrypt using private key
        5. read file data (file name in transfer.info)
        6. encrypt file data and send to server
        7. receive CRC and compare with local calculation
            a. if not the same go back to stage 5 (max 3 tries)


    */
    CryptoUtils cryptoUtils;
    ClientInitializer clientInitializer;
    
    ClientSocketHandler* clientSocketHandler = clientInitializer.initializeClient();

    clientSocketHandler->send()
    return 0;
}