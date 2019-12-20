#include "tcp_server.h"
#include "acceptor.h"
#include "loop.h"
#include "connection.h"
#include "runtime.h"

CTcpServer::CTcpServer(CRuntime* runtime, CLoop* loop, const std::string& port)
	: runtime_(runtime)
	, loop_(loop)
{
	accept_.reset(new CAcceptor(loop_, port));
}

CTcpServer::~CTcpServer()
{}

void CTcpServer::start()
{		
	accept_->set_new_connection_cb(std::bind(&CTcpServer::OnConnection, this, std::placeholders::_1));
	accept_->start();
}

void CTcpServer::OnData(const connectionPtr& conn, void* data, int len)
{
	msg_cb_(conn, data, len);
}

/*
 void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
  // FIXME: unsafe
  loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn)
{
  loop_->assertInLoopThread();
  LOG_INFO << "TcpServer::removeConnectionInLoop [" << name_
           << "] - connection " << conn->name();
  size_t n = connections_.erase(conn->name());
  (void)n;
  assert(n == 1);
  EventLoop* ioLoop = conn->getLoop();
  ioLoop->queueInLoop(
      std::bind(&TcpConnection::connectDestroyed, conn));
}
 **/

void CTcpServer::OnError(int sockfd)
{
	//TODO connections_��base_loop_��� ����ɾ��ʱҪ�ص�base_loop_����
	//����ÿ��conn���fd_�Ǻ�loop��ص� ����fd_�ص��Լ���loop�ر�
	auto itor = connections_.find(sockfd);
	if (itor != connections_.end())
		connections_.erase(itor);
}

void CTcpServer::OnConnection(int fd)
{
	CLoop* loop = runtime_->get_next_loop();
	auto conn = std::make_shared<Connection>(loop, fd);
	conn->start();
	conn->set_data_callback(std::bind(&CTcpServer::OnData, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	conn->set_error_callback(std::bind(&CTcpServer::OnError, this, std::placeholders::_1));
	connections_[fd] = conn;
	new_conn_cb_(conn);
}