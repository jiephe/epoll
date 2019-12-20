#include "runtime.h"
#include "acceptor.h"
#include "loop.h"
#include "connection.h"
#include "tcp_server.h"
#include <iostream>

CRuntime::CRuntime()
	:next_(0)
{}

CRuntime::~CRuntime()
{}

void CRuntime::onConnection(const connectionPtr& conn)
{
	//std::cout << "new connection: " << conn.get() << std::endl;	
}

void CRuntime::onMessage(const connectionPtr& conn, void* data, int len)
{
	std::cout << "thread: " << std::this_thread::get_id() << " conn: " << conn.get() << " len: " << len << std::endl;	
}

void CRuntime::start(int threadnum, const std::string& port)
{
	CLoop loop;
	CTcpServer server(this, &loop, port);
	
	for (int i = 0; i < threadnum; ++i)
	{		
		auto lp = new CLoop();
		loops_.emplace_back(lp);
		auto th = std::make_shared<std::thread>([lp](){lp->start();});
		threads_.emplace_back(th);
	}
	
	server.set_new_connection_cb(std::bind(&CRuntime::onConnection, this, std::placeholders::_1));
	server.set_msg_cb(std::bind(&CRuntime::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	server.start();
	
	base_loop_ = &loop;
	loop.start();
}

CLoop* CRuntime::get_next_loop()
{
	if (loops_.empty())
		return base_loop_;
	else
	{
		auto loop = loops_[next_];
		++next_;
		if (next_ >= loops_.size())
			next_ = 0;
		return loop;
	}
}
