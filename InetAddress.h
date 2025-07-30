#pragma once
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <string>

class InetAddress
{
private:
    sockaddr_in m_servaddr;
public:
    InetAddress();
    InetAddress(const std::string& ip,const uint16_t port);//监听fd的构造函数
    InetAddress(const sockaddr_in addr);                   //连接上来的fd构造函数
    ~InetAddress();

    const char* ip() const;                                //将IP地址转换成字符串     
    const uint16_t port() const;                           //返回整数表示的端口号
    const sockaddr* addr() const;                          //将m_servaddr类型转换成sockaddr
    void setaddr(sockaddr_in clientaddr);  
};
