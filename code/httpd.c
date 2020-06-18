#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <httpd.h>

int startup(u_short *port) {
    int sock;

}



int main(void) {
    u_short port = 4040;
    int ser_sock;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    pthread_t newthread;
    
    ser_sock = startup(&port);
    printf("httpd running on port: %d\n", port);
    return 0;
}
