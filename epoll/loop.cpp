#include "loop.h"
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include "channel.h"

CLoop::CLoop()
{
	//为了保证loops_在主线程里操作 同时每个loop的epollfd_/wakeupFd_由属于的线程create
	//不在loop的构造函数里做任何操作
}

CLoop::~CLoop()
{
	::close(wakeupFd_);
	::close(epollfd_);
}

void CLoop::add_read_event(int fd, Channel* channel)
{
	struct epoll_event ev;
	ev.data.ptr = channel;
	ev.events = EPOLLIN | EPOLLPRI;
	if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev) < 0)
		std::cerr << "epoll_ctl add read fd: " << fd << " error: " << errno << std::endl;
}
void CLoop::delete_read_event(int fd, Channel* channel)
{
	struct epoll_event ev;
	ev.data.ptr = channel;
	ev.events = EPOLLIN | EPOLLPRI;
	if (epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, &ev) < 0)
		std::cerr << "epoll_ctl del read fd: " << fd << " error: " << errno << std::endl;
}
void CLoop::add_write_event(int fd, Channel* channel)
{
	struct epoll_event ev;
	ev.data.ptr = channel;
	ev.events = EPOLLOUT;
	if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev) < 0)
		std::cerr << "epoll_ctl add write fd: " << fd << "error: " << errno << std::endl;
}
void CLoop::delete_write_event(int fd, Channel* channel)
{
	struct epoll_event ev;
	ev.data.ptr = channel;
	ev.events = EPOLLOUT;
	if (epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, &ev) < 0)
		std::cerr << "epoll_ctl del write fd: " << fd << "error: " << errno << std::endl;
}

bool CLoop::is_self_loop()
{
	return (current_thead_id_ == std::this_thread::get_id());
}

void CLoop::runInLoop(Functor cb)
{
	if (is_self_loop())
		cb();
	else
		queueInLoop(std::move(cb));
}

void CLoop::queueInLoop(Functor cb)
{
	{
		std::lock_guard<std::mutex> lock(mutex_);
		pendingFunctors_.emplace_back(std::move(cb));
	}

	if (!is_self_loop() || callingPendingFunctors_)
		wakeup();
}

void CLoop::wakeup()
{
	uint64_t one = 1;
	ssize_t n = write(wakeupFd_, &one, sizeof one);
	if (n != sizeof one)
		std::cout << "EventLoop::wakeup() writes " << n << " bytes instead of 8" << std::endl;
}

void CLoop::OnWakeRead()
{
	uint64_t one = 1;
	ssize_t n = read(wakeupFd_, &one, sizeof one);
	if (n != sizeof one)
		std::cout << "EventLoop::handleRead() reads " << n << " bytes instead of 8 << std::endl;";
}
	
void CLoop::init()
{
	epollfd_ = epoll_create1(EPOLL_CLOEXEC);
	if (epollfd_ == -1)
		std::cerr << "epoll_create1 failed\n";
	
	events_.resize(16);
	
	wakeupFd_ = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (wakeupFd_ < 0)
	{
		std::cout << "Failed in eventfd" << std::endl;
		abort();
	}
	
	wake_channel_ = new Channel(this, wakeupFd_);
	wake_channel_->set_read_cb(std::bind(&CLoop::OnWakeRead, this));
	wake_channel_->start();
	
	current_thead_id_ = std::this_thread::get_id();
	
	run_ = true;
	
	callingPendingFunctors_ = false;
}

void CLoop::start()
{
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
			if ((size_t)nevent == events_.size())
				events_.resize(nevent * 2);
			
			for (int i = 0; i < nevent; ++i)
			{
				Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
				if (events_[i].events & EPOLLHUP && !(events_[i].events & EPOLLIN))
					channel->handle_close();
				
				if (events_[i].events & EPOLLERR)
					channel->handle_error();
				
				if (events_[i].events & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
				{
					//std::cout << "read thread: " << current_thead_id_ << std::endl;
					channel->handle_read();
				}
					
				if (events_[i].events & EPOLLOUT)
					channel->handle_write();
			}
			
			doPendingFunctors();
		}
	}
}

void CLoop::doPendingFunctors()
{
	std::vector<Functor> functors;
	callingPendingFunctors_ = true;

	{
		std::lock_guard<std::mutex> lock(mutex_);
		functors.swap(pendingFunctors_);
	}

	for (const Functor& functor : functors)
		functor();
	callingPendingFunctors_ = false;
}