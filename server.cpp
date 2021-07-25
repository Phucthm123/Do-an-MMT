#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/shm.h>
#include<thread>
#include<iostream>

#define PORT 8080  
#define QUEUE 20

int conn;

void thread_task()
{

}

int main(int argc, char const *argv[])
{
     int ss = socket(AF_INET,SOCK_STREAM,0);
     struct sockaddr_in server_sockaddr;
     server_sockaddr.sin_family =AF_INET;
     server_sockaddr.sin_port = htons (PORT);
     server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

     if(bind(ss,(struct sockaddr*)&server_sockaddr, sizeof(server_sockaddr)) == -1)
     {
         perror("bind");
         exit(1);
     }
    if(listen(ss,QUEUE) == -1)
    {
        perror ("listen");
        exit(1);
    }
    
    std::cout << "Waitting for client !\n";
    struct sockaddr_in client_addr;
    socklen_t length  = sizeof(client_addr);
    conn =  accept(ss,(struct sockaddr*)&client_addr, & length);
    if (conn < 0)
    {
        perror("connect");
        exit(1);
    }

    char buffer[1024];
    char buf[1024];
    while(true)
    {
        

        int len = recv(conn, buffer, sizeof(buffer),0);
        if(strcmp(buffer,"exit\n") == 0) break;
        std::cout << "client: " << buffer << "\n";        
        //memset(buffer,0,sizeof(buffer));
        //send(conn,buffer,len,0);
        //memset(buf,0,sizeof(buf));
           if(fgets(buf,sizeof(buf), stdin) != NULL)
        {
            send(conn,buf, sizeof(buf),0);
        }
    }

    close(conn);
    close(ss);

    return 0;
}   
