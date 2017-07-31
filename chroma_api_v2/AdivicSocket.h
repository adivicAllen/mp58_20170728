#ifndef ADIVICSOCKET_H
#define ADIVICSOCKET_H

#define ADIVICSOCKET_API __declspec(dllexport)

#include <string>

class QTcpSocket;

class ADIVICSOCKET_API AdivicSocket
{
public:
    explicit AdivicSocket();
    virtual ~AdivicSocket();
    bool Open(const char* host, const char* port = "23", unsigned int timeout = 30);
    void Close();

    void Send(const std::string& msg);
    bool WaitToRead( std::string& msg);

private:
    QTcpSocket* qTcpSocket;

};

#endif