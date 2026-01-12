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
    int n = bind(UDP_Def_Data->sockfd, (struct sockaddr *)&UDP_Def_Data->sockaddr, sizeof(sockaddr_in));
    UDP_Def_Data->sockaddr.sin_family = AF_INET;
    UDP_Def_Data->sockaddr.sin_port = PORT;
    UDP_Def_Data->sockaddr.sin_addr.s_addr = IP;

    if (n < 0)
    {
        cerr << "bind error" << endl;
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






















































// #include "UDP.hpp"
// #include "syst.hpp"

// using namespace std;

// void Usage(string proc)
// {
//     cout<<"Usage:\n\t"<<proc<<" serverip serverport\n"<<endl;
// }

// int main(int argc,char *argv[])
// {
//     if(argc!=3)
//     {
//         Usage(argv[0]);
//         exit(1);
//     }
//     string serverip=argv[1];
//     uint16_t serverport=stoi(argv[2]);

//     //1.创建socket
//     int sockfd=socket(AF_INET,SOCK_DGRAM,0);
//     if(sockfd<0)
//     {
//         cerr<<"socket error"<<endl;
//     }

//     struct sockaddr_in server;
//     memset(&server,0,sizeof(server));
//     server.sin_family=AF_INET;
//     server.sin_port=htons(serverport);
//     server.sin_addr.s_addr=inet_addr(serverip.c_str());

//     struct sockaddr_in local;     // struct sockaddr_in 系统提供的数据类型，local是变量，用户栈上开辟空间
//     bzero(&local, sizeof(local)); // 清空
//     local.sin_family = AF_INET;
//     local.sin_port = htons(serverport); // port要经过网络传输给对面，即port先到网络，所以要将_port,从主机序列转化为网络序列
//     local.sin_addr.s_addr=INADDR_ANY;
//     int n = bind(sockfd,(struct sockaddr*)&local,sizeof(local));
//     if(n<0)
//     {
//         printf("bind error,%s,%d\n", strerror(errno), errno);
//     }

//     string message;
//     //3.直接通信即可
//     while(true)
//     {
//         cout<<"Please Enter# ";
//         getline(cin,message);
//         sendto(sockfd,message.c_str(),message.size(),0,(struct sockaddr*)&server,sizeof(server));

//         struct sockaddr_in peer;
//         socklen_t len=sizeof(peer);
//         char buffer[1024];
//         ssize_t n=recvfrom(sockfd,buffer,sizeof(buffer)-1,0,(struct sockaddr*)&peer,&len);
//         if(n>0)
//         {
//             buffer[n]=0;
//             cout<<"server echo# "<<buffer<<endl;
//         }
//     }
//     return 0;
// }


