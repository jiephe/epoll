#include "connection.h"
#include "loop.h"
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <assert.h>
#include "channel.h"
#include "err_string.h"
#include "common.h"

//#define TEST_EPOLL_LT

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
	if (fd_ > 0) {
		close(fd_);
	}

	std::cout << "thread: " << getCurrentThreadId() << "~Connection Close: " << fd_ << std::endl;
}

void Connection::start()
{
	channel_->set_read_cb(std::bind(&Connection::OnRead, this));
	channel_->set_write_cb(std::bind(&Connection::OnWrite, this));
	channel_->set_close_cb(std::bind(&Connection::OnClose, this));
	channel_->set_error_cb(std::bind(&Connection::OnError, this));
	channel_->start();
}

int Connection::send(void* data, int len)
{
	int nwrote = 0;
	int remain = len;
	bool faultError = false;
	if (!channel_->isWriting() && out_buf_.empty()) {
		nwrote = write(fd_, data, len);
		if (nwrote >= 0) {
			remain = len - nwrote; 
			if (remain == 0)
				complete_cb_(shared_from_this());
		} else {
			if (errno != EWOULDBLOCK) {
				std::cout << "send error : " << errno << std::endl;
				if (errno == EPIPE || errno == ECONNRESET) {
					faultError = true;
				}
			}
		}
	}
	
	if (!faultError && remain > 0) {
		out_buf_.insert(out_buf_.end(), (char*)data + nwrote, (char*)data + len);
		channel_->enableWrite();
	}
	
	return 0;
}

void Connection::OnWrite()
{
	if (channel_->isWriting()) {
		int nwrote = write(fd_, &out_buf_[0], out_buf_.size());
		if (nwrote > 0) {
			if (nwrote == out_buf_.size()) {
				channel_->disableWrite();
				out_buf_.clear();
				complete_cb_(shared_from_this());
			} else {
				std::vector<char> tmp;
				tmp.insert(tmp.end(), &out_buf_[0] + nwrote, &out_buf_[0] + out_buf_.size());
				out_buf_.swap(tmp);
			}
		} else {
			std::cout << "OnWrite error : " << errno << std::endl;
		}	
	}
}

void Connection::OnRead()
{	
	std::cout << " OnRead callback: " << fd_ << std::endl;

	std::vector<char> tmp_buf;
	tmp_buf.resize(8192);
	
	while (true) {
#ifndef TEST_EPOLL_LT
		auto count = read(fd_, &tmp_buf[0], tmp_buf.size());
#else
		auto count = read(fd_, &tmp_buf[0], 3);
#endif
		if (count == -1) {
			if (errno == EAGAIN)  {// read all data
				break;
			}
			std::cout << "OnRead -1 : " << fd_ << " errno: " << errno << std::endl;
			OnError();
			return;
		} else if (count == 0) { // EOF - remote closed connection
			OnClose();
			return;
		}

		//std::cout << fd_ << " read data: " <<  count << std::endl;	
		in_buf_.insert(in_buf_.end(), &tmp_buf[0], &tmp_buf[0] + count);
#ifdef TEST_EPOLL_LT
		break;
#endif
	}

	if (msg_cb_) {
		msg_cb_(shared_from_this(), &in_buf_[0], in_buf_.size());
		in_buf_.clear();
	}
}
	
void Connection::set_close_callback(const CloseCallback& cb)
{
	close_cb_ = cb;
}

void Connection::OnClose()
{
	std::cout << "OnClose " << fd_ << std::endl;
	channel_->removeAllEvent();
	connectionPtr self(shared_from_this());
	close_cb_(self);
}

int getSocketError(int sockfd)
{
	int optval;
	socklen_t optlen = static_cast<socklen_t>(sizeof optval);
	if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
		return errno;
	} else {
		return optval;
	}
}
	
void Connection::OnError()
{
	int err = getSocketError(fd_);
	std::cout << "handleError fd: " << fd_ << " errno = " << err << " errstring: " << strerror_tl(err) << std::endl;
}