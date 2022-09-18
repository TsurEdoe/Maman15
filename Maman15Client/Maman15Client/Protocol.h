#include <string>

#define TYPE_LENGTH 3

class Protocol
{    
private:
    uint8_t* parseTypeToBuffer(ProtocolType protocolType);
    ProtocolType parseTypeFromBuffer(uint8_t* bufferFromSocket);
public:
    
    bool parseMessageToBuffer(ProtocolType protocolType, uint8_t* payload, uint8_t* bufferForSocket);
    ProtocolType parseFromBuffer( uint8_t* payload, uint8_t* bufferFromSocket);

    enum ProtocolType 
    {
        ERROR_CODE = -1
    }
    
    enum RequestType : public ProtocolType
    {
        CLIENT_REGISTRATION = 101,
        CLIENT_PUBLIC_KEY = 102,
        CLIENT_ENCRYPTED_FILE = 103,
        FILE_CRC_FAILED = 104,
        FILE_CRC_SUCCESS = 105
    };

    enum ResponseType : public ProtocolType
    {
        REGISTRATION_SUCCESS = 200,
        REGISTRATION_FAILED = 301,
        CLIENT_AES_KEY = 201,
        LAST_SENT_FILE_CRC = 202
    };
}