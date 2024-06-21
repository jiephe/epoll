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
#include "err_string.h"

CAcceptor::CAcceptor(CLoop* loop, const std::string& port)
	: loop_(loop)
	, port_(port)
{}

CAcceptor::~CAcceptor()
{}

bool CAcceptor::start()
{
	fd_ = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);

	int flag = 1;
	::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int));

	flag = 1;
	::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(int));

	struct sockaddr_in servaddr;
    std::memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(std::stoi(port_));
    if (::bind(fd_, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        std::cerr << "Error binding socket" << std::endl;
        close(fd_);
        return false;
    }

	if (listen(fd_, SOMAXCONN) == -1) {
		std::cerr << "listen failed errno : " << errno << "errstring: " << strerror_tl(errno) << std::endl;
		return false;
	}
	
	accept_channel_ = new Channel(loop_, fd_);
	accept_channel_->set_read_cb(std::bind(&CAcceptor::accept_connection, this));
	accept_channel_->start();
	return true;
}

void CAcceptor::accept_connection()
{
	struct sockaddr in_addr;
	socklen_t in_len = sizeof(in_addr);
	int infd = accept(fd_, &in_addr, &in_len);
	if (infd == -1) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {	// Done processing incoming connections 
			return;
		} else {
			std::cerr << "accept failed errno : " << errno << "errstring: " << strerror_tl(errno) << std::endl;
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
		NI_NUMERICHOST | NI_NUMERICSERV) == 0) {
		//std::cout << "[I] Accepted connection on descriptor " << infd << "(host=" << hbuf << ", port=" << sbuf << ")" << "\n";
	}
	
	connection_cb_(infd);
}