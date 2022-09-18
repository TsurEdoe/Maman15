#include "Protocol.h"

void Protocol::parseTypeToBuffer(ProtocolType protocolType, uint8_t* buffer)
{
    buffer = std::to_string(htons(int)protocolType).c_str();
}

ProtocolType Protocol::parseTypeFromBuffer(uint8_t* bufferFromSocket)
{
    if (bufferForSocket == NULL || strlen_s(bufferFromSocket) < TYPE_LENGTH)
    {
        return ProtocolType::ERROR_CODE;
    }

    uint8_t typeBuffer[TYPE_LENGTH];
    memcpy_s(typeBuffer, bufferFromSocket, TYPE_LENGTH)

    return atoi(typeBuffer);
}
    
bool Protocol::parseMessageToBuffer(ProtocolType protocolType, uint8_t* payload, uint8_t* bufferForSocket)
{
    size_t payloadLength = strlen_s(payload);
    size_t bufferLength = strlen_s(bufferForSocket);
    parseTypeToBuffer(protocolType, bufferForSocket);

    if (payload == NULL || bufferForSocket == NULL)
    {
        cout << "ERROR: Can't parse message to buffer, null buffer sent" << endl;
        return false;
    }

    if (bufferLength < TYPE_LENGTH)
    {
        cout << "ERROR: Can't parse message to buffer, buffer too short for type" << endl;
        return false;
    }

    if (payloadLength > bufferLength - TYPE_LENGTH)
    {
        cout << "ERROR: Can't parse message to buffer, buffer to short for payload" << endl;
        return false;
    }

    srtcpy_s(bufferForSocket, payload, payloadLength);
    return true;
}

ProtocolType Protocol::parseFromBuffer(uint8_t* payload, uint8_t* , size_t payloadSize)
{
    size_t payloadLength = strlen_s(payload);
    size_t bufferLength = strlen_s(bufferFromSocket);

    if (payload == NULL || bufferFromSocket == NULL)
    {
        cout << "ERROR: Can't parse message from buffer, null buffer sent" << endl;
        return ProtocolType::ERROR_CODE;
    }

    if (bufferLength < TYPE_LENGTH)
    {
        cout << "ERROR: Can't parse message from buffer, buffer too short for type" << endl;
        return ProtocolType::ERROR_CODE;
    }

    if (payloadLength < payloadSize)
    {
        cout << "ERROR: Can't parse message from buffer, payload too short" << endl;
        return ProtocolType::ERROR_CODE;
    }

    srtcpy_s(payload, bufferFromSocket, payloadSize);

    return parseTypeFromBuffer(bufferFromSocket);
}