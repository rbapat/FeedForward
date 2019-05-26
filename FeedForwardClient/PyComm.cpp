#include "PyComm.h"

PyComm::PyComm(int port)
{
	this->port = port;
	WSAStartup(MAKEWORD(2, 0), &WSAData);
	
	server = socket(AF_INET, SOCK_STREAM, 0);
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(5555);
	bind(server, (SOCKADDR*)& serverAddr, sizeof(serverAddr));
	listen(server, 0);

	int clientAddrSize = sizeof(clientAddr);
	client = accept(server, (SOCKADDR*)& clientAddr, &clientAddrSize);
}

char * PyComm::fetchData(const char* link)
{
	char buf[4];
	send(client, link, strlen(link), 0);
	recv(client, buf, 4, 0);
	int bufLen = std::stoi(buf);
	char* buffer = (char *)malloc(bufLen);
	recv(client, buffer, bufLen, 0);
	return buffer;
}
