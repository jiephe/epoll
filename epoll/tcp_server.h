#pragma once

#include <map>
#include "internal.h"

class CTcpServer
{
public:
	CTcpServer(CRuntime* runtime, CLoop* loop, const std::string& port, int threadnum);
	
	~CTcpServer();
	
public:
	void start();
	
	void set_new_connection_cb(const ConnectionCallback& cb) {new_conn_cb_ = cb;}
	void set_msg_cb(const MessageCallback& cb) {msg_cb_ = cb; }
	
	void OnConnection(int fd);
	
	void OnData(const connectionPtr& conn, void* data, int len);
	
	void OnClose(const connectionPtr& conn);
		
	void removeConnection(const connectionPtr& conn);
			
private:	
	CRuntime*								runtime_;
	CLoop*									loop_;
	loopManagerPtr							loop_manager_;
	ConnectionCallback						new_conn_cb_;
	MessageCallback							msg_cb_;
	acceptorPtr								accept_;
	std::map<int, connectionPtr>			connections_;	
};