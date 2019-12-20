#include "connection.h"
#include "loop.h"
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <assert.h>
#include "channel.h"

#include <errno.h>
#include <string.h>
__thread char t_errnobuf[512];
const char* strerror_tl(int savedErrno)
{
	return strerror_r(savedErrno, t_errnobuf, sizeof t_errnobuf);
}

Connection::Connection(CLoop* loop, int fd)
	: loop_(loop), fd_(fd)
	, channel_(new Channel(loop, fd))
{}

Connection::~Connection()
{}

int Connection::get_fd()
{
	return fd_;
}

CLoop* Connection::get_loop()
{
	return loop_;
}

void Connection::destroyConnection()
{
	if (fd_ > 0)
		close(fd_);
	
	std::cout << "thread: " << std::this_thread::get_id() << "~Connection Close: " << fd_ << std::endl;
}

void Connection::start()
{
	channel_->set_read_cb(std::bind(&Connection::OnRead, this));
	channel_->set_write_cb(std::bind(&Connection::OnWrite, this));
	channel_->set_close_cb(std::bind(&Connection::OnClose, this));
	channel_->set_error_cb(std::bind(&Connection::OnError, this));
	channel_->start();
}

void Connection::OnRead()
{	
	std::vector<char> tmp_buf;
	tmp_buf.resize(8192);
	
	while (true)
	{
		auto count = read(fd_, &tmp_buf[0], tmp_buf.size());
		if (count == -1)
		{
			if (errno == EAGAIN) // read all data
				break;
			std::cout << "OnRead -1 : " << fd_ << " errno: " << errno << std::endl;
			OnError();
			return;
		}
		else if (count == 0) // EOF - remote closed connection
		{
			OnClose();
			return;
		}
		
		//std::cout << fd_ << " read data: " <<  count << std::endl;	
		in_buf_.insert(in_buf_.end(), &tmp_buf[0], &tmp_buf[0] + count);
	}

	if (msg_cb_)
	{
		msg_cb_(shared_from_this(), &in_buf_[0], in_buf_.size());
		in_buf_.clear();
	}
}
	
void Connection::OnWrite()
{
}

void Connection::set_close_callback(const CloseCallback& cb)
{
	close_cb_ = cb;
}

void Connection::OnClose()
{
	std::cout << "OnClose " << fd_ << "\n";
	//必须放在这里 现在本线程把event去除 不然当其他线程处理[loop_->runInLoop(std::bind(&CTcpServer::removeConnection, this, conn));]时
	// 本线程还一直on_read(因为是LT(level triggered)默认 导致CTcpServer::OnClose()一直被调用)
	channel_->removeAllEvent();
	connectionPtr self(shared_from_this());
	close_cb_(self);
}

int getSocketError(int sockfd)
{
	int optval;
	socklen_t optlen = static_cast<socklen_t>(sizeof optval);
	if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
		return errno;
	else
		return optval;
}
	
void Connection::OnError()
{
	int err = getSocketError(fd_);
	std::cout << "TcpConnection::handleError [" << fd_ << "] - SO_ERROR = " << err << " " << strerror_tl(err);
}