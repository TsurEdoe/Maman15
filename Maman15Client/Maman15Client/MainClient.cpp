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
        cout << "MainClient - ERROR: Failed creating MainClient, Failed creating encryptoin handler" << endl;
    }

    if (!_encryptionHandler->initializeHandler(clientUUID, userName))
    {
        delete _encryptionHandler;
        throw std::runtime_error("MainClient - ERROR: Failed initizing encryption handler");
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

    cout << "MainClient - Started file CRC validation on " << fileName << endl;

    if (handleCRCValidation(getServerCalculatedCRC(), _fileHandler->calculateFileCRC(fileName), fileName))
    {
        return true;
    }

    cout << "MainClient - Wrong CRC calculated for file " << fileName << " by server, resending file" << endl;

    string paddedFileName = fileName;
    EncryptionHandler::padStringWithZeroes(paddedFileName, FILE_NAME_FIELD_SIZE);

    uint8_t payload[UUID_LENGTH + FILE_NAME_FIELD_SIZE];

    memcpy(payload, this->_clientUUID, UUID_LENGTH);
    memcpy(payload + UUID_LENGTH, paddedFileName.c_str(), FILE_NAME_FIELD_SIZE);

    // CRC Incorrect, resending
    ClientRequest crcClientResponseToServer(_clientUUID, ClientRequest::RequestCode::FILE_CRC_FAILED_RESENDING, 
        UUID_LENGTH + FILE_NAME_FIELD_SIZE, payload);

    crcClientResponseToServer.serializeIntoBuffer(buffer);

    _clientSocketHandler->send(buffer, crcClientResponseToServer.sizeWithPayload());

    memset(buffer, 0, PACKET_SIZE);
    _clientSocketHandler->receive(buffer, sizeof(ServerResponse::ServerResponseHeader) + sizeof(uint32_t));

    ServerResponse crcFailedResponse(buffer, sizeof(ServerResponse::ServerResponseHeader) + sizeof(uint32_t));
    if (crcFailedResponse.header._code == ServerResponse::ResponseType::REQUEST_RECEIVED)
    {
        return false;
    }

    cout << "MainClient - ERROR: Failed sending FILE_CRC_FAILED_RESENDING message to server: Got bad response type (" <<
        crcFailedResponse.header._code << ")" << endl;
    return false;
}

/*
    Receives the calculates CRC of the last sent file from the server
*/
uint32_t MainClient::getServerCalculatedCRC()
{
    uint8_t buffer[PACKET_SIZE];
    memset(buffer, 0, PACKET_SIZE);

    cout << "MainClient - Receiving file CRC" << endl;

    size_t fileCRCMessageSize = 
        sizeof(ServerResponse::ServerResponseHeader) + // header
        sizeof(uint32_t) + // payload size
        UUID_LENGTH + FILE_CONTENT_FIELD_SIZE + FILE_NAME_FIELD_SIZE + CHECKSUM_FIELD_SIZE; // payload

    _clientSocketHandler->receive(buffer, fileCRCMessageSize);
    ServerResponse crcResponse(buffer, fileCRCMessageSize);
    if (crcResponse.header._code != ServerResponse::ResponseType::LAST_SENT_FILE_CRC)
    {
        return 0;
    }
    
    uint32_t fileServerCRC;

    memcpy(&fileServerCRC, 
        crcResponse.payload.payload + UUID_LENGTH + FILE_CONTENT_FIELD_SIZE + FILE_NAME_FIELD_SIZE, 
        CHECKSUM_FIELD_SIZE);

    return fileServerCRC;
}

/*
    Checks if the crc from the server matches the crc calculated by the client.
    Returns true if matches, else false
*/
bool MainClient::handleCRCValidation(uint32_t serverCalculatedCRC, uint32_t clientCalculatedCRC, string fileName)
{
    uint8_t buffer[PACKET_SIZE];
    memset(buffer, 0, PACKET_SIZE);

    // CRC correct
    if (serverCalculatedCRC == clientCalculatedCRC)
    {
        string paddedFileName = fileName;
        EncryptionHandler::padStringWithZeroes(paddedFileName, FILE_NAME_FIELD_SIZE);

        uint8_t payload[UUID_LENGTH + FILE_NAME_FIELD_SIZE];

        memcpy(payload, this->_clientUUID, UUID_LENGTH);
        memcpy(payload + UUID_LENGTH, paddedFileName.c_str(), FILE_NAME_FIELD_SIZE);

        ClientRequest crcClientResponseToServer(_clientUUID, ClientRequest::RequestCode::FILE_CRC_SUCCESS, 
            UUID_LENGTH + FILE_NAME_FIELD_SIZE, payload);
        crcClientResponseToServer.serializeIntoBuffer(buffer);
        _clientSocketHandler->send(buffer, crcClientResponseToServer.sizeWithPayload());

        memset(buffer, 0, PACKET_SIZE);
        _clientSocketHandler->receive(buffer, sizeof(ServerResponse::ServerResponseHeader));

        ServerResponse crcResponse(buffer, sizeof(ServerResponse::ServerResponseHeader));
        if (crcResponse.header._code == ServerResponse::ResponseType::REQUEST_RECEIVED)
        {
            cout << "MainClient - SUCCESS! Sent file to server with correct CRC received!" << endl;
            return true;
        }

        cout << "MainClient - ERROR: Failed sending final message to server: Got bad response type (" 
            << crcResponse.header._code << ")" << endl;
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
            cout << "MainClient - Resending file due to bad CRC calculation" << endl;
        }
    }

    cout << "MainClient - ERROR: Failed sending FILE_CRC_FAILED_FINISHED message to server" << endl;

    string paddedFileName = fileName;
    EncryptionHandler::padStringWithZeroes(paddedFileName, FILE_NAME_FIELD_SIZE);

    uint8_t payload[UUID_LENGTH + FILE_NAME_FIELD_SIZE];

    memcpy(payload, this->_clientUUID, UUID_LENGTH);
    memcpy(payload + UUID_LENGTH, paddedFileName.c_str(), FILE_NAME_FIELD_SIZE);

    // CRC Incorrect after max times, finishing
    ClientRequest crcClientResponseToServer(_clientUUID, ClientRequest::RequestCode::FILE_CRC_FAILED_FINISHED, 
        UUID_LENGTH + FILE_NAME_FIELD_SIZE, payload);
    crcClientResponseToServer.serializeIntoBuffer(buffer);

    _clientSocketHandler->send(buffer, crcClientResponseToServer.sizeWithPayload());

    memset(buffer, 0, PACKET_SIZE);
    _clientSocketHandler->receive(buffer, sizeof(ServerResponse::ServerResponseHeader) + sizeof(uint32_t));

    ServerResponse crcFinalFailedResponse(buffer, sizeof(ServerResponse::ServerResponseHeader) + sizeof(uint32_t));
    if (crcFinalFailedResponse.header._code != ServerResponse::ResponseType::REQUEST_RECEIVED)
    {
        cout << "MainClient - ERROR: Failed sending FILE_CRC_FAILED_FINISHED message to server: Got bad response type ("
            << crcFinalFailedResponse.header._code << ")" << endl;
    }

    return false;
}