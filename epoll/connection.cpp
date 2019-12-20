#include "connection.h"
#include "loop.h"
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <assert.h>
#include "channel.h"

Connection::Connection(CLoop* loop, int fd)
	: loop_(loop), fd_(fd)
{}

Connection::~Connection()
{
	if (fd_ > 0)
		close(fd_);
	
	std::cout << "~Connection Close: " << fd_ << std::endl;
}

void Connection::start()
{
	channel_ = new Channel(loop_, fd_);
	channel_->start();
	channel_->set_read_cb(std::bind(&Connection::OnRead, this));
	channel_->set_write_cb(std::bind(&Connection::OnWrite, this));
	channel_->set_error_cb(std::bind(&Connection::OnError, this));
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
			error_cb_(fd_);
			return;
		}
		else if (count == 0) // EOF - remote closed connection
		{
			//std::cout << "OnRead error " << fd_ << std::endl;
			error_cb_(fd_);
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
	
void Connection::OnError()
{
	std::cout << "OnError Close " << fd_ << "\n";
	error_cb_(fd_);
}