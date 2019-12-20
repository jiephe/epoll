#pragma once

#include <vector>
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
};