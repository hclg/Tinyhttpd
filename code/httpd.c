#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <httpd.h>
#include <sys/stat.h>


const int size = 1024;


int startup(u_short *port) {
    int sock;
    struct sockaddr_in addr;
    socklen_t addr_len;
    int on;
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket error");
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(*port);
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, on, &on) == -1) 
        error_handling("setsockopt error");
    if (bind(sock, (struct sockaddr *) &addr, addr_len) == -1) 
        error_handling("bind error");
    
    if (*port == 0) {
        if (getsockname(sock, (struct sockaddr*) &addr, &addr_len) == -1) 
            error_handling("getsockname error");
        *port = htons(addr.sin_port);
    }
    if (listen(sock, 5) == -1) 
        error_handling("listen error");
    return sock;
}


void error_handling(const char *msg) {
    perror(msg);
    exit(1);
}

void *accept_request(void *arg) {
    struct stat sta;
    char buff[size], method[255];
    int buff_len;
    
}

int main(void) {
    u_short port = 4040;
    int ser_sock;
    int cli_sock;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    pthread_t newthread;
    
    ser_sock = startup(&port);
    printf("httpd running on port: %d\n", port);

    while (1) {
        cli_sock = accept(ser_sock, (struct sockaddr*) &client_addr, &client_addr_len);
        if (cli_sock == -1) error_handling("accept error");
        if (pthread_create(&newthread, NULL, (void *)accept_request, (void *)(intptr_t) cli_sock) != 0)
            error_handling("pthread_create error");
    }
    close(ser_sock);

    return 0;
}
