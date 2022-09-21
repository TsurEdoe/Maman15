#include "MainClient.h"

MainClient::MainClient(ClientSocketHandler* clientSocketHandler, RSAWrapper* rsaWrapper, uint8_t* clientUUID, string userName) : 
	_clientSocketHandler(clientSocketHandler), _rsaWrapper(rsaWrapper), _clientUUID(clientUUID), _userName(userName)
{
    _encryptionHandler = new EncryptionHandler(rsaWrapper, clientSocketHandler);

    if (!_encryptionHandler->initializeHandler(clientUUID, userName))
    {
        delete _encryptionHandler;
        return;
    }

    _fileHandler = new FileHandler(_encryptionHandler);
}

MainClient::~MainClient()
{
    if (_encryptionHandler != NULL)
    {
        delete _encryptionHandler;
    }
    if (_fileHandler != NULL)
    {
        delete _fileHandler;
    }
}

bool MainClient::sendFileToServer(string fileFullPath)
{
    uint8_t buffer[PACKET_SIZE];

    if (!_fileHandler->sendEncryptedFile(fileFullPath, _clientUUID))
    {
        return false;
    }

    _clientSocketHandler->receive(buffer);
    ServerResponse crcResponse(buffer, sizeof(ServerResponse::ServerResponseHeader) + FILE_CONTENT_FEILD_SIZE + FILE_NAME_SIZE + CHECKSUM_SIZE);
    if (crcResponse.header._code != ServerResponse::ResponseType::LAST_SENT_FILE_CRC)
    {
        return false;
    }

    memset(buffer, 0, PACKET_SIZE);

    uint32_t serverCalculatedCRC = *(crcResponse.payload.payload + FILE_CONTENT_FEILD_SIZE + FILE_NAME_SIZE);
    uint32_t clientCalculatedCRC = _fileHandler->calculateFileCRC(fileFullPath);

    // CRC correct
    if (serverCalculatedCRC == clientCalculatedCRC)
    {
        ClientRequest crcClientResponseToServer(_clientUUID, ClientRequest::RequestCode::FILE_CRC_SUCCESS, 0, NULL);
        crcClientResponseToServer.serializeIntoBuffer(buffer);
        _clientSocketHandler->send(buffer);

        memset(buffer, 0, PACKET_SIZE);
        _clientSocketHandler->receive(buffer);

        ServerResponse crcResponse(buffer, sizeof(ServerResponse::ServerResponseHeader));
        if (crcResponse.header._code == ServerResponse::ResponseType::REQUEST_RECEIVED)
        {
            cout << "SUCCESS! Sent file to server with correct CRC received!" << endl;
            system("pause");
            return true;
        }

        cout << "Failed sending final message to server: Got bad response type (" << crcResponse.header._code << ")" << endl;
        return false;
    }

    // CRC Incorrect, resending
    ClientRequest crcClientResponseToServer(_clientUUID, ClientRequest::RequestCode::FILE_CRC_FAILED_RESENDING, 0, NULL);
    crcClientResponseToServer.serializeIntoBuffer(buffer);

    _clientSocketHandler->send(buffer);

    memset(buffer, 0, PACKET_SIZE);
    _clientSocketHandler->receive(buffer);

    ServerResponse finalCRCFailedResponse(buffer, sizeof(ServerResponse::ServerResponseHeader));
    if (crcResponse.header._code == ServerResponse::ResponseType::REQUEST_RECEIVED)
    {
        cout << "Resending file due to bad CRC calculation" << endl;
    }

    cout << "Failed sending FILE_CRC_FAILED_RESENDING message to server: Got bad response type (" << crcResponse.header._code << ")" << endl;
    return false;
}

bool MainClient::runClient(string fileFullPath)
{
    uint8_t buffer[PACKET_SIZE];

    for (size_t i = 0; i < MAX_RETRIES_FILE_RESEND; i++)
    {
        sendFileToServer(fileFullPath);
    }

    // CRC Incorrect, resending
    ClientRequest crcClientResponseToServer(_clientUUID, ClientRequest::RequestCode::FILE_CRC_FAILED_FINISHED, 0, NULL);
    crcClientResponseToServer.serializeIntoBuffer(buffer);

    _clientSocketHandler->send(buffer);

    memset(buffer, 0, PACKET_SIZE);
    _clientSocketHandler->receive(buffer);

    ServerResponse crcResponse(buffer, sizeof(ServerResponse::ServerResponseHeader));
    if (crcResponse.header._code == ServerResponse::ResponseType::REQUEST_RECEIVED)
    {
        cout << "Resending file due to bad CRC calculation" << endl;
    }

    cout << "Failed sending FILE_CRC_FAILED_FINISHED message to server: Got bad response type (" << crcResponse.header._code << ")" << endl;
    return false;
}