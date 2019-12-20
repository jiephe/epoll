#pragma once

#include <map>
#include "internal.h"
#include "acceptor.h"

class CTcpServer
{
public:
	CTcpServer(CRuntime* runtime, CLoop* loop, const std::string& port);
	
	~CTcpServer();
	
public:
	void start();
	
	void set_new_connection_cb(const ConnectionCallback& cb) {new_conn_cb_ = cb;}
	void set_msg_cb(const MessageCallback& cb) {msg_cb_ = cb; }
	
	void OnConnection(int fd);
	
	void OnData(const connectionPtr& conn, void* data, int len);
	
	void OnError(int sockfd);
			
private:	
	std::map<int, connectionPtr>			connections_;	
	CLoop*									loop_;
	CRuntime*								runtime_;
	ConnectionCallback						new_conn_cb_;
	MessageCallback							msg_cb_;
	acceptorPtr								accept_;
};