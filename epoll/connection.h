#pragma once

#include "internal.h"
#include <vector>

class Connection : public std::enable_shared_from_this<Connection>
{
public:
	Connection(CLoop* loop, int fd);
	~Connection();
	
	void start();
	
	void set_data_callback(const MessageCallback& cb)
	{
		msg_cb_ = cb;
	}
	void set_error_callback(const ErrorCallback& cb)
	{
		error_cb_ = cb;
	}
	
public:
	void OnRead();
	
	void OnWrite();
	
	void OnError();
	
private:
	CLoop*					loop_;
	
	int						fd_;
	
	Channel*				channel_;
	
	std::vector<char>		in_buf_;
	
	MessageCallback			msg_cb_;
	
	ErrorCallback			error_cb_;
};