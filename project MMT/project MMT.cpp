#include"Header.h"

int main(int argc, char const* argv[])
{
	int server_fd;
	int new_socket; // cho s
	int valread;	// bien doc

	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof{address};
	char buffer[1024] = { 0 };
	char* hello = "hello from server";

	// creating socket file 
	if((server_fd = socket()))
}