#pragma once

#include <vector>
#include <signal.h>
#include "internal.h"

class CRuntime
{
public:
	CRuntime();
	
	~CRuntime();
	
public:
	static void sigroutine(int signo, siginfo_t *info, void *extra);
	
public:
	void start(int threadnum, const std::string& port);
	
	void onConnection(const connectionPtr& conn);
	void onWriteComplete(const connectionPtr& conn);
	void onMessage(const connectionPtr& conn, void* data, int len);
};