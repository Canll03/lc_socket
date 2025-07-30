#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>
using namespace std;
#include "InetAddress.h"
#include "Socket.h"

int main(int argc, char const *argv[])
{
    //检查输入格式
    if(argc != 3)
    {
        cout<< "Using: ./demo idaddress port" <<endl;
        cout<< "Example: ./demo 192.168.6.135 5005" <<endl;
        return -1;
    }

    /*
    //创建服务端的监听标识符
    int listenfd = socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK,IPPROTO_TCP);
    if(listenfd < 0)
    {
        perror("socket()");
        return -1;
    }

    //设置listen的属性
    //1.设置socket的SO_REUSEADDR 选项，允许在 bind() 过程中重用处于TIME_WAIT状态的 socket 地址。用于快速重启服务器程序
    //2.TCP_NODELAY取消Nagle算法，延迟发送小数据包以进行优化，适用于需要低延迟而不关心网络利用率的应用
    //3.启用socket的SO_KEEPALIVE 选项，允许操作系统在检测到空闲连接后发送心跳包，以检测连接是否仍然有效
    //4.设置socket的SO_REUSEPORT 选项，允许多个socket绑定到相同的端口，实现负载均衡或快速重启服务
    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, static_cast<socklen_t>(sizeof opt));
    setsockopt(listenfd, SOL_SOCKET, TCP_NODELAY, &opt, static_cast<socklen_t>(sizeof opt));
    setsockopt(listenfd, SOL_SOCKET, SO_KEEPALIVE, &opt, static_cast<socklen_t>(sizeof opt));
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &opt, static_cast<socklen_t>(sizeof opt));

    //把服务端用于通信的IP地址和端口绑定到socket上
    InetAddress servaddr(argv[1],atoi(argv[2]));
    if((bind(listenfd,servaddr.addr(),sizeof(servaddr))) < 0)
    {
        perror("bind()");
        close(listenfd);
        return -1;
    }

    //把socket设置为可连接的状态
    if(listen(listenfd,128) != 0) 
    {
        perror("listen()");
        close(listenfd);
        return -1;
    }
    */
    Socket servsock(creatnonblocking());
    InetAddress servaddr(argv[1],atoi(argv[2]));
    servsock.setReuseaddr(true);
    servsock.setNodelay(true);
    servsock.setKeepalive(true);
    servsock.setReuseport(true);
    servsock.bindfd(servaddr);
    servsock.listen();

    int epollfd = epoll_create(1);//创建epoll句柄

    //为服务端的listenfd准备读事件
    epoll_event ev;
    ev.data.fd = servsock.fd();
    ev.events = EPOLLIN;//监听读事件

    epoll_ctl(epollfd,EPOLL_CTL_ADD,servsock.fd(),&ev);
    epoll_event evw[10];

    while(1)
    {
        int infd = epoll_wait(epollfd,evw,10,-1);
        if(infd < 0)
        {
            perror("epoll_wait() failed");
            break;
        }
        else if(infd == 0)
        {
            perror("epoll_wait() timeout");
            continue;
        }
        else
        {
            //遍历事件数组
            for(int i = 0;i < infd;i++)
            {
                if(evw[i].events & EPOLLRDHUP) //对方关闭
                {
                    cout<< "client(eventfd = " << evw[i].data.fd << ") disconnected" <<endl;
                    close(evw[i].data.fd);
                }
                else if(evw[i].events & (EPOLLIN|EPOLLPRI))//接收缓冲区中有数据可读
                {
                    if(evw[i].data.fd == servsock.fd())
                    {
                        InetAddress clientaddr;
                        //int clientfd = servsock.accept(clientaddr);
                        Socket* clientsock = new Socket(servsock.accept(clientaddr));
                        if(clientsock->fd() < 0)
                        {
                            perror("accpet()");
                            cout<< "failed fd: " << servsock.fd() <<endl;
                            break;
                        }
                        //setnonblocking(clientfd);

                        cout<< "accpet client(fd = " << clientsock->fd() << ",ipadd = " << clientaddr.ip() << ",port = " << clientaddr.port() << ")" <<endl;

                        //添加新客户端到epoll中
                        ev.data.fd = clientsock->fd();
                        ev.events = EPOLLIN|EPOLLET;
                        epoll_ctl(epollfd,EPOLL_CTL_ADD,clientsock->fd(),&ev);
                    }
                    else
                    {
                        char buffer[1024];
                        while(1)
                        {
                            bzero(buffer,sizeof(buffer));//清空buffer
                            ssize_t nread = read(evw[i].data.fd,buffer,sizeof(buffer));
                            if(nread > 0)
                            {
                                cout<< "recv(eventfd = " << evw[i].data.fd << "): " << buffer <<endl;
                                send(evw[i].data.fd,buffer,strlen(buffer),0);
                            }
                            else if(nread == -1 && errno == EINTR)//读数据时信号中断
                            {
                                continue;
                            }
                            else if(nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))//中断后数据读完
                            {
                                break;
                            }
                            else if(nread == 0)//客户端断开连接
                            {
                                cout<< "client(eventfd = " << evw[i].data.fd << ") disconnected" <<endl;
                                close(evw[i].data.fd);
                                break;
                            }
                        }
                    }
                }
                else if(evw[i].events & EPOLLOUT)//有数据要写
                {

                }
                else
                {
                    perror("client()");
                    close(evw[i].data.fd);
                }
            }
        }
    }

    return 0;
}
