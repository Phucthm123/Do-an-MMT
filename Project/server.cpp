#include<iostream>
#include<WS2tcpip.h>+
#include<winsock2.h>
#include<stdio.h>

#define PORT 8080
#define IP "127.0.0.1"
#define BUF_SIZE 2048

#pragma comment (lib, "ws2_32.lib")

void main()
{
	// Initialze winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);
	int iResult = WSAStartup(ver, &wsData);
	if (iResult != 0)
	{
		std::cout << "Can't Initialize winsock! Try again" << "\n";
		return;
	}

	// Create the socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		std::cout << "Can't create socket!" << "\n";
		return;
	}

	// Bind the socket to an IP address and port
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(PORT);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell Winsock the socketis for listening

	listen(listening, SOMAXCONN);

	// Wait for a connection
	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

	char host[NI_MAXHOST];
	char service[NI_MAXHOST];

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXHOST);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXHOST, 0) == 0)
	{
		std::cout << host << " connected on port " << service << "\n";

	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << " connected on port " << service << ntohs(client.sin_port) << "\n";

	}

	//close listening socket

	closesocket(listening);

	// while loop to chat between server and client


	char buf[BUF_SIZE];

	while (true)
	{
		ZeroMemory(buf, BUF_SIZE);

		int byteReceived = recv(clientSocket, buf, BUF_SIZE, 0);
		if (byteReceived == SOCKET_ERROR)
		{
			std::cout << "Error in recv() !" << "\n";
			break;
		}

		if (byteReceived == 0)
		{
			std::cout << "Client disconnected !" << "\n";
			break;
		}
		send(clientSocket, buf, byteReceived + 1, 0);
	}

	closesocket(clientSocket);

	// Cleanup winsock
	WSACleanup();

}
