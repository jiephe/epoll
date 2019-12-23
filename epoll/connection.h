#pragma once

#include "internal.h"
#include <vector>

class Connection : public std::enable_shared_from_this<Connection>
{
public:
	Connection(CLoop* loop, int fd);
	~Connection();
	
	void destroyConnection();
	
	void start();
	
	void set_data_callback(const MessageCallback& cb) { msg_cb_ = cb; }
	
	void set_close_callback(const CloseCallback& cb);
	
	void set_write_complete_callback(const WriteCompleteCallback& cb) { complete_cb_ = cb;}

	int get_fd();
	
	CLoop* get_loop();
	
public:
	int send(void* data, int len);
	
public:
	void OnRead();
	
	void OnWrite();
	
	void OnClose();
	
	void OnError();
	
private:
	CLoop*					loop_;
	
	int						fd_;
		
	std::vector<char>		out_buf_;
	
	Channel*				channel_;
	
	std::vector<char>		in_buf_;
	
	MessageCallback			msg_cb_;
	
	WriteCompleteCallback	complete_cb_;
	
	CloseCallback			close_cb_;
};