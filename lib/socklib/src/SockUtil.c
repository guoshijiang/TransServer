#include "PoolSock.h"
#include "SockLog.h"

ssize_t readn(int fd, void *buf, size_t count)
{
	size_t nleft = count;
	ssize_t nread;
	char *bufp = (char*)buf;

	while (nleft > 0)
	{
		if ((nread = read(fd, bufp, nleft)) < 0)
		{
			if (errno == EINTR)
				continue;
			return -1;
		}
		else if (nread == 0)
			return count - nleft;

		bufp += nread;
		nleft -= nread;
	}

	return count;
}

ssize_t writen(int fd, const void *buf, size_t count)
{
	size_t nleft = count;
	ssize_t nwritten;
	char *bufp = (char*)buf;

	while (nleft > 0)
	{
		if ((nwritten = write(fd, bufp, nleft)) < 0)
		{
			if (errno == EINTR)
				continue;
			return -1;
		}
		else if (nwritten == 0)
			continue;

		bufp += nwritten;
		nleft -= nwritten;
	}

	return count;
}

ssize_t recv_peek(int sockfd, void *buf, size_t len)
{
	while (1)
	{
		int ret = recv(sockfd, buf, len, MSG_PEEK);
		if (ret == -1 && errno == EINTR)
			continue;
		return ret;
	}
}

int activate_nonblock(int fd)
{
	int ret = 0;
	int flags = fcntl(fd, F_GETFL);
	if (flags == -1)
	{
		ret = flags;
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func activate_nonblock() err");
		return ret;
	}
		
	flags |= O_NONBLOCK;
	ret = fcntl(fd, F_SETFL, flags);
	if (ret == -1)
	{
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func activate_nonblock() err");
		return ret;
	}
	return ret;
}

int deactivate_nonblock(int fd)
{
	int ret = 0;
	int flags = fcntl(fd, F_GETFL);
	if (flags == -1)
	{
		ret = flags;
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func deactivate_nonblock() err");
		return ret;
	}

	flags &= ~O_NONBLOCK;
	ret = fcntl(fd, F_SETFL, flags);
	if (ret == -1)
	{
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func deactivate_nonblock() err");
		return ret;
	}
	return ret;
}

static int connect_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds)
{
	int ret;
	socklen_t addrlen = sizeof(struct sockaddr_in);

	if (wait_seconds > 0)
		activate_nonblock(fd);

	ret = connect(fd, (struct sockaddr*)addr, addrlen);
	if (ret < 0 && errno == EINPROGRESS)
	{
		fd_set connect_fdset;
		struct timeval timeout;
		FD_ZERO(&connect_fdset);
		FD_SET(fd, &connect_fdset);
		timeout.tv_sec = wait_seconds;
		timeout.tv_usec = 0;
		do
		{
			ret = select(fd + 1, NULL, &connect_fdset, NULL, &timeout);
		} while (ret < 0 && errno == EINTR);
		if (ret == 0)
		{
			ret = -1;
			errno = ETIMEDOUT;
		}
		else if (ret < 0)
			return -1;
		else if (ret == 1)
		{
			int err;
			socklen_t socklen = sizeof(err);
			int sockoptret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &socklen);
			if (sockoptret == -1)
			{
				return -1;
			}
			if (err == 0)
			{
				ret = 0;
			}
			else
			{
				errno = err;
				ret = -1;
			}
		}
	}
	if (wait_seconds > 0)
	{
		deactivate_nonblock(fd);
	}
	return ret;
}

int write_timeout(int fd, unsigned int wait_seconds)
{
	int ret = 0;
	if (wait_seconds > 0)
	{
		fd_set write_fdset;
		struct timeval timeout;

		FD_ZERO(&write_fdset);
		FD_SET(fd, &write_fdset);

		timeout.tv_sec = wait_seconds;
		timeout.tv_usec = 0;
		do
		{
			ret = select(fd + 1, NULL, &write_fdset, NULL, &timeout);
		} while (ret < 0 && errno == EINTR);

		if (ret == 0)
		{
			ret = -1;
			errno = ETIMEDOUT;
		}
		else if (ret == 1)
			ret = 0;
	}

	return ret;
}

int read_timeout(int fd, unsigned int wait_seconds)
{
	int ret = 0;
	if (wait_seconds > 0)
	{
		fd_set read_fdset;
		struct timeval timeout;

		FD_ZERO(&read_fdset);
		FD_SET(fd, &read_fdset);

		timeout.tv_sec = wait_seconds;
		timeout.tv_usec = 0;
		do
		{
			ret = select(fd + 1, &read_fdset, NULL, NULL, &timeout);
		} while (ret < 0 && errno == EINTR); 

		if (ret == 0)
		{
			ret = -1;
			errno = ETIMEDOUT;
		}
		else if (ret == 1)
			ret = 0;
	}

	return ret;
}

int sckClient_init()
{
	return 0; 
}


int sckClient_connect(char *ip, int port, int connecttime, int *connfd)
{
	int ret = 0;

	if (ip==NULL || connfd==NULL || port<=0 || port>65537 || connecttime < 0)
	{
		ret = Sck_ErrParam;
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func sckClient_connect() err, check  (ip==NULL || connfd==NULL || port<=0 || port>65537 || connecttime < 0)");
		return ret;
	}
	int sockfd;
	sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd < 0)
	{
		ret = errno;
		printf("func socket() err:  %d\n", ret);
		return ret;
	}
	
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = inet_addr(ip);
	
    ret = connect_timeout(sockfd, (struct sockaddr_in*) (&servaddr), (unsigned int )connecttime);
    if (ret < 0)
    {
    	if (ret==-1 && errno == ETIMEDOUT)
    	{
    		ret = Sck_ErrTimeOut;
    		return ret;
    	}
    	else
    	{
    		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func connect_timeout() err");
    	}
    }
    
    *connfd = sockfd;
   
   	return ret;
	
}

int sckClient_send(int connfd, int sendtime, unsigned char *data, int datalen)
{
	int 	ret = 0;
	
	if (data == NULL || datalen <= 0)
	{
		ret = Sck_ErrParam;
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func sckClient_send() err, check (data == NULL || datalen <= 0)");
		return ret;
	} 
	
	ret = write_timeout(connfd, sendtime);
	if (ret == 0)
	{
		int writed = 0;
		unsigned char *netdata = ( unsigned char *)malloc(datalen + 4);
		if ( netdata == NULL)
		{
			ret = Sck_ErrMalloc;
			printf("func sckClient_send() mlloc Err:%d\n ", ret);
			return ret;
		}
		int netlen = htonl(datalen);
		memcpy(netdata, &netlen, 4);
		memcpy(netdata+4, data, datalen);
		
		writed = writen(connfd, netdata, datalen + 4);
		if (writed < (datalen + 4) )
		{
			if (netdata != NULL) 
			{
				free(netdata);
				netdata = NULL;
			}
			return writed;
		}
		
		if (netdata != NULL)
		{
			free(netdata);
			netdata = NULL;
		}  
	}
	
	if (ret < 0)
	{
		if (ret == -1 && errno == ETIMEDOUT)
		{
			ret = Sck_ErrTimeOut;
			printf("func sckClient_send() mlloc Err:%d\n ", ret);
			return ret;
		}
		return ret;
	}
	
	return ret;
}

int sckClient_rev(int connfd, int revtime, unsigned char **out, int *outlen)
{
	
	int		ret = 0;
	unsigned char *tmpBuf = NULL;
	
	if (out==NULL || outlen==NULL)
	{
		ret = Sck_ErrParam;
		printf("func sckClient_rev() timeout , err:%d \n", Sck_ErrTimeOut);
		return ret;
	}
	
	ret =  read_timeout(connfd, revtime ); 
	if (ret != 0)
	{
		if (ret==-1 || errno == ETIMEDOUT)
		{
			ret = Sck_ErrTimeOut;
			Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func read_timeout() timeout");
			return ret;
		}
		else
		{
			Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func read_timeout() err");
			return ret;
		}	
	}
	
	int netdatalen = 0;
    ret = readn(connfd, &netdatalen,  4); 
	if (ret == -1)
	{
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func readn() err");
		return ret;
	}
	else if (ret < 4)
	{
		ret = Sck_ErrPeerClosed;
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func readn() err, peer closed");
		return ret;
	}
	
	int n;
	n = ntohl(netdatalen);
	tmpBuf = (unsigned char *)malloc(n+1);
	if (tmpBuf == NULL)
	{
		ret = Sck_ErrMalloc;
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"malloc() err");
		return ret;	
	}
	
	
	ret = readn(connfd, tmpBuf, n); 
	if (ret == -1)
	{
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"readn() err");
		return ret;
	}
	else if (ret < n)
	{
		ret = Sck_ErrPeerClosed;
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func readn() err,  peer closed");
		return ret;
	}
	
	*out = tmpBuf;
	*outlen = n;
	tmpBuf[n] = '\0';
	return 0;
}

int sckClient_closeconn(int connfd)
{
	if (connfd >=0 )
	{
		close(connfd);
	}
	return 0;
}

int sck_FreeMem(void **buf)
{
	if (buf == NULL)
	{
		return 0;
	}	
	if (*buf != NULL)
	{
		free (*buf);
	}
	*buf = NULL; 
	return 0;
}

int sckClient_destroy()
{
	return 0;
}

int sckServer_init(char *addr, int port, int *listenfd)
{
	int 	ret = 0;
	int mylistenfd;
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = inet_addr(addr);
	mylistenfd = socket(PF_INET, SOCK_STREAM, 0);
	if (mylistenfd < 0)
	{
		ret = errno ;
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func socket() err");
		return ret;
	}
		

	int on = 1;
	ret = setsockopt(mylistenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );
	if (ret < 0)
	{
		ret = errno ;
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func setsockopt() err");
		return ret;
	}
	

	ret = bind(mylistenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	if (ret < 0)
	{
		ret = errno ;
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func bind() err");
		return ret;
	}
		
	ret = listen(mylistenfd, SOMAXCONN);
	if (ret < 0)
	{
		ret = errno ;
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func listen() err");
		return ret;
	}
		
	*listenfd = mylistenfd;

	return 0;
}

int accept_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds)
{
	int ret;
	socklen_t addrlen = sizeof(struct sockaddr_in);

	if (wait_seconds > 0)
	{
		fd_set accept_fdset;
		struct timeval timeout;
		FD_ZERO(&accept_fdset);
		FD_SET(fd, &accept_fdset);
		timeout.tv_sec = wait_seconds;
		timeout.tv_usec = 0;
		do
		{
			ret = select(fd + 1, &accept_fdset, NULL, NULL, &timeout);
		} while (ret < 0 && errno == EINTR);
		if (ret == -1)
			return -1;
		else if (ret == 0)
		{
			errno = ETIMEDOUT;
			return -1;
		}
	}

	if (addr != NULL)
		ret = accept(fd, (struct sockaddr*)addr, &addrlen); 
	else
		ret = accept(fd, NULL, NULL);
		if (ret == -1)
		{
			ret = errno;
			Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func accept() err");
			return ret;
		}
	

	return ret;
}

int sckServer_accept(int listenfd,  int timeout, int *connfd)
{
	int	ret = 0;
	
	ret = accept_timeout(listenfd, NULL, (unsigned int) timeout);
	if (ret < 0)
	{
		if (ret == -1 && errno == ETIMEDOUT)
		{
			ret = Sck_ErrTimeOut;
			return ret;
		}
		else
		{
			ret = errno;
			Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func accept_timeout() err");
			return ret;
		}
	}
	
	*connfd = ret;
	return 0;
}

int sckServer_send(int connfd, int timeout, unsigned char *data, int datalen)
{
	int 	ret = 0;
	
	ret = write_timeout(connfd, timeout);
	if (ret == 0)
	{
		int writed = 0;
		unsigned char *netdata = ( unsigned char *)malloc(datalen + 4);
		if ( netdata == NULL)
		{
			ret = Sck_ErrMalloc;
			Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func malloc() err");
			return ret;
		}
		int netlen = htonl(datalen);
		memcpy(netdata, &netlen, 4);
		memcpy(netdata+4, data, datalen);
		
		writed = writen(connfd, netdata, datalen + 4);
		if (writed < (datalen + 4) )
		{
			if (netdata != NULL) 
			{
				free(netdata);
				netdata = NULL;
			}
			return writed;
		}
		  
	}
	
	if (ret < 0)
	{
		if (ret == -1 && errno == ETIMEDOUT)
		{
			ret = Sck_ErrTimeOut;
			Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func sckServer_send() err");
			return ret;
		}
		return ret;
	}
	
	return ret;
}

int sckServer_rev(int  connfd, int timeout, unsigned char **out, int *outlen)
{
		
	int		ret = 0;
	unsigned char *tmpBuf = NULL;
	
	if (out==NULL || outlen==NULL)
	{
		ret = Sck_ErrParam;
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func sckServer_rev() err, check (out==NULL || outlen==NULL)");
		return ret;
	}
	
	ret =  read_timeout(connfd, timeout); 
	if (ret != 0)
	{
		if (ret==-1 || errno == ETIMEDOUT)
		{
			ret = Sck_ErrTimeOut;
			Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func read_timeout() err");
			return ret;
		}
		else
		{
			Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func read_timeout() err");
			return ret;
		}	
	}
	
	int netdatalen = 0;
  ret = readn(connfd, &netdatalen,  4); 
	if (ret == -1)
	{
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func readn() err");
		return ret;
	}
	else if (ret < 4)
	{
		ret = Sck_ErrPeerClosed;
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func readn() err, peer closed");
		return ret;
	}
	
	int n;
	n = ntohl(netdatalen);
	
	tmpBuf = (unsigned char *)malloc(n+1);
	if (tmpBuf == NULL)
	{
		ret = Sck_ErrMalloc;
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func malloc() err");
		return ret;	
	}
	
	ret = readn(connfd, tmpBuf, n); 
	if (ret == -1)
	{
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func readn() err");
		return ret;
	}
	else if (ret < n)
	{
		ret = Sck_ErrPeerClosed;
		Socket_Log(__FILE__, __LINE__,SocketLevel[4], ret,"func readn() err. peer closed");
		return ret;
	}
	
	*out = tmpBuf; 
	*outlen = n;
	tmpBuf[n] = '\0';
	
	return 0;
}

int sckServer_close(int connfd)
{
	close(connfd);	
	return 0;
}

int sckServer_destroy()
{
	return 0;
}