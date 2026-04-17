#include "UDP.hpp"
using namespace std;

#define BUFFER_SIZE 8192

//初始化一个UDP连接
int Init_One_UDP_Connect(UDP_Def* UDP_Def_Data, uint32_t IP, uint16_t PORT)
{
    UDP_Def_Data->sockfd = socket(AF_INET, SOCK_DGRAM, 0); //创建一个socket接口
    if (UDP_Def_Data->sockfd < 0)
    {
        cerr << "socket error" << endl;
        return -1;
    }

    uint32_t IP_END = IP>>24;
    if (IP_END == 255) {
        int reuse_opt = 1;
        // 设置SO_REUSEADDR：允许端口复用
        if (setsockopt(UDP_Def_Data->sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse_opt, sizeof(reuse_opt)) == -1) {
            std::cerr << "Failed to set SO_REUSEADDR for broadcast." << std::endl;
            close(UDP_Def_Data->sockfd);
            return -1;
        }
        // 设置SO_REUSEPORT：支持多进程/多线程同时绑定同一端口（Linux特有）
        if (setsockopt(UDP_Def_Data->sockfd, SOL_SOCKET, SO_REUSEPORT, &reuse_opt, sizeof(reuse_opt)) == -1) {
            std::cerr << "Failed to set SO_REUSEPORT for broadcast." << std::endl;
            close(UDP_Def_Data->sockfd);
            return -1;
        }
        // cout << "IP_END is 255, enable port reuse success." << endl;
    }

    // 开启 SO_BROADCAST 选项
    int broadcastEnable = 1;
    if (setsockopt(UDP_Def_Data->sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) == -1) {
        std::cerr << "Failed to enable broadcast." << std::endl;
        close(UDP_Def_Data->sockfd);
        return -1;
    }

    // 设置发送缓冲区大小为 8192 字节
    int bufferSize = BUFFER_SIZE;
    if (setsockopt(UDP_Def_Data->sockfd, SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize)) == -1) {
        std::cerr << "Failed to set send buffer size." << std::endl;
        close(UDP_Def_Data->sockfd);
        return -1;
    }

    int rbufferSize = BUFFER_SIZE;
    if (setsockopt(UDP_Def_Data->sockfd, SOL_SOCKET, SO_RCVBUF, &rbufferSize, sizeof(rbufferSize)) == -1) {
        std::cerr << "Failed to set send buffer size." << std::endl;
        close(UDP_Def_Data->sockfd);
        return -1;
    }

    // int broadcast_permit = 1;
    // if (setsockopt(UDP_Def_Data->sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast_permit, sizeof(broadcast_permit)) < 0) {
    //     perror("setsockopt() failed");
    //     close(UDP_Def_Data->sockfd);
    //     exit(EXIT_FAILURE);
    // }

    memset(&UDP_Def_Data->sockaddr, 0, sizeof(sockaddr_in));
    UDP_Def_Data->sockaddr.sin_family = AF_INET;
    UDP_Def_Data->sockaddr.sin_port = PORT;
    UDP_Def_Data->sockaddr.sin_addr.s_addr = INADDR_ANY;
    int n = ::bind(UDP_Def_Data->sockfd, (struct sockaddr *)&UDP_Def_Data->sockaddr, sizeof(sockaddr_in));
    UDP_Def_Data->sockaddr.sin_family = AF_INET;
    UDP_Def_Data->sockaddr.sin_port = PORT;
    UDP_Def_Data->sockaddr.sin_addr.s_addr = IP;

    if (n < 0)
    {
        cerr << "bind error" << " port:" << PORT << endl;
        return -1;
    }

    return 0;
}

int Close_One_UDP_Connect(UDP_Def* UDP_Def_Data)
{
    close(UDP_Def_Data->sockfd);
    return 0;
}

int UDP_Send_Data(UDP_Def* UDP_Def_Data, uint8_t *data, int len)
{
    sendto(UDP_Def_Data->sockfd, data, len, 0, (struct sockaddr *)&UDP_Def_Data->sockaddr, sizeof(sockaddr_in));
    return 0;
}

int UDP_Rec_Data(UDP_Def* UDP_Def_Data, uint8_t *data, int len)
{
    struct sockaddr_in peer;
    socklen_t peer_len = sizeof(peer);
    int n = recvfrom(UDP_Def_Data->sockfd, data, len, 0, (struct sockaddr *)&peer, &peer_len);
    return n;
}



