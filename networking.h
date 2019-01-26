#define PORT 8080
#include <sys/socket.h> 
#include <netinet/in.h> 

int init_server();
int init_client(char* address);

int try_read(int sock, char* data, int buffer_size, int timeout_ms);