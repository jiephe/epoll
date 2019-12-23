#pragma once

#include <atomic>
#include <vector>
#include <thread>
#include <mutex>
#include "internal.h"

class CLoop
{
public:
	CLoop();
	~CLoop();
	
public:
	void init();
	
	void start();
	
	bool is_self_loop();
	
	void wakeup();
	
	void runInLoop(Functor cb);
	
	void queueInLoop(Functor cb);
	
	void doPendingFunctors();
		
	void add_event(int fd, Channel* channel);
	
	void delete_event(int fd, Channel* channel);
	
	void modify_event(int fd, Channel* channel);
	
public:
	void OnWakeRead();
	
private:
	int									epollfd_;
		
	int									wakeupFd_;
	
	Channel*							wake_channel_;
	
	std::atomic<bool>					run_;
	
	std::vector<struct epoll_event>		events_;
	
	std::thread::id						current_thead_id_;
	
	std::vector<Functor>				pendingFunctors_;
	
	bool								callingPendingFunctors_;
	
	std::mutex							mutex_;
};