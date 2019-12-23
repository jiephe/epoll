#include "channel.h"
#include "loop.h"
#include <iostream>
#include <sys/epoll.h>

Channel::Channel(CLoop* loop, int fd)
	: loop_(loop), fd_(fd)
	, revents_(0)
{}

void Channel::start()
{
	revents_ |= (EPOLLIN | EPOLLPRI);
	loop_->add_event(fd_, this);
}

Channel::~Channel()
{}

void Channel::disableWrite()
{
	revents_ &= ~EPOLLOUT;
	loop_->modify_event(fd_, this);
}
	
void Channel::enableWrite()
{
	revents_ |= EPOLLOUT;
	loop_->modify_event(fd_, this);
}

bool Channel::isWriting()
{
	return (revents_ & EPOLLOUT);
}

void Channel::removeAllEvent()
{
	loop_->delete_event(fd_, this);
}

void Channel::handle_read()
{	
	//std::cout << "fd: " << fd_ << " handle_read" << std::endl;
	if (read_cb_)
		read_cb_();
}
	
void Channel::handle_write()
{
	if (write_cb_)
		write_cb_();
}
	
void Channel::handle_close()
{
	if (close_cb_)
		close_cb_();
}

void Channel::handle_error()
{
	if (error_cb_)
		error_cb_();
}