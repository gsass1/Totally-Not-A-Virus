#include "stdafx.h"
#include "Network.h"
#include "Hax.h"
#include "Settings.h"

Network network;

Network::Network() : bufLen(sizeof(buf))
{
    WSAStartup(MAKEWORD(2, 0), &WSAData);
}


Network::~Network()
{
    WSACleanup();
}

std::string Network::SendPost(const char* msg, size_t len)
{
    std::string request =
                "POST " V_NET_FILE " HTTP/1.1\r\n"
                "Host: " V_NET_DOMAIN "\r\n"
                "Content-Type: application/x-www-form-urlencoded\r\n"
                "Content-Length: ";
    request += std::to_string(len);
    request += "\r\n\r\n";
    request += msg;
    request += "\r\n";

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

    struct hostent *host;
	host = gethostbyname(V_NET_DOMAIN);

    SOCKADDR_IN sin;
	sin.sin_addr.s_addr = *((unsigned long*)host->h_addr);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(80);

    if (connect(sock, (struct sockaddr *) &sin, sizeof(sin)) < 0)
    {
        Error(_T("connect() failed"));
        return "";
    }

    if (send(sock, request.c_str(), request.length(), 0) != request.length())
    {
        Error(_T("send() sent a different number of bytes than expected"));
        closesocket(sock);
        return "";
    }

    int recv_len = recv(sock, buf, (this->bufLen-1), 0);
    buf[recv_len] = '\0';

    char* start = buf;
    int linebreaks = 0;

    while(*start != '\0') {
        switch(*start) {
        case '\r':
        case '\n':
            ++linebreaks;
            break;
        default:
            linebreaks = 0;
        }

        ++start;

        if (linebreaks == 4) {
            break;
        }
    }

    closesocket(sock);

    return std::string(start);
}