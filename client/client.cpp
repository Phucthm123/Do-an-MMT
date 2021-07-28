#include<iostream>
#include<string>
#include<WS2tcpip.h>

#define PORT 8080
#define BUF_SIZE 4096

#pragma comment(lib,"ws2_32.lib")

int main()
{
	std::string ipAddress = "127.0.0.1";

	//Initialize Winsock
	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data); 
	if (wsResult != 0)
	{
		std::cout << "Can't start Winsock !" << "\n";
		return 1 ;
	}
	// Create socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock == INVALID_SOCKET)
	{
		std::cout << "Can't create socket !" << "\n";
		return 1 ;
	}

	// Fill in a hint structure
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(PORT);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);
	
	// Connect to server
	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		std::cout << "Can't connect to server !" << WSAGetLastError() << "\n";
		closesocket(sock);
		WSACleanup();
		return 1;
	}



	// A while loop to send and receive data
	char buf[BUF_SIZE];
	std::string userInput;

	do
	{
		std::cout << "> ";
		getline(std::cin, userInput);

		if (userInput.size() > 0)
		{
			int sendmess = send(sock, userInput.c_str(), userInput.size() + 1, 0);
			if (sendmess != SOCKET_ERROR)
			{
				ZeroMemory(buf, BUF_SIZE);
				int byteReceive = recv(sock, buf, BUF_SIZE, 0);
				if (byteReceive > 0)
				{
					std::cout << "SERVER ->" << std::string(buf, 0, byteReceive) << "\n";
				}
			}
		}

	} while (userInput.size() > 0);
	closesocket(sock);
	WSACleanup();
	return 0;
}