#include "tcp_server.h"
#include "acceptor.h"
#include "loop.h"
#include "connection.h"
#include "runtime.h"
#include "loop_manager.h"
#include <assert.h>
#include <iostream>

CTcpServer::CTcpServer(CRuntime* runtime, CLoop* loop, const std::string& port, int threadnum)
	: runtime_(runtime)
	, loop_(loop)
	, loop_manager_(new CLoopManger(loop_, threadnum))
	, accept_(new CAcceptor(loop_, port))
{}

CTcpServer::~CTcpServer()
{}

void CTcpServer::OnData(const connectionPtr& conn, void* data, int len)
{
	msg_cb_(conn, data, len);
}

void CTcpServer::removeConnection(const connectionPtr& conn)
{
	//std::cout << "removeConnection fd: " << conn->get_fd() << std::endl;
	size_t n = connections_.erase(conn->get_fd());
	assert(n == 1);
	conn->get_loop()->runInLoop(std::bind(&Connection::destroyConnection, conn));
}

void CTcpServer::OnClose(const connectionPtr& conn)
{
	//connections_属于base_loop_ 所以删除时要回到base_loop_处理
	//但如果是多线程 每个conn里的fd_是分别一个loop的 所以fd_回到自己的loop关闭	
	//std::cout << "OnClose fd: " << conn->get_fd() << std::endl;
	loop_->runInLoop(std::bind(&CTcpServer::removeConnection, this, conn));
}

void CTcpServer::OnConnection(int fd)
{
	CLoop* loop = loop_manager_->get_next_loop();
	auto conn = std::make_shared<Connection>(loop, fd);
	conn->start();
	conn->set_data_callback(std::bind(&CTcpServer::OnData, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	conn->set_close_callback(std::bind(&CTcpServer::OnClose, this, std::placeholders::_1));
	connections_[fd] = conn;
	new_conn_cb_(conn);
}

void CTcpServer::start()
{	
	loop_manager_->start();
	accept_->set_new_connection_cb(std::bind(&CTcpServer::OnConnection, this, std::placeholders::_1));
	accept_->start();
	
	std::cout << "start server success!" << std::endl;
}

