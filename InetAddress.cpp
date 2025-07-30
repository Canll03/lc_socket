#include "InetAddress.h"

InetAddress::InetAddress()
{
    
}

InetAddress::InetAddress(const std::string& ip,uint16_t port)//监听fd的构造函数
{
    m_servaddr.sin_family = AF_INET;
    m_servaddr.sin_addr.s_addr = inet_addr(ip.c_str());
    m_servaddr.sin_port = htons(port);
}

InetAddress::InetAddress(const sockaddr_in addr):m_servaddr(addr)//连接上来的fd构造函数
{
    
}

InetAddress::~InetAddress()
{

}

const char* InetAddress::ip() const                                 //将IP地址转换成字符串     
{
    return inet_ntoa(m_servaddr.sin_addr);
}

const uint16_t InetAddress::port() const                            //返回整数表示的端口号
{
    return ntohs(m_servaddr.sin_port);
}

const sockaddr* InetAddress::addr() const                           //将m_servaddr类型转换成sockaddr  
{
    return (sockaddr*)&m_servaddr;
}

void InetAddress::setaddr(sockaddr_in clientaddr)
{
    m_servaddr = clientaddr;
}
