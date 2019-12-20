#include "runtime.h"
#include "acceptor.h"
#include "loop.h"
#include "connection.h"
#include "tcp_server.h"
#include <iostream>

CRuntime::CRuntime()
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
	loop.init();
	CTcpServer server(this, &loop, port, threadnum);
	server.set_new_connection_cb(std::bind(&CRuntime::onConnection, this, std::placeholders::_1));
	server.set_msg_cb(std::bind(&CRuntime::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	server.start();
	loop.start();
}

