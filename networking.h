#define PORT 2137
#include <sys/socket.h> 
#include <netinet/in.h> 

typedef struct sockaddr_in sockaddr_in;

typedef struct connection_data_t{
    int is_server;
    int sock;
    sockaddr_in server_info;
    int newsocket;
}connection_data;

connection_data* init_server();
connection_data* init_client(char* address);

int send(char* data, connection_data* connection);
int recv(char* data, connection_data* connection);