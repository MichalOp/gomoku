#define PORT 8080
#include <sys/socket.h> 
#include <netinet/in.h> 

int init_server();
int wait_for_connection(int sock, int timeout_ms);
int init_client(char* address);

int try_read(int sock, char* data, int buffer_size, int timeout_ms);