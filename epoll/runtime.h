#pragma once

#include <vector>
#include <thread>
#include "internal.h"

class CRuntime
{
public:
	CRuntime();
	
	~CRuntime();
	
public:
	void start(int threadnum, const std::string& port);
	
	void onConnection(const connectionPtr& conn);
	void onMessage(const connectionPtr& conn, void* data, int len);
	
	CLoop* get_next_loop();
		
private:
	std::vector<CLoop*>							loops_;
	std::vector<std::shared_ptr<std::thread>>	threads_;
	uint32_t									next_;		
	CLoop*										base_loop_;
};