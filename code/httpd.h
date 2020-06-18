

#ifndef HTTPD__H
#define HTTPD__H

#ifdef __cplusplus
extern "C"
{
#endif

#define ISspace(x) isspace((int)(x))

#define SERVER_NAME "Server: HCLtinyhttp/0.1.1\r\n"
#define STDIN 0
#define STDOUT 1
#define STDERR 2

/*
    通过对应端口启动web服务器的套接字
    参数： 端口
    返回值： web服务器的套接字
*/
int startup(u_short *port);


#ifdef __cplusplus
}
#endif



#endif