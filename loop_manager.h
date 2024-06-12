#pragma once

#include "internal.h"
#include <thread>
#include <vector>

class CLoopManger
{
public:
	CLoopManger(CLoop* loop, int threadnum);
	~CLoopManger();
	
public:
	void start();

public:
	CLoop* get_next_loop();
		
private:	
	std::vector<CLoop*>				loops_;

	CLoop*							base_loop_;
	
	uint32_t						next_;	
	
	int								thread_num_;
	
	std::vector<std::shared_ptr<std::thread>> threads_;
};