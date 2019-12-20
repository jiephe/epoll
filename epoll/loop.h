#pragma once

#include <atomic>
#include <vector>
#include "internal.h"

class CLoop
{
public:
	CLoop();
	
	~CLoop();
	
public:
	void start();
		
	void add_read_event(int fd, Channel* channel);
	
	void add_write_event(int fd, Channel* channel);
	
	void delete_write_event(int fd, Channel* channel);
	
private:
	int									epollfd_;
		
	std::atomic<bool>					run_;
	
	std::vector<struct epoll_event>		events_;
};