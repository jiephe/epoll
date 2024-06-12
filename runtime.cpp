#include "runtime.h"
#include "acceptor.h"
#include "loop.h"
#include "connection.h"
#include "tcp_server.h"
#include <iostream>
#include "common.h"

CRuntime::CRuntime()
{}

CRuntime::~CRuntime()
{}

void CRuntime::onConnection(const connectionPtr& conn)
{
	std::cout << "new connection: " << conn.get() << std::endl;	
}

void CRuntime::onWriteComplete(const connectionPtr& conn)
{
	std::cout << "thread: " << getCurrentThreadId() << " conn: " << conn.get() << " write complete " << std::endl;	
}

void CRuntime::onMessage(const connectionPtr& conn, void* data, int len)
{
	std::cout << "thread: " << getCurrentThreadId() << " conn: " << conn.get() << " len: " << len << std::endl;	
	conn->set_write_complete_callback(std::bind(&CRuntime::onWriteComplete, this, std::placeholders::_1));
	
	std::vector<char> send_data(10, 'a');
	conn->send(&send_data[0], send_data.size());
}

void CRuntime::start(int threadnum, const std::string& port)
{
	CLoop loop;
	loop.init();
	CTcpServer server(this, &loop, port, threadnum);
	server.set_new_connection_cb(std::bind(&CRuntime::onConnection, this, std::placeholders::_1));
	server.set_msg_cb(std::bind(&CRuntime::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	server.start();
	
	struct sigaction action;
	action.sa_flags = SA_SIGINFO;
	action.sa_sigaction = sigroutine;
	sigaction(SIGUSR1, &action, NULL);  		// kill -USR1 pid
	sigaction(SIGUSR2, &action, NULL);  		// kill -USR2 pid
	sigaction(SIGTERM, &action, NULL);  		// kill pid | kill -15 pid
	sigaction(SIGINT, &action, NULL);  			// CTRL+C
	
	loop.start();
}

void CRuntime::sigroutine(int signo, siginfo_t *info, void *extra)
{
	switch (signo) {
	case SIGUSR1:
		{
			
		}
		break;
	case SIGUSR2:
		{
			
		}
		break;
	case SIGTERM:
		{
			
		}
		break;
	case SIGINT:
		{
			
		}
		break;
	}
}
