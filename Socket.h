#include "Socket.h"

int creatnonblocking()
{
    //创建非阻塞的服务端的监听标识符
    int listenfd = socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK,IPPROTO_TCP);
    if(listenfd < 0)
    {
        perror("socket()");
        exit(-1);
    }
    return listenfd;
}

Socket::Socket(int fd):m_fd(fd)
{

}

Socket::~Socket()
{
    ::close(m_fd);
}

int Socket::fd() const
{
    return m_fd;
}

void Socket::setReuseaddr(bool on)
{
    int opt = on ? 1 : 0;
    ::setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &opt, static_cast<socklen_t>(sizeof opt));
}

void Socket::setNodelay(bool on)
{
    int opt = on ? 1 : 0;
    ::setsockopt(m_fd, SOL_SOCKET, TCP_NODELAY, &opt, static_cast<socklen_t>(sizeof opt));
}

void Socket::setKeepalive(bool on)
{
    int opt = on ? 1 : 0;
    ::setsockopt(m_fd, SOL_SOCKET, SO_KEEPALIVE, &opt, static_cast<socklen_t>(sizeof opt));
}

void Socket::setReuseport(bool on)
{
    int opt = on ? 1 : 0;
    ::setsockopt(m_fd, SOL_SOCKET, SO_REUSEPORT, &opt, static_cast<socklen_t>(sizeof opt));
}

void Socket::bindfd(const InetAddress& servaddr)
{
    if((::bind(m_fd,servaddr.addr(),sizeof(servaddr))) < 0)
    {
        perror("bind()");
        close(m_fd);
        exit(-1);
    }
}

void Socket::listen(int nn)
{
    if(::listen(m_fd,nn) != 0) 
    {
        perror("listen()");
        close(m_fd);
        exit(-1);
    }
}

int Socket::accept(InetAddress& clientaddr)
{
    struct sockaddr_in peeraddr;
    socklen_t len = sizeof(peeraddr);
    int clientfd = accept4(m_fd,(struct sockaddr*)&peeraddr,&len,SOCK_NONBLOCK);
    clientaddr.setaddr(peeraddr);

    return clientfd;
}
