#include "loop.h"
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include "channel.h"

CLoop::CLoop()
{
	epollfd_ = epoll_create1(0);
	if (epollfd_ == -1)
		std::cerr << "epoll_create1 failed\n";
	
	events_.resize(16);
}

CLoop::~CLoop()
{
	::close(epollfd_);
}

void CLoop::add_read_event(int fd, Channel* channel)
{
	struct epoll_event ev;
	ev.data.ptr = channel;
	ev.events = EPOLLIN | EPOLLPRI;
	if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev) < 0)
		std::cerr << "epoll_ctl add read fd: " << fd << "error" << std::endl;
}
void CLoop::add_write_event(int fd, Channel* channel)
{
	struct epoll_event ev;
	ev.data.ptr = channel;
	ev.events = EPOLLOUT;
	if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev) < 0)
		std::cerr << "epoll_ctl add write fd: " << fd << "error" << std::endl;
}
void CLoop::delete_write_event(int fd, Channel* channel)
{
	struct epoll_event ev;
	ev.data.ptr = channel;
	ev.events = EPOLLOUT;
	if (epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, &ev) < 0)
		std::cerr << "epoll_ctl del write fd: " << fd << "error" << std::endl;
}

void CLoop::start()
{
	run_ = true;
	while (run_)
	{
		auto nevent = epoll_wait(epollfd_, &events_[0], events_.size(), -1);
		if (nevent < 0)
		{
			if (errno == EINTR)
				continue;
			std::cout << "errno: " << errno << std::endl;
			break;
		}
		else if (nevent > 0)
		{
			if (nevent == events_.size())
				events_.resize(nevent * 2);
			
			for (int i = 0; i < nevent; ++i)
			{
				Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
				if (events_[i].events & (EPOLLERR | EPOLLHUP) && !(events_[i].events & EPOLLIN))
					channel->handle_error();
				
				if (events_[i].events & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
					channel->handle_read();
				
				if (events_[i].events & EPOLLOUT)
					channel->handle_write();
			}
		}
	}
}