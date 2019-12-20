#include "acceptor.h"
#include "loop.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <iostream>
#include <cstring>
#include <assert.h>
#include "channel.h"

CAcceptor::CAcceptor(CLoop* loop, const std::string& port)
	: loop_(loop)
	, port_(port)
{}

CAcceptor::~CAcceptor()
{
}

bool CAcceptor::start()
{
	fd_ = create_and_bind(port_);
	if (fd_ == -1)
		return false;

	if (!make_socket_nonblocking(fd_))
		return false;

	if (listen(fd_, SOMAXCONN) == -1)
	{
		std::cerr << "[E] listen failed\n";
		return false;
	}
	
	accept_channel_ = new Channel(loop_, fd_);
	accept_channel_->start();
	accept_channel_->set_read_cb(std::bind(&CAcceptor::accept_connection, this));
	return true;
}

void CAcceptor::accept_connection()
{
	struct sockaddr in_addr;
	socklen_t in_len = sizeof(in_addr);
	int infd = accept(fd_, &in_addr, &in_len);
	if (infd == -1)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK) // Done processing incoming connections
			return;
		else
		{
			std::cerr << "[E] accept failed\n";
			return;
		}
	}

	std::string hbuf(NI_MAXHOST, '\0');
	std::string sbuf(NI_MAXSERV, '\0');
	if (getnameinfo(&in_addr,
		in_len,
		const_cast<char*>(hbuf.data()),
		hbuf.size(),
		const_cast<char*>(sbuf.data()),
		sbuf.size(),
		NI_NUMERICHOST | NI_NUMERICSERV) == 0)
	{
		//std::cout << "[I] Accepted connection on descriptor " << infd << "(host=" << hbuf << ", port=" << sbuf << ")" << "\n";
	}

	if (!make_socket_nonblocking(infd))
	{
		std::cerr << "[E] make_socket_nonblocking failed\n";
		return;
	}
	
	connection_cb_(infd);
}
	
int CAcceptor::create_and_bind(std::string const& port)
{
	struct addrinfo hints;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC; /* Return IPv4 and IPv6 choices */
	hints.ai_socktype = SOCK_STREAM; /* TCP */
	hints.ai_flags = AI_PASSIVE; /* All interfaces */

	struct addrinfo* result;
	int sockt = getaddrinfo(nullptr, port.c_str(), &hints, &result);
	if (sockt != 0)
	{
		std::cerr << "[E] getaddrinfo failed\n";
		return -1;
	}

	struct addrinfo* rp;
	int socketfd;
	for (rp = result; rp != nullptr; rp = rp->ai_next)
	{
		socketfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (socketfd == -1)
			continue;

		sockt = bind(socketfd, rp->ai_addr, rp->ai_addrlen);
		if (sockt == 0)
			break;

		close(socketfd);
	}

	if (rp == nullptr)
	{
		std::cerr << "[E] bind failed\n";
		return -1;
	}

	freeaddrinfo(result);
	return socketfd;
}

bool CAcceptor::make_socket_nonblocking(int socketfd)
{
	int flags = fcntl(socketfd, F_GETFL, 0);
	if (flags == -1)
	{
		std::cerr << "[E] fcntl failed (F_GETFL)\n";
		return false;
	}

	flags |= O_NONBLOCK;
	int s = fcntl(socketfd, F_SETFL, flags);
	if (s == -1)
	{
		std::cerr << "[E] fcntl failed (F_SETFL)\n";
		return false;
	}
	
	int flag = 1;
	setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int));
	return true;
}