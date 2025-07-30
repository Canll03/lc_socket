#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
using namespace std;

int main(int argc, char const *argv[])
{
    //检查执行编译代码格式
    if(argc != 3) {
        cout<< "Using: ./demo 服务端ip 服务端端口号\n Example: ./demo 192.168.6.135 5005\n" <<endl;
        return -1;
    }

    //第一步：创建客户端的socket
    int sockfd = socket(AF_INET,SOCK_STREAM,0);             //绑定本地网络信息到哪个套接字上(绑定到哪一套协议上)
    // AF_INET表示ipv4协议（地址族） SOCK_STREAM表示tcp协议（套接字类型） 0（套接字协议）
    if(sockfd == -1) {
        perror("socket");return -1;
    }

    //第二步：向服务器发送连接请求
    struct sockaddr_in servaddr;
    memset(&servaddr,0,sizeof(servaddr));                   //清空结构体，防止旧信息的干扰
    servaddr.sin_family = AF_INET;                          //指定协议
    servaddr.sin_port = htons(stoi(argv[2]));               //指定连接服务端的端口号
    
    struct hostent* h;                                      //存放服务端IP地址的结构体的指针
    if((h = gethostbyname(argv[1])) == nullptr) {           //把域名/主机名/字符串形式的IP地址转换为结构体
        cout<< "gethostbyname failed\n" <<endl;
        close(sockfd);                                      //关闭socket
        return -1;
    }
    memcpy(&servaddr.sin_addr,h->h_addr_list[0],h->h_length);  //指定服务端的IP（大端序）

    //第三步：与服务端通信
    if(connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) == -1) {
        perror("connect");
        close(sockfd);
        return -1;
    }

    char buffer[1024];
    for(int i = 0;i < 10;i++) {
        int iret;
        memset(buffer,0,sizeof(buffer));
        //sprintf(buffer,"这是第%d个超级女生，编号：%3d",i + 1,i + 1);
        cout<< "Please input:";
        cin >> buffer;
        if((iret = send(sockfd,buffer,strlen(buffer),0)) <= 0) {
            perror("send");
            break;
        }

        //cout<< "发送：" << buffer <<endl;
        memset(buffer,0,sizeof(buffer));

        //接受服务端的报文，如果服务端没有发送报文，recv()将阻塞等待
        if((iret = recv(sockfd,buffer,sizeof(buffer),0)) <= 0) {
            cout<< "iret = " << iret <<endl;
            break;
        }

        //sleep(1);
        cout<< "接收：" << buffer <<endl; 
    }

    //第四步：关闭socket
    close(sockfd);

    return 0;
}
