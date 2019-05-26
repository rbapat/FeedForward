#pragma once
#pragma comment(lib,"WS2_32")
#include <iostream>
#include <winsock2.h>
#include <string>

class PyComm
{
public:
	PyComm(int port);
	char * fetchData(const char* link);
private:
	int port;
	WSADATA WSAData;
	SOCKET server, client;
	SOCKADDR_IN serverAddr, clientAddr;
};

