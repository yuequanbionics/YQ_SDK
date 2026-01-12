#ifndef UDP_H_
#define UDP_H_
#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct{
    int sockfd;
    struct sockaddr_in sockaddr;
}UDP_Def;

int Init_One_UDP_Connect(UDP_Def* UDP_Def_Data, uint32_t IP, uint16_t PORT);
int Close_One_UDP_Connect(UDP_Def* UDP_Def_Data);
int UDP_Send_Data(UDP_Def* UDP_Def_Data, uint8_t *data, int len);
int UDP_Rec_Data(UDP_Def* UDP_Def_Data, uint8_t *data, int len);

#endif 



