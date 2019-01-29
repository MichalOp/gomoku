#include <stdlib.h> 
#include <string.h> 
#include "networking.h"
#include <poll.h>
#include <sys/select.h>

int wait_for_connection(int s, int timeout){

    int iResult;
    struct timeval tv;
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(s, &rfds);

    tv.tv_sec = 0;
    tv.tv_usec = (long)timeout*1000;

    iResult = select(s+1, &rfds, (fd_set *) 0, (fd_set *) 0, &tv);
    if(iResult > 0)
    {
        return accept(s, NULL, NULL);
    }
    else
    {
        printf("waiting");
    }
    return 0;
}

int try_read(int sock, char* data, int buffer_size, int timeout_ms){
    struct pollfd fd;
    int ret;
    fd.fd = sock;
    fd.events = POLLIN;
    ret = poll(&fd, 1, timeout_ms);
    switch (ret) {
        case -1:
            return -1;
        case 0:
            return 0;
        default:
            return recv(sock,data, buffer_size, 0);
    }
}

int init_server(){

    struct sockaddr_in sockaddr_data;
    int server_fd, new_socket, valread; 
    int opt = 1; 
    int addrlen = sizeof(sockaddr_data); 

    printf("INITIALIZING SERVER\n");

    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
    //printf("here\n");
    
    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    
    //printf("here\n");
    sockaddr_data.sin_family = AF_INET; 
    sockaddr_data.sin_addr.s_addr = INADDR_ANY; 
    sockaddr_data.sin_port = htons( PORT ); 
    
    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)&sockaddr_data,  
                                 sizeof(sockaddr_data))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    
    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    
    return server_fd;  
}

int init_client(char* address){

    struct sockaddr_in sockaddr_data;
    int sock = 0, valread;
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
       exit(EXIT_FAILURE); 
    } 
   
    memset(&sockaddr_data, '0', sizeof(sockaddr_data)); 
   
    sockaddr_data.sin_family = AF_INET; 
    sockaddr_data.sin_port = htons(PORT); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, address, &sockaddr_data.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        exit(EXIT_FAILURE); 
    } 
   
    if (connect(sock, (struct sockaddr *)&sockaddr_data, sizeof(sockaddr_data)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        exit(EXIT_FAILURE); 
    } 
    return sock; 

}