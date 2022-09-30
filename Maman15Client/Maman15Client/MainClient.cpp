#include "MainClient.h"

/*
    C'tor
*/
MainClient::MainClient(ClientSocketHandler* clientSocketHandler, RSAWrapper* rsaWrapper, uint8_t* clientUUID, string userName) : 
	_clientSocketHandler(clientSocketHandler), _rsaWrapper(rsaWrapper), _clientUUID(clientUUID), _userName(userName)
{
    _encryptionHandler = new EncryptionHandler(rsaWrapper, clientSocketHandler);

    if (!_encryptionHandler)
    {
        cout << "Error: Failed creating MainClient, Failed creating encryptoin handler" << endl;
    }

    if (!_encryptionHandler->initializeHandler(clientUUID, userName))
    {
        delete _encryptionHandler;
        throw std::runtime_error("Error: Failed initizing encryption handler");
    }

    _fileHandler = new FileHandler(_encryptionHandler, clientSocketHandler);
}

/*
    D'tor
*/
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

/*
    Sends a file to the server.
    Returns true if CRC response from server is correct, false otherwise.
*/
bool MainClient::sendFileToServer(string fileName)
{
    uint8_t buffer[PACKET_SIZE];
    memset(buffer, 0, PACKET_SIZE);

    if (!_fileHandler->sendEncryptedFile(fileName, _clientUUID))
    {
        return false;
    }

    cout << "MainClient: Started file CRC validation on " << fileName << endl;

    if (handleCRCValidation(getServerCalculatedCRC(), _fileHandler->calculateFileCRC(fileName)))
    {
        return true;
    }

    cout << "Wrong CRC calculated for file " << fileName << " by server, resending file" << endl;

    // CRC Incorrect, resending
    ClientRequest crcClientResponseToServer(_clientUUID, ClientRequest::RequestCode::FILE_CRC_FAILED_RESENDING, 0, NULL);
    crcClientResponseToServer.serializeIntoBuffer(buffer);

    _clientSocketHandler->send(buffer, crcClientResponseToServer.sizeWithPayload());

    memset(buffer, 0, PACKET_SIZE);
    _clientSocketHandler->receive(buffer, sizeof(ServerResponse::ServerResponseHeader) + sizeof(uint32_t));

    ServerResponse crcFailedResponse(buffer, sizeof(ServerResponse::ServerResponseHeader) + sizeof(uint32_t));
    if (crcFailedResponse.header._code == ServerResponse::ResponseType::REQUEST_RECEIVED)
    {
        return false;
    }

    cout << "Failed sending FILE_CRC_FAILED_RESENDING message to server: Got bad response type (" << crcFailedResponse.header._code << ")" << endl;
    return false;
}

/*
    Receives the calculates CRC of the last sent file from the server
*/
uint32_t MainClient::getServerCalculatedCRC()
{
    uint8_t buffer[PACKET_SIZE];
    memset(buffer, 0, PACKET_SIZE);

    cout << "MainClient: Receiving file CRC" << endl;

    _clientSocketHandler->receive(buffer, sizeof(ServerResponse::ServerResponseHeader) + sizeof(uint32_t) + UUID_LENGTH + FILE_CONTENT_FIELD_SIZE + FILE_NAME_FIELD_SIZE + CHECKSUM_FIELD_SIZE);
    ServerResponse crcResponse(buffer, sizeof(ServerResponse::ServerResponseHeader) + sizeof(uint32_t) + UUID_LENGTH + FILE_CONTENT_FIELD_SIZE + FILE_NAME_FIELD_SIZE + CHECKSUM_FIELD_SIZE);
    if (crcResponse.header._code != ServerResponse::ResponseType::LAST_SENT_FILE_CRC)
    {
        return 0;
    }
    
    uint32_t fileServerCRC;

    memcpy(&fileServerCRC, crcResponse.payload.payload + UUID_LENGTH + FILE_CONTENT_FIELD_SIZE + FILE_NAME_FIELD_SIZE, CHECKSUM_FIELD_SIZE);

    return fileServerCRC;
}

/*
    Checks if the crc from the server matches the crc calculated by the client.
    Returns true if matches, else false
*/
bool MainClient::handleCRCValidation(uint32_t serverCalculatedCRC, uint32_t clientCalculatedCRC)
{
    uint8_t buffer[PACKET_SIZE];
    memset(buffer, 0, PACKET_SIZE);

    // CRC correct
    if (serverCalculatedCRC == clientCalculatedCRC)
    {
        ClientRequest crcClientResponseToServer(_clientUUID, ClientRequest::RequestCode::FILE_CRC_SUCCESS, 0, NULL);
        crcClientResponseToServer.serializeIntoBuffer(buffer);
        _clientSocketHandler->send(buffer, crcClientResponseToServer.sizeWithPayload());

        memset(buffer, 0, PACKET_SIZE);
        _clientSocketHandler->receive(buffer, sizeof(ServerResponse::ServerResponseHeader));

        ServerResponse crcResponse(buffer, sizeof(ServerResponse::ServerResponseHeader));
        if (crcResponse.header._code == ServerResponse::ResponseType::REQUEST_RECEIVED)
        {
            cout << "SUCCESS! Sent file to server with correct CRC received!" << endl;
            return true;
        }

        cout << "Failed sending final message to server: Got bad response type (" << crcResponse.header._code << ")" << endl;
        return false;
    }

    return false;
}

/*
    Main client runner, encapsulated all od the file sendings and CRC validations needed.
    Return true if the file was uploaded successsfully, false otherwise.
*/
bool MainClient::runClient(string fileName)
{
    uint8_t buffer[PACKET_SIZE];

    for (size_t i = 0; i < MAX_RETRIES_FILE_RESEND; i++)
    {
        if (sendFileToServer(fileName))
        {
            return true;
        }

        if (i < MAX_RETRIES_FILE_RESEND)
        {
            cout << "Resending file due to bad CRC calculation" << endl;
        }
    }

    cout << "Failed sending FILE_CRC_FAILED_FINISHED message to server" << endl;

    // CRC Incorrect after max times, finishing
    ClientRequest crcClientResponseToServer(_clientUUID, ClientRequest::RequestCode::FILE_CRC_FAILED_FINISHED, 0, NULL);
    crcClientResponseToServer.serializeIntoBuffer(buffer);

    _clientSocketHandler->send(buffer, crcClientResponseToServer.sizeWithPayload());

    memset(buffer, 0, PACKET_SIZE);
    _clientSocketHandler->receive(buffer, sizeof(ServerResponse::ServerResponseHeader) + sizeof(uint32_t));

    ServerResponse crcFinalFailedResponse(buffer, sizeof(ServerResponse::ServerResponseHeader) + sizeof(uint32_t));
    if (crcFinalFailedResponse.header._code != ServerResponse::ResponseType::REQUEST_RECEIVED)
    {
        cout << "Failed sending FILE_CRC_FAILED_FINISHED message to server: Got bad response type (" << crcFinalFailedResponse.header._code << ")" << endl;
    }

    return false;
}